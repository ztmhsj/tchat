#ifndef NET
#define NET
#include<sys/socket.h>
#include<arpa/inet.h>
#include<stdlib.h>
#include<sys/types.h>
#include<errno.h>
#include<stdio.h>
#define PORT 9999
#define LISTEN 50
int make_server_socket(int port,int listen_num);
#endif
