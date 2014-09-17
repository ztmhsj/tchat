#ifndef SER
#define SER 
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include"rb_tree.h"
#include<unistd.h>
#define MAXLENGHT 5000
#define HEADBYTES (sizeof(int)+1)
#define GRM_ONLINE 0
#define GRM_ONLINE_LENGTH 26
#define GRM_REFRESH 4
#define GRM_REFRESH_LENGTH (sizeof(int)+sizeof(struct sockaddr))
#define GRM_REFRESH_SERVER_LENGTH (sizeof(int))
#define GRM_MESSAGE 1
#define USER_NAME 10
#define PASSWD 16
void *process_query(void *arg);
void p_error(char *,int );
typedef struct{
	int sockcnfd;
	struct sockaddr client_soaddr;
}cnfd_addr;
rb_tree uid_sock;

#endif 
