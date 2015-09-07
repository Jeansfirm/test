#include<stdio.h>

void main()
{
	FILE *fp_count,*fp_old,*fp_rm;
	char buffer_count[30],buffer_old[100];
	char rm_cmd[300];
	int close_count;
	int pic_num;
	
	fp_count=popen("ls ./ | wc -l ","r");
	while(!feof(fp_count))
	{
		fgets(buffer_count,sizeof(buffer_count),fp_count);
	}
	close_count=pclose(fp_count);
	printf("%s\n",buffer_count);
	if(close_count==-1)
	{
		//err_num=-19;
		//goto over;
	}
	pic_num = atoi(buffer_count);
	while(pic_num>14)
	{
		fp_old=popen("ls -t ./ | tail -n 1","r");
		while(!feof(fp_old))
		{
			fgets(buffer_old,sizeof(buffer_old),fp_old);
		}
		pclose(fp_old);
		printf("ready to remove %s\n ",buffer_old);
		sprintf(rm_cmd,"rm -rf /var/www/IPCRecord/%s",buffer_old);
		printf("%s\n ",rm_cmd);
		fp_rm=popen(rm_cmd,"r");
		if(fp_rm==NULL)
		{
			printf("fp_rm fail\n");
		}
		pclose(fp_rm);
		system("sync");
		pic_num--;
	}
}
