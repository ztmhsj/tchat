#include"rb_tree.h"
#include"process.h"
int transfer_message(int sockcnfd,int length,rb_tree *friendlist,struct client_friend *my);
int to_message(int destid,char *buf,int length,rb_tree *friendlist,struct client_friend *my);
int afk_message(int destid,char *buf);
