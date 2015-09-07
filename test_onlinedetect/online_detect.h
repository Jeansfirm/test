/***************************************************************************
  Copyright (C),  2009-2015 GuangdongGuanglian Electronic Technology Co.,Ltd.
  File name:      online_detect.h
  Description:    提供检测IPC在线接口
  Author:         jiang   
  Version:        1.0       
  Date:           2014-06-16
  History:           
                  
    1. Date:	2015-07-24
       Author:	pjf
       Modification:
    2. ...
***************************************************************************/
#ifndef _ONLINE_DETECT_H
#define _ONLINE_DETECT_H
int detect_ipc_online(char *ipaddr);
//int get_ipc_status(int*ipc_status);
char get_ipc_status(char *ipc_status);
int init_online_detect();
void init_data_online_detect();

#endif
