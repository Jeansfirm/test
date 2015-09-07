#include<stdio.h>

#include"online_detect.h"

void main()
{
	init_online_detect();
	char a=get_ipc_status("192.168.1.172");
	printf("now status:%c\n",a);
}
