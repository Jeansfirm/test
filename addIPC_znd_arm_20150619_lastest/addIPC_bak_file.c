/***************************************************************************
  Copyright (C), 2009-2014 GuangdongGuanglian Electronic Technology Co.,Ltd.
  FileName:      addIPC.c
  Author:        jiang
  Version :      1.0    
  Date:          2014-04-14
  Description:   实现添加IPC      
  History:         
      <author>  <time>   <version >   <desc> 
***************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/file.h>

#include"cJSON.h"

char *ipc_config[10];
char *ipc0_config = "/gl/etc/video/ipc0.config";
char *ipc1_config = "/gl/etc/video/ipc1.config";
char *ipc2_config = "/gl/etc/video/ipc2.config";
char *ipc3_config = "/gl/etc/video/ipc3.config";
char *ipc4_config = "/gl/etc/video/ipc4.config";
char *ipc5_config = "/gl/etc/video/ipc5.config";
char *ipc6_config = "/gl/etc/video/ipc6.config";
char *ipc7_config = "/gl/etc/video/ipc7.config";
char *ipc8_config = "/gl/etc/video/ipc8.config";
char *ipc9_config = "/gl/etc/video/ipc9.config";
#define MAX_IPC_NUM        10
char file_num = 0;  //IPC ID

/***************************************************************************
  Function:       check_ip(const char *str)
  Description:    检查ip地址是否合法
  Input:          char* inputstr,char *outputstr,char* value
                  
  Output:         
  Return:          
  Others:         
***************************************************************************/
int check_ip(const char *str)
{
	struct in_addr addr;
    int ret;
    volatile int local_errno;

    ret = inet_pton(AF_INET, str, &addr);
    local_errno = errno;
    if (ret > 0)
    {
        ret = 1;
    }
    else 
    {
        ret = -1;
    }
    return ret;
}

/***************************************************************************
  Function:       check_string_len(const char *str,int t_len)
  Description:    检查输入字符串长度是否在t_len范围内
  Input:          
                  
  Output:         
  Return:          
  Others:         
***************************************************************************/
int check_string_len(const char *str,int t_len)
{
    int ret = -1;
	int len = -1;
	len = strlen(str);
    if((len >= 0) && (len <= t_len))
    {
		ret = 1;
    }
    return ret;
}

/***************************************************************************
  Function:       check_port(const char *str)
  Description:    检查rtsp端口是否合法 0-65535
  Input:          char* inputstr,char *outputstr,char* value
                  
  Output:         
  Return:          
  Others:         
***************************************************************************/
int check_port(const char *str)
{
    int ret = -1;
	int temp = -1;
	temp = atoi(str);
    if((temp >= 0) && (temp < 65536))
    {
		ret = 1;
    }
    return ret;
}

/***************************************************************************
  Function:       check_listid(const int *str)
  Description:    检查rtsp端口是否合法 0-65535
  Input:          char* inputstr,char *outputstr,char* value
                  
  Output:         
  Return:          
  Others:         
***************************************************************************/


/***************************************************************************
  Function:       parse_data(char* inputstr,char *outputstr,char* value)
  Description:    在字符串inputstr中查找value，输出查找到的字符串outputstr
  Input:          char* inputstr,char *outputstr,char* value
                  
  Output:         
  Return:          
  Others:         
***************************************************************************/
void parse_data(char* inputstr,char *outputstr,char* value)
{
	int i,j;
	char *tmpstr;

	tmpstr=strstr(inputstr,value);
	j = 0;

	for(i = strlen(value); i < (int)strlen(tmpstr); i++)
	{
		if(tmpstr[i] == '&')
		{
			break;
		}                  
		outputstr[j++]=tmpstr[i];
	}
	outputstr[j] = '\0';
}

/***************************************************************************
  Function:       char* getcgidata(FILE* fp, char* requestmethod)
  Description:    获取web服务器的输入
  Input:          
                  
  Output:         返回web服务器输入的字符串
  Return:         
  Others:         
***************************************************************************/
char* getcgidata(FILE* fp, char* requestmethod)
{
    char* input;
    int len;
    int size = 1024;
    int i = 0;
       
    if (!strcmp(requestmethod, "GET"))
    { 
		//GET在cgi中传递的Username="admin"&Password="aaaaa"被放置在环境变量QUERY_STRING中
		input = getenv("QUERY_STRING");
		return input;
    }
    else if (!strcmp(requestmethod, "POST"))
    {
		len = atoi(getenv("CONTENT_LENGTH"));
		input = (char*)malloc(sizeof(char)*(size + 1));
		          
		if (len == 0)
		{
			input[0] = '\0';
			return input;
		}
		          
		while(1)
		{   //POST在cgi中传递的Username="admin"&Password="aaaaa"被写入stdin标准输入流中
	 	    input[i] = (char)fgetc(fp);
		        if (i == size)
		        {
		             input[i+1] = '\0';
		             return input;
		        }
		                 
		        --len;
		        if (feof(fp) || (!(len)))
		        {
		              i++;
		              input[i] = '\0';
		              return input;
		        }
		        i++;
		                 
		}
    }
       return NULL;
}

/***************************************************************************
  Function:       ipaddr_isuse(char *target_ipaddr) 
  Description:    检查IP地址是否已经使用
  Input:          
                  

  Output:         
  Return:         使用返回-1，没有使用返回0
  Others:         
****************************************************************************/
int ipaddr_isuse(const char *target_ipaddr)
{
    char i;
    int ret = 0;
    char buf[512];
    char ipaddr[20] = {0};
    FILE *fp; 
    int fd; 
	//查找文件
    for(i=0;i<MAX_IPC_NUM;i++)
    {
		if(access(ipc_config[i], 0) == 0)//文件存在
        {
   	        fd=open(ipc_config[i],O_WRONLY|O_APPEND);
            if(flock(fd,LOCK_EX|LOCK_NB)==0)//加锁
            {
			    fp = fopen(ipc_config[i],"r");//只读打开文件
	 		    if(fp != NULL)
			    {          
				    //读
		 		    fgets(buf, sizeof(buf), fp);

		            parse_data(buf,ipaddr,"ipaddr="); 

                    if(strlen(ipaddr) != 0)//以防出错
				    {
				        if(strstr(ipaddr,target_ipaddr)!=NULL)//找到已经使用
					    {
                            //printf("<p>tttstrstr(ipaddr,target_ipaddr)!=NULL</p>");
                            ret = -1;
							break;
					    }
                        else
                        {
                            //printf("<p>tttstrstr(ipaddr,target_ipaddr)==NULL</p>");
                        }
					}
                     memset(ipaddr,0,20);//使用完清零
                }//if(fp != NULL)

                fclose(fp);
                flock(fd,LOCK_UN);//解锁

            }//if(flock(fd,LOCK_EX|LOCK_NB)==0)//加锁
        }//if(access(ipc_config[i], 0) == 0)//文件存在
    }//for(i=0;i<MAX_IPC_NUM;i++)
    //printf("<p>ret = %d</p>",ret);
    return ret;//没有发现IP已经使用则返回0
}


/***************************************************************************
  Function:       package_json 
  Description:    封装JSON
  Input:          消息类型
                  
  Output:         
  Return:         封装json后转化为char类型的数据
  Others:         
****************************************************************************/
char *package_json(char type,char *msg)
{
    cJSON *root,*fmt;
    char *out;	
    //Our "Video" datatype: 
    root=cJSON_CreateObject();	
    cJSON_AddStringToObject(root, "action", "addipc");
    cJSON_AddItemToObject(root, "response_params", fmt=cJSON_CreateObject());
    cJSON_AddNumberToObject(fmt,"status",     type);
    cJSON_AddStringToObject(fmt,"status_msg", msg);
    if(type == 0)
    {
        cJSON_AddNumberToObject(fmt,"ipc_id",file_num);
    }

    //out=cJSON_Print(root);
    out=cJSON_PrintUnformatted(root);
    

    cJSON_Delete(root);
    //cJSON_Delete(fmt);
    //printf("%s\n",out);	
    //free(out);
    // Print to text, Delete the cJSON, print it, release the string. 

    return out; 
}

/***************************************************************************
  Function:       main()
  Description:    主函数
  Input:          
                  
  Output:         
  Return:         
  Others:         
***************************************************************************/
int main()
{ 
    //printf("Content-type:text/html\n\n");                        //cgi 输出html标准格式
    //printf("Content-type:text/html;charset=gb2312\n\n");       //cgi 输出html标准格式 ,输出中文显示
    printf("Content-type:text/html;charset=utf-8\n\n");       //cgi 输出html标准格式 ,输出中文显示
    printf("<html>"); 
    printf("<head><title>addIPC</title></head><body>"); 
    //printf("<p>hello world</p>"); 

    char *input;
    char *req_method; 
    FILE *fp;  
	char ipaddr[20];
	char name[20]; 
	char password[20]; 
	char rtspport[20]; 
    char httpport[20]; 
    char alias[150];
    int err_num = 0;
    //char file_num = 0;


    ipc_config[0] = ipc0_config;
	ipc_config[1] = ipc1_config;
	ipc_config[2] = ipc2_config;
	ipc_config[3] = ipc3_config;
	ipc_config[4] = ipc4_config;
	ipc_config[5] = ipc5_config;
	ipc_config[6] = ipc6_config;
	ipc_config[7] = ipc7_config;
	ipc_config[8] = ipc8_config;
	ipc_config[9] = ipc9_config;
   
    req_method = getenv("REQUEST_METHOD");//访问页面时的请求方法
    input = getcgidata(stdin, req_method);

    //获取检查ip地址;
    parse_data(input,ipaddr,"ipaddr=");
    if(ipaddr == NULL)
    {
		err_num = 2;
		goto over;
    }
	if(check_ip(ipaddr) < 0)
    {
		err_num = 3;
		goto over;				
    }
    if(ipaddr_isuse(ipaddr) == -1)
    {
        err_num = 10;
		goto over;	
    }

    //获取检查用户名
    parse_data(input,name,"name=");
    if(name == NULL)
    {
		err_num = 2;
		goto over;
    }
    if(check_string_len(name,16)<0)
    {
		err_num = 4;
		goto over;				
    }

    //获取检查密码
    parse_data(input,password,"password=");
    if(password == NULL)
    {
		err_num = 2;
		goto over;
    }
    if(check_string_len(password,16)<0)
    {
		err_num = 5;
		goto over;				
    }

    //获取检查rtsp端口号
    parse_data(input,rtspport,"rtspport=");
    if(rtspport == NULL)
    {
		err_num = 2;
		goto over;	
    }
	if(check_port(rtspport) < 0)
    {
		err_num = 6;
		goto over;					
    }

    //获取检查http端口号
    parse_data(input,httpport,"httpport=");
    if(httpport == NULL)
    {
		err_num = 2;
		goto over;	
    }
	if(check_port(httpport) < 0)
    {
		err_num = 7;
		goto over;					
    }

    //获取检查别名长度
    parse_data(input,alias,"alias=");
    if(alias == NULL)
    {
		err_num = 2;
		goto over;	
    }
	if(check_string_len(alias,150)<0)
    {
		err_num = 8;
		goto over;					
    }

	//查找文件
    for(file_num=0;file_num<MAX_IPC_NUM;file_num++)
    {
		if(access(ipc_config[file_num], 0) == -1)//文件不存在
        {
			//printf( "<p> file_num = %d </p>",file_num);
        	break;
        }
    }
    if(file_num == MAX_IPC_NUM)
    {
		//printf( "<p> file_num = %d </p>",file_num);
        err_num = 9;
        goto over;
    }

    //参数合法即写入文件 
    fp = fopen(ipc_config[file_num],"w");//只写打开文件，没有文件则创建，有则删除再建立新的文件
    if(fp == NULL)
    {          
		err_num = 1;
		goto over;
    }  

	if(EOF == fputs(input, fp))
    {
		err_num = 1;
		goto over;
    }
	fflush(fp);  
    fclose(fp); 

over:
	switch(err_num)
    {
		case 0:printf("<p>%s</p>",package_json(0,"add ipc success"));break;
        
        case 1:printf("<p>%s</p>",package_json(1,"gateway write file error"));break;

		case 2:printf("<p>%s</p>",package_json(2,"input parameter error"));break;

		case 3:printf("<p>%s</p>",package_json(3,"input ip address parameter error"));break;

		case 4:printf("<p>%s</p>",package_json(4,"input user name parameter error exceed max len"));break;

		case 5:printf("<p>%s</p>",package_json(5,"input password parameter error exceed max len"));break;

		case 6:printf("<p>%s</p>",package_json(6,"input rtspport parameter error exceed 0-65535"));break;

		case 7:printf("<p>%s</p>",package_json(7,"input httpport parameter error exceed 0-65535"));break;

		case 8:printf("<p>%s</p>",package_json(8,"input alias parameter error exceed max len"));break;

		case 9:printf("<p>%s</p>",package_json(9,"ipc total num reach max ipc num 10"));break;

        case 10:printf("<p>%s</p>",package_json(10,"ipaddr is already used error"));break;

        default:break;
    }
    printf("</body></html>"); 

    return 0;
}



