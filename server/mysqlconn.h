#ifndef MYS
#define MYS
#include<mysql/mysql.h>
#include"process.h"
#define MYHOST "localhost"
#define MYUSER "zz"
#define MYPASS "123"
#define MYDB "tchat"
#define MYPORT 3306
int getfriendlist(int userid,struct client_friend *my,rb_tree *friendlist);
int try_userid_to_database(const char *buf);
#endif
