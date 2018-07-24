

#ifndef SPICOM_H
#define SPICOM_H

#include <linux/types.h>
typedef  unsigned char  U08;
typedef  unsigned short U16;
typedef  unsigned int   U32;
typedef  signed char    I08;
typedef  signed short   I16;

#define PACKED __attribute__((aligned(1),packed))  // 一字节对齐
//typedef struct{
//	U08        startFlag;
//	U08        packetLen[2];
//	U08        packetNum[2];
//	long       timeS;
//	U16        timeM;
//	U08        siteId[4];
//	U08        devId;
//	U08        packetCmd;
//	U08        packetAnswer;
//} PACKED COM_HEAD_TYPE;
typedef struct{
	U08        startFlag;
	U16        packetLen;
	U16        packetNum;
	long       timeS;
	U16        timeM;
	long       siteId;
	U08        devId;
	U08        packetCmd;
	U08        packetAnswer;
} PACKED COM_HEAD_TYPE;

#define PACKET_HEAD_LEN (sizeof(COM_HEAD_TYPE))
#define PACKET_CONTEXT_LEN 734
#define PACKET_MAX_LEN 1024

typedef union
{
	U08             buffer[PACKET_MAX_LEN];
    COM_HEAD_TYPE   head;
} COM_TYPE;

typedef union
{
	U32   lword;
	U08   byte[4];
} UNION_4_BYTE;
typedef union
{
	U16   word;
	U08   byte[2];
} UNION_2_BYTE;
#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))
#define BUFFER_SIZE 768
#define SPI_DEVICE "/dev/spidev0.0"
#define GPIO_DEVICE "/sys/class/gpio/gpio68/value"

#endif /* SPICOM_H */
