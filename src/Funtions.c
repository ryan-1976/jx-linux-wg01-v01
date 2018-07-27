#include <stdio.h>  
#include <stdlib.h>  
#include <unistd.h>  
#include "circlebuff.h"
#include "spiCom.h"

//U08 cdtuBuf[PACKET_LEN];
INT8U cdtuBuf[1024];
extern U08 spiRxBuff[];
U16 g_comPacketIdx=0;
//static U32 g_siteId=0x18071301;
extern U32 g_siteId;
extern DATAS_BUFF_T   comBuff0;
int spi2MqtttPacket(void)
{
	COM_TYPE *pCom;
	U16 i;
	U16 crc=0;
	int packetLen;
	struct timeval tv;
    if(spiRxBuff[1]==0)
    {
    	return 0;
    }
	pCom =(COM_TYPE *)&cdtuBuf[0];
	pCom->head.startFlag = 0x7e;
	pCom->head.packetNum=g_comPacketIdx;
	gettimeofday(&tv,NULL);
	pCom->head.timeS = tv.tv_sec;
	pCom->head.timeM = (U16)(tv.tv_usec/1000);
	g_comPacketIdx++;
	pCom->head.siteId=g_siteId;
	pCom->head.devId = spiRxBuff[1];
	pCom->head.packetCmd = 0x82;
	pCom->head.packetAnswer = 0xff;
	packetLen =PACKET_HEAD_LEN;
	pCom->buffer[packetLen++]=0xff;
	pCom->buffer[packetLen++]=0x01;
	pCom->buffer[packetLen++]=0x0f;

    for(i=0;i<PACKET_CONTEXT_LEN-3;i++){
    	pCom->buffer[packetLen++]=spiRxBuff[i+1];
    }
    pCom->head.packetLen =packetLen-3;
	pCom->buffer[packetLen++]=(U08)(crc>>8);
	pCom->buffer[packetLen++]=(U08)(crc);
	pCom->buffer[packetLen++]=0x7e;
//	 printf("\n ");
//    for(i=0;i<packetLen;i++)
//    {
//    	printf("%x ",cdtuBuf[i]);
//    }
//    printf("\n ");

    pthread_mutex_lock(&comBuff0.lock);
	AP_circleBuff_WritePacket(cdtuBuf,packetLen,DTU2MQTPA);
	//pthread_cond_signal(&comBuff0.newPacketFlag);
	pthread_mutex_unlock(&comBuff0.lock);
	return 1;
}
void getTimePacket(void)
{
	COM_TYPE *pCom;
	U16 i;
	struct timeval tv;
	U16 crc=0;
	int packetLen;
	pCom =(COM_TYPE *)&cdtuBuf[0];
	pCom->head.startFlag = 0x7e;
	pCom->head.packetNum=g_comPacketIdx;
	g_comPacketIdx++;
	gettimeofday(&tv,NULL);
	pCom->head.timeS = tv.tv_sec;
	pCom->head.timeM = (U16)(tv.tv_usec/1000);

	pCom->head.siteId=g_siteId;
	pCom->head.devId =  0;
	pCom->head.packetCmd = 0x01;
	pCom->head.packetAnswer = 0xff;
	packetLen =PACKET_HEAD_LEN;
	pCom->buffer[packetLen++]=0x07;
	pCom->buffer[packetLen++]=0x01;
	pCom->buffer[packetLen++]=0x00;

    for(i=0;i<4;i++){
    	pCom->buffer[packetLen++]=0;
    }
	pCom->head.packetLen =packetLen-3;
	pCom->buffer[packetLen++]=(U08)(crc>>8);
	pCom->buffer[packetLen++]=(U08)(crc);
    pCom->buffer[packetLen++]=0x7e;
//    printf("send packet ---\n ");
//    for(i=0;i<pCom->head.packetLen+6;i++)
//    {
//    	printf("%x ",cdtuBuf[i]);
//    }
//    printf("\n ");
//    printf("packetLen=%d ",packetLen);
//    printf("\n ");
    pthread_mutex_lock(&comBuff0.lock);
	AP_circleBuff_WritePacket(cdtuBuf,packetLen,DTU2MQTPA);
	//pthread_cond_signal(&comBuff0.newPacketFlag);
	pthread_mutex_unlock(&comBuff0.lock);
}

int testPackt(void)
{
	int i;
	static int j;

    pthread_mutex_lock(&comBuff0.lock);
    for(i=0;i<996;i++)cdtuBuf[i]=j;
    j++;
	AP_circleBuff_WritePacket(cdtuBuf,996,DTU2MQTPA);
	//printf("input data into circleBuffer packet index=%d\n ",j);
	//pthread_cond_signal(&comBuff0.newPacketFlag);
	pthread_mutex_unlock(&comBuff0.lock);
	return 1;
}
