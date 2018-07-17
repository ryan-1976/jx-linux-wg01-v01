//串口相关的头文件  
#include<stdio.h>      /*标准输入输出定义*/  
#include<stdlib.h>     /*标准函数库定义*/  
#include<unistd.h>     /*Unix 标准函数定义*/  
#include<sys/types.h>   
#include<sys/stat.h>     
#include<fcntl.h>      /*文件控制定义*/  
#include<termios.h>    /*PPSIX 终端控制定义*/  
#include<errno.h>      /*错误号定义*/  
#include<string.h>  
#define UART_MAX_BUFF_LEN 1024
typedef struct
{
	volatile unsigned long int    readPos;
	volatile unsigned long int    writePos;
    unsigned char   data[UART_MAX_BUFF_LEN];
}UART_CIRCLE_BUFF;
UART_CIRCLE_BUFF g_uartCircleBuff;
 static const char *serialPath[]={"/dev/ttymxc7","/dev/ttymxc2","/dev/ttymxc3","/dev/ttymxc5","/dev/ttymxc4","/dev/ttymxc6"};
 static const char *rs485[]={"RS485-0","RS485-1","RS485-2","RS485-3","RS485-4","RS485-5"};
//宏定义  

#define FALSE  -1  
#define TRUE   0  
//char rcv_buf[6][256];
/******************************************************************* 
* 名称：                  UART0_Open 
* 功能：                打开串口并返回串口设备文件描述 
* 入口参数：        fd    :文件描述符     port :串口号(ttyS0,ttyS1,ttyS2) 
* 出口参数：        正确返回为1，错误返回为0 
*******************************************************************/  
int UART_Open(int fd,char* port)
{  
     
	fd = open( port, O_RDWR|O_NOCTTY|O_NDELAY);  
	if (FALSE == fd)  
	{  
		perror("Can't Open Serial Port");  
		return(FALSE);  
	}  
	//恢复串口为阻塞状态                                 
	if(fcntl(fd, F_SETFL, 0) < 0)  
	{  
		//printf("fcntl failed!\n");
		return(FALSE);  
	}       
	else  
	{  
		//printf("fcntl=%d\n",fcntl(fd, F_SETFL,0));
	}  
	//测试是否为终端设备      
	if(0 == isatty(STDIN_FILENO))  
	{  
		printf("standard input is not a terminal device\n");  
		return(FALSE);  
	}  
	else  
	{  
		//printf("is a tty success!\n");
	}                
	printf("fd->open=%d ok\n",fd);
	return fd;  
}  
/******************************************************************* 
* 名称：                UART0_Close 
* 功能：                关闭串口并返回串口设备文件描述 
* 入口参数：        fd    :文件描述符     port :串口号(ttyS0,ttyS1,ttyS2) 
* 出口参数：        void 
*******************************************************************/  
   
void UART_Close(int fd)
{  
	close(fd);  
}  
   
/******************************************************************* 
* 名称：                UART0_Set 
* 功能：                设置串口数据位，停止位和效验位 
* 入口参数：        fd        串口文件描述符 
*                              speed     串口速度 
*                              flow_ctrl   数据流控制 
*                           databits   数据位   取值为 7 或者8 
*                           stopbits   停止位   取值为 1 或者2 
*                           parity     效验类型 取值为N,E,O,,S 
*出口参数：          正确返回为1，错误返回为0 
*******************************************************************/  
int UART_Set(int fd,int speed,int flow_ctrl,int databits,int stopbits,int parity)
{  
     
	int   i;  
	int   status;  
	int   speed_arr[] = { B115200, B57600, B38400, B19200, B9600, B4800, B2400, B1200, B300};
	int   name_arr[] = {115200, 57600, 38400, 19200,  9600,  4800,  2400,  1200,  300};
           
	struct termios options;  
     
	/*tcgetattr(fd,&options)得到与fd指向对象的相关参数，并将它们保存于options,该函数还可以测试配置是否正确，该串口是否可用等。若调用成功，函数返回值为0，若调用失败，函数返回值为1. 
    */  
	if( tcgetattr( fd,&options)  !=  0)  
	{  
		perror("SetupSerial 1");      
		return(FALSE);   
	}  
    
    //设置串口输入波特率和输出波特率  
	for ( i= 0;  i < sizeof(speed_arr) / sizeof(int);  i++)  
	{  
		if  (speed == name_arr[i])  
		{               
			cfsetispeed(&options, speed_arr[i]);   
			cfsetospeed(&options, speed_arr[i]);    
		}  
	}       
     
    //修改控制模式，保证程序不会占用串口  
    options.c_cflag |= CLOCAL;  
    //修改控制模式，使得能够从串口中读取输入数据  
    options.c_cflag |= CREAD;  
    
    //设置数据流控制  
    switch(flow_ctrl)  
    {  
        
		case 0 ://不使用流控制  
              options.c_cflag &= ~CRTSCTS;  
              break;     
        
		case 1 ://使用硬件流控制  
              options.c_cflag |= CRTSCTS;  
              break;  
		case 2 ://使用软件流控制  
              options.c_cflag |= IXON | IXOFF | IXANY;  
              break;  
    }  
    //设置数据位  
    //屏蔽其他标志位  
    options.c_cflag &= ~CSIZE;  
    switch (databits)  
    {    
		case 5    :  
                     options.c_cflag |= CS5;  
                     break;  
		case 6    :  
                     options.c_cflag |= CS6;  
                     break;  
		case 7    :      
                 options.c_cflag |= CS7;  
                 break;  
		case 8:      
                 options.c_cflag |= CS8;  
                 break;    
		default:     
			      options.c_cflag |= CS8;
			     break;
    }  
    //设置校验位  
    switch (parity)  
    {    
		case 'n':  
		case 'N': //无奇偶校验位。  
                 options.c_cflag &= ~PARENB;   
                 options.c_iflag &= ~INPCK;      
                 break;   
		case 'o':    
		case 'O'://设置为奇校验      
                 options.c_cflag |= (PARODD | PARENB);   
                 options.c_iflag |= INPCK;               
                 break;   
		case 'e':   
		case 'E'://设置为偶校验    
                 options.c_cflag |= PARENB;         
                 options.c_cflag &= ~PARODD;         
                 options.c_iflag |= INPCK;        
                 break;  
		case 's':  
		case 'S': //设置为空格   
                 options.c_cflag &= ~PARENB;  
                 options.c_cflag &= ~CSTOPB;  
                 break;   
        default:    //无奇偶校验位。
//                 fprintf(stderr,"Unsupported parity\n");
//                 return (FALSE);
				options.c_cflag &= ~PARENB;
				options.c_iflag &= ~INPCK;
				break;
    }   
    // 设置停止位   
    switch (stopbits)  
    {    
		case 1:     
                 options.c_cflag &= ~CSTOPB; break;   
		case 2:     
                 options.c_cflag |= CSTOPB; break;  
		default:     
			     options.c_cflag &= ~CSTOPB; break;
    }  
     
	//修改输出模式，原始数据输出  
	options.c_oflag &= ~OPOST;  
    
	options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);  
	//options.c_lflag &= ~(ISIG | ICANON);  
     
    //设置等待时间和最小接收字符  
    options.c_cc[VTIME] = 1; /* 读取一个字符等待1*(1/10)s */    
    options.c_cc[VMIN] = 1; /* 读取字符的最少个数为1 */  
     
    //如果发生数据溢出，接收数据，但是不再读取 刷新收到的数据但是不读  
    tcflush(fd,TCIFLUSH);  
     
    //激活配置 (将修改后的termios数据设置到串口中）  
    if (tcsetattr(fd,TCSANOW,&options) != 0)    
	{  
		perror("com set error!\n");    
		return (FALSE);   
	}  
    return (TRUE);   
}  
/******************************************************************* 
* 名称：                UART0_Init() 
* 功能：                串口初始化 
* 入口参数：        fd       :  文件描述符    
*               speed  :  串口速度 
*                              flow_ctrl  数据流控制 
*               databits   数据位   取值为 7 或者8 
*                           stopbits   停止位   取值为 1 或者2 
*                           parity     效验类型 取值为N,E,O,,S 
*                       
* 出口参数：        正确返回为1，错误返回为0 
*******************************************************************/  
int UART_Init(int fd, int speed,int flow_ctrl,int databits,int stopbits,int parity)
{  
    int err;  
    //设置串口数据帧格式  
    if (UART_Set(fd,speed,flow_ctrl,databits,stopbits,parity) == FALSE)
	{                                                           
		return FALSE;  
	}  
    else  
	{  
		return  TRUE;  
	}  
}  
   
/******************************************************************* 
* 名称：                  UART0_Recv 
* 功能：                接收串口数据 
* 入口参数：        fd                  :文件描述符     
*                              rcv_buf     :接收串口中数据存入rcv_buf缓冲区中 
*                              data_len    :一帧数据的长度 
* 出口参数：        正确返回为1，错误返回为0 
*******************************************************************/  
int UART_Recv(int fd, char *rcv_buf,int data_len)
{  
	int len,fs_sel;  
    fd_set fs_read;  
     
    struct timeval time;  
     
    FD_ZERO(&fs_read);  
    FD_SET(fd,&fs_read);  
     
    time.tv_sec = 0;
    time.tv_usec = 30000;
     
    //使用select实现串口的多路通信  
    fs_sel = select(fd+1,&fs_read,NULL,NULL,&time);  
   // printf("fs_sel = %d\n",fs_sel);
    if(fs_sel)  
	{  
		len = read(fd,rcv_buf,data_len);  
		return len;  
	}  
    else  
	{  
		return FALSE;  
	}       
}  
/******************************************************************** 
* 名称：                  UART0_Send 
* 功能：                发送数据 
* 入口参数：        fd                  :文件描述符     
*                              send_buf    :存放串口发送数据 
*                              data_len    :一帧数据的个数 
* 出口参数：        正确返回为1，错误返回为0 
*******************************************************************/  
int UART_Send(int fd, char *send_buf,int data_len)
{  
    int len = 0;  
     
    len = write(fd,send_buf,data_len);  
    if (len == data_len )  
	{  
		return len;  
	}       
    else     
	{  
		tcflush(fd,TCOFLUSH);  
		return FALSE;  
	}  
     
}  
static unsigned char  AP_circleBuff_HaveData_Buff(void)
{
    if (g_uartCircleBuff.readPos != g_uartCircleBuff.writePos)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

static unsigned char  AP_circleBuff_WriteData(unsigned char data)
{
    unsigned long int  nextPos;
	//printf("-----comBuff0.writePos= %d\n",comBuff0.writePos);
    nextPos = (g_uartCircleBuff.writePos + 1) % UART_MAX_BUFF_LEN;
    if (nextPos != g_uartCircleBuff.readPos)
    {
    	g_uartCircleBuff.data[g_uartCircleBuff.writePos] = data;
    	g_uartCircleBuff.writePos = nextPos;
		 return 1;

    }
    else
    {
        return 0;
    }
}

static unsigned char AP_circleBuff_ReadData(void)
{
    unsigned char dataTemp =0;

    if (AP_circleBuff_HaveData_Buff() == 1)
    {
        dataTemp = g_uartCircleBuff.data[g_uartCircleBuff.readPos];
        g_uartCircleBuff.readPos = (g_uartCircleBuff.readPos + 1) % UART_MAX_BUFF_LEN;
    }
    return  dataTemp;
}
void *uartTRecThread(void)
{  
    int fd;                            //文件描述符  
    int err;                           //返回调用函数的状态  
    int len;                          
    int i;  
    char rcv_buf[255];
    char send_buf[20]="tiger john";

    fd = UART_Open(fd,"/dev/ttymxc6"); //打开串口，返回文件描述符
    do
	{  
		err = UART_Init(fd,38400,0,8,1,'N');
		printf("Set Port Exactly!\n");  
	}while(FALSE == err || FALSE == fd);  
     
		while (1) //循环读取数据  
		{    
			len = UART_Recv(fd, rcv_buf,44);
  			if(len > 0)  
			{  
  				//tcflush(fd,TCIFLUSH);
				printf("len = %d\n",len);  
				for(i=0;i<len;i++){
					printf(" %d",rcv_buf[i]);
					AP_circleBuff_WriteData(rcv_buf[i]);
				}
				printf("\n");
			}  
			else  
			{  
				usleep(10000);
			}  
		}              
		UART_Close(fd);
}
