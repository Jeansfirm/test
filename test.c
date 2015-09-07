
#include<unistd.h>
#include<signal.h>

void handler()
{
    printf("hello\n");
    signal(SIGALRM,handler);
    alarm(4);
}

void main()
{
    int i;
    handler();
    for(i=0;i<21;i++)
    {
        
        printf("sleep  %d\n",i);
        sleep(1);
    }
}
