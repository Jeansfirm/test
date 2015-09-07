/***************************************************************************
  Copyright (C), 2009-2014 GuangdongGuanglian Electronic Technology Co.,Ltd.
  FileName:      online_detect.c
  Author:        jiang
  Version :      1.0    
  Date:          2014-06-16
  Description:   实现IPC在线检测      
  History:         
      <author>  <time>   <version >   <desc> 
***************************************************************************/
#include <stdio.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
#include <netdb.h>
#include <setjmp.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#include "sqlite_access.h"

#define PACKET_SIZE     4096
#define MAX_WAIT_TIME   1
#define MAX_NO_PACKETS  3


char sendpacket[PACKET_SIZE];
char recvpacket[PACKET_SIZE];
int sockfd,datalen=56;
int nsend=0,nreceived=0,unreceived=0;
struct sockaddr_in dest_addr;
pid_t pid;
struct sockaddr_in from;
struct timeval tvrecv;
//void statistics(int signo);
unsigned short cal_chksum(unsigned short *addr,int len);
int pack(int pack_no);
void send_packet(void);
void recv_packet(void);
int unpack(char *buf,int len);
void tv_sub(struct timeval *out,struct timeval *in);



/*校验和算法*/
unsigned short cal_chksum(unsigned short *addr,int len)
{      
	int nleft=len;    
	int sum=0;      
	unsigned short *w=addr;  
	unsigned short answer=0;     
	/*把ICMP报头二进制数据以2字节为单位累加起来*/   
	while(nleft>1)      
	{     
		sum+=*w++;       
		nleft-=2;       
	}       
	/*若ICMP报头为奇数个字节，会剩下最后一字节。
	把最后一个字节视为一个2字节数据的高字节，这个2字节数据的低字节为0，继续累加*/    
	if( nleft==1)       
	{      
		*(unsigned char *)(&answer)=*(unsigned char *)w;      
		sum+=answer;        
	}       
	sum=(sum>>16)+(sum&0xffff);    
	sum+=(sum>>16);      
	answer=~sum;       
	return answer;
}
/*设置ICMP报头*/
int pack(int pack_no)
{      
	int i,packsize;     
	struct icmp *icmp;      
	struct timeval *tval;     
	icmp=(struct icmp*)sendpacket;   
	icmp->icmp_type=ICMP_ECHO;      
	icmp->icmp_code=0;     
	icmp->icmp_cksum=0;       
	icmp->icmp_seq=pack_no;     
	icmp->icmp_id=pid;       
	packsize=8+datalen;       
	tval= (struct timeval *)icmp->icmp_data;   

	gettimeofday(tval,NULL);  /*记录发送时间*/   

	icmp->icmp_cksum=cal_chksum( (unsigned short *)icmp,packsize);/*校验算法*/     
	
    return packsize;
}
/*发送三个ICMP报文*/
void send_packet()
{    
	nsend=0;
    //printf("****send packet 0000****\r\n"); 
	int packetsize;     
	while( nsend<MAX_NO_PACKETS)     
	{      
		nsend++;       
		packetsize=pack(nsend);
		/*设置ICMP报头*/           
		if(sendto(sockfd,sendpacket,packetsize,0, (struct sockaddr *)&dest_addr,sizeof(dest_addr))<0)   
		{    
			printf("<p>sendto error</p>");
			//perror("sendto error");          
			//continue;            
		}               
		usleep(700); 
		/*每隔一秒发送一个ICMP报文*/      
	}
    //printf("****send packet 1111****\r\n");
}

/*两个timeval结构相减*/
void tv_sub(struct timeval *out,struct timeval *in)
{      
	if( (out->tv_usec-=in->tv_usec)<0)      
	{     
		--out->tv_sec;      
		out->tv_usec+=1000000;   
	}       
	out->tv_sec-=in->tv_sec;
}

/*剥去ICMP报头*/
int unpack(char *buf,int len)
{    
	int i,iphdrlen;  
	struct ip *ip;   
	struct icmp *icmp;  
	struct timeval *tvsend;  
	double rtt;  
    
	ip=(struct ip *)buf; 
    
	iphdrlen=ip->ip_hl<<2;  /*求ip报头长度,即ip报头的长度标志乘4*/ 
	  
	icmp=(struct icmp *)(buf+iphdrlen); /*越过ip报头,指向ICMP报头*/  
  
	len-=iphdrlen; /*ICMP报头及ICMP数据报的总长度*/  

	if(len<8)     /*小于ICMP报头长度则不合理*/     
	{     
		printf("ICMP packets\'s length is less than 8\n");  
		return -1;   
	}      
    /*确保所接收的是我所发的的ICMP的回应*/     
	if((icmp->icmp_type==ICMP_ECHOREPLY) && (icmp->icmp_id==pid))   
	{      
		//tvsend=(struct timeval *)icmp->icmp_data;   
		//tv_sub(&tvrecv,tvsend);                     /*接收和发送的时间差*/           
		//rtt=tvrecv.tv_sec*1000+tvrecv.tv_usec/1000; /*以毫秒为单位计算rtt*/             
		/*显示相关信息*/             
		//printf("%d byte from %s: icmp_seq=%u ttl=%d rtt=%.3f ms\n",len,inet_ntoa(from.sin_addr),icmp->icmp_seq,ip->ip_ttl,rtt); 
        return 1;   
	}       
	else   
		return -1;
}


/*接收所有ICMP报文*/
void recv_packet()
{      
	int n,fromlen;  
    int i;    
	extern int errno;   
	int recv_ret;
	//signal(SIGALRM,statistics);
	fromlen=sizeof(from);  
	//while( nreceived<nsend)    
	for(recv_ret=0;recv_ret<nsend;recv_ret++)
	{      
		//alarm(MAX_WAIT_TIME);  
        //for(i=0;i<5;i++)
        //{
        //    usleep(10000);//延时以便收到报文，因为使用类非阻塞socket 
        //}
                 
		if((n=recvfrom(sockfd,recvpacket,sizeof(recvpacket),0,(struct sockaddr *)&from,&fromlen)) <0) 
        //if(n = recv(sockfd,recvpacket,sizeof(recvpacket), 0)<=0)//阻塞读     
		{   
			if((errno==EINTR)||(errno == EAGAIN))
				continue;       
			perror("recvfrom error");  
			continue;          
		}              
		gettimeofday(&tvrecv,NULL); 
		/*记录接收时间*/           
		if(unpack(recvpacket,n)==-1)
		{
			unreceived++;
			continue; 
		}          
		nreceived++;    
		usleep(700);
	}
    //printf("****recv_packet 1111****\r\n");
}

int detect_ipc_online(char *ipaddr)
{     
    nsend=0;
    nreceived=0; 
    unreceived=0;
    dest_addr.sin_addr.s_addr = inet_addr(ipaddr);
    
	pid=getpid();        /*获取进程id,用于设置ICMP的标志符*/  
         
	send_packet();       /*发送所有ICMP报文*/   

	recv_packet();       /*接收所有ICMP报文*/  
	
    return nreceived;  //nreceived=3 for ping success

}

/*
void init_data_online_detect()
{
    int ipc_num;
    for(ipc_num=0;ipc_num<MAX_IPC_NUM;ipc_num++)
    {
        //ipc_etc_msg.ipc_list[ipc_num].ip_addr = NULL;
        memset(ipc_etc_msg.ipc_list[ipc_num].ip_addr,0,sizeof(ipc_etc_msg.ipc_list[ipc_num].ip_addr));
    }
}
*/

int init_online_detect()
{
    struct protoent *protocol;   
    
    int size=3*1024; 

    nsend=0;
    nreceived=0; 

	if((protocol=getprotobyname("icmp") )==NULL)     
	{    
		perror("getprotobyname");   
		//exit(1); 
        return -1;    
	}   
	/*生成使用ICMP的原始套接字,这种套接字只有root才能生成*/    
	if((sockfd=socket(AF_INET,SOCK_RAW,protocol->p_proto))<0)       
	{      
		perror("socket error"); 
        close(sockfd);        
		//exit(1); 
        return -1;     
	}    

    //设置阻塞接收超时
    struct timeval tv_out;
    tv_out.tv_sec = 2; //超时2秒没有收到ICMP报文就返回
    tv_out.tv_usec = 0;
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv_out, sizeof(tv_out));
  
	/* 回收root权限,设置当前用户权限*/     
	//setuid(getuid());    
	/*扩大套接字接收缓冲区到50K这样做主要为了减小接收缓冲区溢出的     
	的可能性,若无意中ping一个广播地址或多播地址,将会引来大量应答*/     
	setsockopt(sockfd,SOL_SOCKET,SO_RCVBUF,&size,sizeof(size));    
	bzero(&dest_addr,sizeof(dest_addr));  
	dest_addr.sin_family=AF_INET; 
    //dest_addr.sin_addr.s_addr = inet_addr("192.168.1.184");
    return 0;
}

/***************************************************************************
  Function:       get_ipc_status(char*ipc_status))
  Description:    从数据库中取得IPC IP地址，并查询获PC状态，查询结构保存在ipc_status
  Input:                            
  Output:         
  Return:        0：成功，-1：操作数据库失败 
  Others:         
***************************************************************************/
char get_ipc_status(char *ipaddr)
{
	char status;
	
    if(detect_ipc_online(ipaddr)>=2)
    {
      status= 'a'; // IPC online
    }
    else
    {
      status = 'b';// IPC not online
    }
    return status;
}


