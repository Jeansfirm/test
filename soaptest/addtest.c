#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int add(const char *server,int num1,int num2, int *sum);

int main(int argc,char **argv)
{
	int result=1;
	char server[128]={0};
	int num1;
	int num2,sum;

	if(argc<4)
	{
		printf("usage: %s <ip:port> num1 num2  \n",argv[0]);
		exit(1);	
	}

	strcpy(server,argv[1]);
	num1 = atoi(argv[2]);
	num2 = atoi(argv[3]);
	result = add(server,num1,num2,&sum);

	if(result!=0)
	{
		printf("soap error, errcode=%d\n", result);  
	}else{
		printf("%d + %d = %d\n", num1, num2, sum); 
	}

	return 0;

}
