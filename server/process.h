#ifndef PRO
#define PRO
#include"rb_tree.h"
#include<pthread.h>
#include"server.h"
char *set_head(char *gram,int size,int type);
int getuserid(char *buf);
int getlength(char *buf);
int writen(int sockcnfd,char *message,int size);
int readn(int sockcnfd,char *buf,int size);
void p_error(char *x,int y);
void transfer_refresh_message(data *datapt,void **arg,int size);


struct friend {
    int user_id;
    char user_name[USER_NAME];
    int online;
    int face;
    int sockcnfd;
    struct sockaddr address;
};
struct client_friend{
    int user_id;
    char user_name[USER_NAME];
    int face;
	int online;
    struct sockaddr address;
};
#endif
