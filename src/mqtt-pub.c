//#include "stdio.h"
//#include "stdlib.h"
//#include "string.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MQTTClient.h"
#include <unistd.h>
#include "circlebuff.h"

const char *gs_siteId="18071301";
#define ADDRESS     "tcp://47.106.81.63:1883"
//#define ADDRESS     "tcp://120.77.254.235:2183"

char gs_report[50];
char g_mqTopicCtrl[50];
char g_mqTopicResponse[50];
char g_mqClientId[50];
static char temp[50];
static const char *report="/D2S";
static const char *response="/response";
static const char *rec="/S2D";
static const char *topicFront="SBM01/";
char g_firstConnetFlag=0;
#define QOS         1
#define TIMEOUT     5000L


char pubBuf[2048];
extern MQTT_SENT_BUFF_T   mqSentBuff;

//----------------------------------------------------------------------
extern DATAS_BUFF_T   comBuff0;
volatile MQTTClient_deliveryToken deliveredtoken;
int g_connectErrCount=0;
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    //printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

int msgarrvd(void *context, char *topicName, int topicLen, MQTTClient_message *message)
{
    int i;
    char* payloadptr;

    printf(" new mqtt Message arrived:");
    printf(" topic: %s", topicName);
	printf(" topicLen:%d\n ",message->payloadlen);

    payloadptr = message->payload;

	pthread_mutex_lock(&comBuff0.lock);

	AP_circleBuff_WritePacket(payloadptr++,message->payloadlen,MQTPA2DTU);
	pthread_cond_signal(&comBuff0.newPacketFlag);
	pthread_mutex_unlock(&comBuff0.lock);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost\n");
    printf("     cause: %s\n", cause);
}

//----------------------------------------------------------------------
void *mqtt_pub_treat(int argc, char* argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_deliveryToken token;
    int rc, i;


   	gs_report[0]=0;
   	strcat(gs_report,topicFront);
   	strcat(gs_report,gs_siteId);
	strcat(gs_report,report);

   	g_mqTopicCtrl[0]=0;
   	strcat(g_mqTopicCtrl,topicFront);
   	strcat(g_mqTopicCtrl,gs_siteId);
   	strcat(g_mqTopicCtrl,rec);

   	g_mqTopicResponse[0]=0;
   	strcat(g_mqTopicResponse,topicFront);
   	strcat(g_mqTopicResponse,gs_siteId);
   	strcat(g_mqTopicResponse,response);

   	g_mqClientId[0]=0;
 	strcat(g_mqClientId,topicFront);
   	strcat(g_mqClientId,gs_siteId);

	conn_opts.username="jxkj007";
	conn_opts.password="001";
    //-----------------------------------------
	//strcat(conn_opts.will->topicName,"{\"willstreams\":");
	//conn_opts.will->topicName=0;
	//conn_opts.will->topicName=gs_siteId;
	//strcat(conn_opts.will->topicName,"test");
	//strcat(conn_opts.will->topicName,"}");
	//sprintf(conn_opts.will->topicName,"%s%s%s", "{\"willstreams\":",g_mqClientId,"}");
//	strcat(g_mqClientId,gs_siteId);
//	conn_opts.will->topicName=temp;
	//------------------------------------
	//printf("-------enter mqtt_pub_treat AMTF----------------- \n");
    MQTTClient_create(&client, ADDRESS, g_mqClientId, MQTTCLIENT_PERSISTENCE_NONE, NULL);

    conn_opts.keepAliveInterval = 60;
    conn_opts.cleansession = 1;
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		printf("start up Failed to connect, return code %d\n", rc);
		//exit(EXIT_FAILURE);
	}

	MQTTClient_subscribe(client, g_mqTopicCtrl, QOS);

	while(1)
	{
		while(!MQTTClient_isConnected(client)){
			if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
				{
					printf(" pub retry to connect failure, return code %d\n", rc);
				}
			else
			{
				MQTTClient_subscribe(client, g_mqTopicCtrl, QOS);
			}
			g_connectErrCount++;
			if(g_connectErrCount>200){
				printf(" -----------reboot--------------\n");
				system("reboot");
			}
			sleep(3);
		};
		g_firstConnetFlag=1;
		g_connectErrCount=0;
		pthread_mutex_lock(&mqSentBuff.lock);
		pthread_cond_wait(&mqSentBuff.newPacketFlag, &mqSentBuff.lock);

		for(i =0;i<mqSentBuff.len;i++)
		{
			pubBuf[i]= mqSentBuff.data[i];
		}

		pthread_mutex_unlock(&mqSentBuff.lock);
		
		if(mqSentBuff.mqttTopicFlag == MQTPA)
		{
			MQTTClient_publish(client, gs_report,mqSentBuff.len, pubBuf,QOS,0, &token);
			rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
			//printf("---rc=%d------token=%d---------------\n",rc,token);
			if(rc!=0)
			{
				printf("----retry---------------------\n");
				pthread_mutex_lock(&comBuff0.lock);
				AP_circleBuff_WritePacket(pubBuf,mqSentBuff.len,DTU2MQTPA);
				pthread_cond_signal(&comBuff0.newPacketFlag);
				pthread_mutex_unlock(&comBuff0.lock);

			}
		}
		else
		{
			MQTTClient_publish(client, g_mqTopicResponse,mqSentBuff.len, pubBuf,QOS,0, &token);
			rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
		}
		//if(rc==0)printf("Message with delivery token %d delivered\n", token);
		//--------------------------------------
		//sleep(2);
		
	}

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    //return rc;
}
