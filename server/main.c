#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<pthread.h>
#include"net.h"
#include"server.h"
#define SOCKCNFDBUF 20
void setup_detache(pthread_attr_t *attr); //设置独立线程
int main(void)
{
	int sockfd,sockcnfd,i;
	struct sockaddr client_soaddr;
	int size_addr=sizeof client_soaddr;
	cnfd_addr cnad_buf[SOCKCNFDBUF]={0}; //防止进程赋值时被修改
	pthread_t work;
	pthread_attr_t attr;
	sockfd=make_server_socket(PORT,LISTEN);
	setup_detache(&attr);
	initializetree(&uid_sock);
	for(i=0;i<SOCKCNFDBUF;++i){
		sockcnfd=accept(sockfd,&client_soaddr,&size_addr);
		printf("connect to socket %d\n",sockcnfd);
		cnad_buf[i].sockcnfd=sockcnfd;
		cnad_buf[i].client_soaddr=client_soaddr;
		pthread_create(&work,&attr,process_query,(void *)&cnad_buf[i]);
		if(i==SOCKCNFDBUF-1)
			i=0;
	}
	return 0;
}
void setup_detache(pthread_attr_t *attr)
{
	if(pthread_attr_init(attr)<0)
		p_error("error in detache",1);
	if(pthread_attr_setdetachstate(attr,PTHREAD_CREATE_DETACHED)<0)
		p_error("error in detache",1);
}
