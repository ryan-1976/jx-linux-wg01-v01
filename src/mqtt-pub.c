//#include "stdio.h"
//#include "stdlib.h"
//#include "string.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "MQTTClient.h"
#include <unistd.h>
#include "circlebuff.h"


extern char rd_siteId[];
extern char rd_serverIP[];

#define ADDRESS     "tcp://47.106.81.63:1883"
//#define ADDRESS     "tcp://120.77.254.235:2183"

char gs_d2s[50];
char gs_s2d[50];

char g_mqClientId[50];
static const char *d2s="/D2S";
static const char *rec="/S2D";
static const char *topicFront="SBM01/";
char g_firstConnetFlag=0;
#define QOS         1
#define TIMEOUT     5000L
int g_preToken=0;

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

//    printf(" new mqtt Message arrived:");
//    printf(" topic: %s", topicName);
//	  printf(" topicLen:%d\n ",message->payloadlen);

    payloadptr = message->payload;

	pthread_mutex_lock(&comBuff0.lock);

	AP_circleBuff_WritePacket(payloadptr++,message->payloadlen,MQTPA2DTU);
	//pthread_cond_signal(&comBuff0.newPacketFlag);
	pthread_mutex_unlock(&comBuff0.lock);

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topicName);
    return 1;
}

void connlost(void *context, char *cause)
{
    printf("\nConnection lost cause: %s\n", cause);
 }

//----------------------------------------------------------------------
void *mqtt_pub_treat(int argc, char* argv[])
{
    MQTTClient client;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    MQTTClient_deliveryToken token;
    int rc, i;


    gs_d2s[0]=0;
   	strcat(gs_d2s,topicFront);
   	strcat(gs_d2s,rd_siteId);
	strcat(gs_d2s,d2s);

	gs_s2d[0]=0;
   	strcat(gs_s2d,topicFront);
   	strcat(gs_s2d,rd_siteId);
   	strcat(gs_s2d,rec);

   	g_mqClientId[0]=0;
 	strcat(g_mqClientId,topicFront);
   	strcat(g_mqClientId,rd_siteId);

	conn_opts.username="jxkj007";
	conn_opts.password="001";

	if(strlen(rd_serverIP))
	{
		MQTTClient_create(&client, rd_serverIP, g_mqClientId, MQTTCLIENT_PERSISTENCE_NONE, NULL);
		printf("read_serverIP=%s\n",rd_serverIP);
	}
	else
	{
		MQTTClient_create(&client, ADDRESS, g_mqClientId, MQTTCLIENT_PERSISTENCE_NONE, NULL);

		printf("ADDRESS server=%s\n",ADDRESS);
	}

    conn_opts.keepAliveInterval = 60;
    conn_opts.cleansession = 1;
    MQTTClient_setCallbacks(client, NULL, connlost, msgarrvd, delivered);
	if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		printf("start up Failed to connect, return code %d\n", rc);
	}

	MQTTClient_subscribe(client, gs_s2d, QOS);

	while(1)
	{
		while(!MQTTClient_isConnected(client)){
			if ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
				{
					printf(" pub retry to connect failure, return code %d\n", rc);
				}
			else
			{
				MQTTClient_subscribe(client, gs_s2d, QOS);
			}
			g_connectErrCount++;
			if(g_connectErrCount>200){
				printf(" -----system reboot--------------\n");
				system("reboot");
			}
			sleep(3);
		};
		g_firstConnetFlag=1;
		g_connectErrCount=0;
//		pthread_mutex_lock(&mqSentBuff.lock);
//		pthread_cond_wait(&mqSentBuff.newPacketFlag, &mqSentBuff.lock);
		while(mqSentBuff.packetSum ==0)
		{
			usleep(50000);
		}
		for(i =0;i<mqSentBuff.len;i++)
		{
			pubBuf[i]= mqSentBuff.data[i];
		}
//		pthread_mutex_unlock(&mqSentBuff.lock);
		if(mqSentBuff.packetSum>0)mqSentBuff.packetSum--;
		if(mqSentBuff.mqttTopicFlag == MQTPA)
		{
			MQTTClient_publish(client, gs_d2s,mqSentBuff.len, pubBuf,QOS,0, &token);
			//printf("gs_d2s=%s--\n",gs_d2s);
			rc = MQTTClient_waitForCompletion(client, token, TIMEOUT);
			if(rc!=0 && (token!=(g_preToken+1)))
			{
				printf("retry-rc=%d--token=%d--packetSum=%d--\n",rc,token,comBuff0.packetSum);
				pthread_mutex_lock(&comBuff0.lock);
				AP_circleBuff_WritePacket(pubBuf,mqSentBuff.len,DTU2MQTPA);
				//pthread_cond_signal(&comBuff0.newPacketFlag);
				pthread_mutex_unlock(&comBuff0.lock);
			}
			else
			{
				g_preToken =token;
				//printf("token=%d\n",token);
			}
		}
		//--------------------------------------
		//sleep(1);
	}

    MQTTClient_disconnect(client, 10000);
    MQTTClient_destroy(&client);
    //return rc;
}
