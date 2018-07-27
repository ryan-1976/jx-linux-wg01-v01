#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "circlebuff.h"
#include <stdlib.h>
extern void * mqRecPackeThread(void);
extern void * mqtt_pub_treat(int argc, char* argv[]);
extern void * msgDisPatcherThread(void);
extern void * sampleData_treat(void);
extern void * sqlite_treat(void);
//extern void * bluetooth_treat(void);
extern void G_Buff_init(void);
//extern void * uartTRecThread(void);
int g_siteId;
char rd_siteId[20];
char rd_serverIP[30];
const char *gs_siteId="18072799";
 int main(void)
 {
	pthread_t th_a, th_b,th_c,th_d,th_e,th_f;

	void *retval;
	printf("|-----monitor start run---nmamtf------|\n");
	readConfig();
	if(!strlen(rd_siteId))
	{
		g_siteId=atoi(gs_siteId);
	}
	else
	{
		g_siteId=atoi(rd_siteId);
		printf("read devid=%s\n",rd_siteId);
	}
	G_Buff_init();
	//tabProc();
	pthread_create(&th_d, NULL, (void *(*)(void *))sqlite_treat, 0);
	pthread_create(&th_a, NULL, (void *(*)(void *))msgDisPatcherThread, 0);
	pthread_create(&th_b, NULL, (void *(*)(void *))mqRecPackeThread, 0);
	pthread_create(&th_c, NULL, (void *(*)(void *))mqtt_pub_treat, 0);
	pthread_create(&th_e, NULL, (void *(*)(void *))sampleData_treat, 0);
	//pthread_create(&th_f, NULL, (void *(*)(void *))uartTRecThread, 0);

	pthread_join(th_d, &retval);
	pthread_join(th_a, &retval);
	//pthread_join(th_b, &retval);
	pthread_join(th_c, &retval);
	pthread_join(th_e, &retval);
	//pthread_join(th_f, &retval);
	return 0;
 }

 void  readConfig(void)
 {
	 int rt;
	 FILE *fid;
	 char str[3][20];
	 rd_siteId[0]=0;
	 rd_serverIP[0]=0;

     fid=fopen("/opt/config.txt","r");
     if(fid==NULL)
     {
    	 printf("not config.txt file \n ");
         return ;
     }
	 fscanf(fid,"%s",&str[0][0]);
	 fscanf(fid,"%s",&str[1][0]);

	 strcat(rd_siteId,str[0]);
	 strcat(rd_serverIP,str[1]);
	//printf("%s\n ",str[0]);
	//printf("%s\n ",rd_serverIP);

     return ;
 }
