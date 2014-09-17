#include"server.h"
#include"process.h"
#include"message.h"
#include"process.h"
static int find_destsock(int destid,rb_tree *friendlist);
int transfer_message(int sockcnfd,int length,rb_tree *friendlist,struct client_friend *my)
{
	char *buf,*message;
	int online,destid;
	buf=calloc(sizeof(char),length);
	readn(sockcnfd,buf,length);
	destid=*(int *)buf;
	online=*(int *)(buf+sizeof(int));
	int rv=0;
	if(online)
		rv=to_message(destid,buf,length,friendlist,my);
	else
		rv=afk_message(destid,buf);
	free(buf);
	printf("rv %d \n",rv);
	return rv;
}
int to_message(int destid,char *buf,int length,rb_tree *friendlist,struct client_friend *my)
{
	int destsock,rv;
	char *grm=malloc(HEADBYTES+length);
	char *pt=set_head(grm,length,1);
	*(int *)pt=my->user_id;
	*((int *)pt+1)=1;
	strncpy(pt+sizeof(int)*2,buf+sizeof(int)*2,length-sizeof(int)*2);
	if((destsock=find_destsock(destid,friendlist))<0){
		free(grm);
		return -1;
	}
	rv=writen(destsock,grm,HEADBYTES+length);
	printf("rv is %d\n",rv);
	printf("to %d transfer %s\n",destid,pt+sizeof(int)*2);
	free(grm);
	return rv;
}
int afk_message(int destid,char *buf)
{
	printf("error in afk message is");
}
static int find_destsock(int destid,rb_tree *friendlist)
{
	data *datapt;
	int destsock;
	if((datapt=searchdata(friendlist,destid))==NULL){
		fprintf(stderr,"error in find\n");
		return -1;
	}
	destsock=((struct friend *)datapt->content)->sockcnfd;
	return destsock;
}



	
