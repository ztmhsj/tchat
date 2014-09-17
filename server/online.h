#ifndef ONL
#define ONL
#include"server.h"
#include"process.h"
int client_online(cnfd_addr *cd,rb_tree *friendlist,char *head,struct client_friend *my);
extern pthread_mutex_t lock;
#endif
