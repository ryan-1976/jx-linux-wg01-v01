#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "MQTTClient.h"
#include "circlebuff.h"
#include <unistd.h>
#include "spiCom.h"
extern RECV_BUFF_T RecvBuff4treat;
typedef struct {
	int     ol;
	int 	oid;
	int 	value;
	int     option;
}TF_MON_OBJ ;

TF_MON_OBJ  monObj_tbl[] = {
    //len       oid    value
	{0x04,    0x0001,  0x00 ,0},
	{0xff,    0xffff,  0xff ,0Xff},
};
void specialTreak(int oid);
int search_table_index(int oid)
{
	int i,index;

   index=0xffff;
   for(i=0;i<1000;i++)
   {
	   if(monObj_tbl[i].oid == 0xffff)
		{
		   break;
		}
		else if(monObj_tbl[i].oid == oid)
		{
			return i;
		}
   }
   return index;
}
void *mqRecPackeThread(void)
{
	int i,j;
	COM_TYPE *pCom;
	int packetRxLen;
	U08 *ptrRx;
	UNION_2_BYTE tmp ;
	U08 tmpLen;
	UNION_4_BYTE ltmp;

	while(1)
	{
		pthread_mutex_lock(&RecvBuff4treat.lock);

		pthread_cond_wait(&RecvBuff4treat.newPacketFlag, &RecvBuff4treat.lock);
		printf("\n");
		printf("---receiv packet :--------------------\n");
		for(i =0;i<RecvBuff4treat.len;i++)
		{
			printf("%x ",RecvBuff4treat.data[i]);
			//pubBuf[i]= RecvBuff4treat.data[i];
		}
		printf("\n");
		pCom=(COM_TYPE*)&RecvBuff4treat.data[0];
		packetRxLen = pCom->head.packetLen;
//		printf("---packetRxLen=%d-------------------\n",packetRxLen);
		ptrRx = &RecvBuff4treat.data[PACKET_HEAD_LEN];
	//	ptrTx = (U08)&RecvBuff4treat[PACKET_HEAD_LEN];
		for (i = 0; i < (packetRxLen+3-PACKET_HEAD_LEN); i += tmpLen)
		{
				tmpLen =*ptrRx++;
			    tmp.byte[0] =*ptrRx++;
				tmp.byte[1] =*ptrRx++;
				j=search_table_index((int)tmp.word);
//				printf("---oid=%d-------------------\n",tmp.word);
//				printf("---j=%d-------------------\n",j);
				if(j !=0xffff)
				{
					if(tmpLen<=7)
					{
						ltmp.lword=0;
						if(tmpLen>=4)ltmp.byte[0]=*ptrRx++;
						if(tmpLen>=5)ltmp.byte[1]=*ptrRx++;
						if(tmpLen>=6)ltmp.byte[2]=*ptrRx++;
						if(tmpLen>=7)ltmp.byte[3]=*ptrRx++;
						monObj_tbl[j].value =ltmp.lword;

					//	printf("---monObj_tbl[j].value=%x-------------------\n",monObj_tbl[j].value);
					}
					specialTreak(j);
				}
		}
		pthread_mutex_unlock(&RecvBuff4treat.lock);
	}

}
void specialTreak(int index)
{
	 struct timeval stime;
	if(monObj_tbl[index].oid==0x0001)
	{
//		gettimeofday(&stime,NULL);
//		printf("before 的时间秒数是：%ld,毫秒数是：%ld\n",stime.tv_sec,stime.tv_usec);
		stime.tv_sec = monObj_tbl[index].value;
		//printf("stime.tv_sec=%x\n",stime.tv_sec);
		//stime.tv_sec = 0x5b582839;
		stime.tv_usec = 0;
		if(settimeofday (&stime, (struct timezone *) 0) < 0)
		{
			printf("\nSet system datatime error!\n");
		}
		else
		{
			printf("\nSet system datatime ok!\n");
		}
//		else
//		{
//			printf("\n");
//			printf("Set system datatime ok!\n");
//			gettimeofday(&stime,NULL);
//		    printf("set after时间秒数是：%ld,毫秒数是：%ld\n",stime.tv_sec,stime.tv_usec);
//		}
	}

}
