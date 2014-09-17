#include"server.h"
#include"process.h"
#include"online.h"
#include"message.h"
static int readhead(int sockcnfd,char *headbuf);
static int add_in_server(int conn_id,cnfd_addr *cd);
static inline int getdatagrmtype(char *buf);
static int refresh_list(int sockcnfd,rb_tree *friendlist);
	
pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;

void *process_query(void *arg)
{
	cnfd_addr cd;
	rb_tree friendlist;
	char headbuf[HEADBYTES];
	int type=0,length;
    struct client_friend my;

	cd=*(cnfd_addr *)arg;
	initializetree(&friendlist);
	while(1){
		if(readhead(cd.sockcnfd,headbuf)<0)   //读取数据报头
			return NULL;//client_offline();
		type=getdatagrmtype(headbuf);
		length=getlength(headbuf);
		printf("type is %d\n",type);
		switch(type){
			case 0:if(client_online(&cd,&friendlist,headbuf,&my)<0){//head,userid
					   close(cd.sockcnfd);
					   return NULL;
				   }
			//			   client_offline();
						   continue;
			case 1:if(transfer_message(cd.sockcnfd,length,&friendlist,&my)<0)
					   fprintf(stderr,"error in transfer_message");
					   continue;
			case 4:if(refresh_list(cd.sockcnfd,&friendlist)<0){
					   close(cd.sockcnfd);
				   }
				   continue;
			default:
			      // client_force_exit(cd.sockcnfd);
			       return NULL;
		}
	}
}
int readhead(int sockcnfd,char *headbuf)
{
	int count=read(sockcnfd,headbuf,HEADBYTES);
	if(count!=HEADBYTES){
		fprintf(stderr,"error in readhead\n");
		return -1;
	}
	printf("read head is %d ,%d\n",
			*(int *)headbuf,*((char *)(headbuf+sizeof(int))));
}
int getlength(char *buf)  //得到数据报长度 1个int
{
	int *pt=(int *)buf;
	return *pt;
}
static inline int getdatagrmtype(char *buf)
{
	return (int)*(buf+sizeof(int));
}
int getuserid(char *buf)
{
	int userid;
	int *pt=(int *)(buf+HEADBYTES);
	userid=*pt;
	return userid;
}
char *set_head(char *gram,int size,int type)
{
	*(int *)gram=size;
	*(char *)((int *)gram+1)=(char)type;
	return gram+sizeof(int)+sizeof(char);
}
int add_in_server(int conn_id,cnfd_addr *cd)
{
	data *d=makedata(conn_id,(void *)cd,sizeof(*cd));
	if(d==NULL)
		return -1;
	pthread_mutex_lock(&lock);
	insertree(&uid_sock,d);
	pthread_mutex_unlock(&lock);
	return 0;
}
void transfer_refresh_message(data *datapt,void **arg,int size)
{
	char *message;
	struct friend *fpt=(struct friend *)datapt->content;
	if(fpt->online==1){
		printf("发送一个 sockcnfd is %d",fpt->sockcnfd);
		message=*(char **)arg;
		writen(fpt->sockcnfd,message,size);
	}
}
static int refresh_list(int sockcnfd,rb_tree *friendlist)
{
	char buf[GRM_REFRESH_SERVER_LENGTH];
	int fid;
	data *fdatapt,*udatapt;
	if(readn(sockcnfd,buf,GRM_REFRESH_SERVER_LENGTH)<=0)
		return -1;
	fid=*(int *)buf;
	printf("%d alrealy online \n",fid);
	if((fdatapt=searchdata(friendlist,fid))==NULL)
		return -1;
	if((udatapt=searchdata(&uid_sock,fid))==NULL)
		return -1;
	((struct friend *)fdatapt->content)->online=1;
	((struct friend *)fdatapt->content)->sockcnfd=
		((cnfd_addr *)udatapt->content)->sockcnfd;
	((struct friend *)fdatapt->content)->address=
		((cnfd_addr *)udatapt->content)->client_soaddr;
	return fid;
}
			
void p_error(char *x,int y)
{
	perror(x);
	exit(y);
}
int writen(int sockcnfd,char *message,int size)
{
	int right,n;
	char *tempt=message;
	right=size;
	while(right>0){
		if((n=write(sockcnfd,tempt,size))<=0)
				return -1;
		right-=n;
		tempt+=n;
	}
	return size;
}
int readn(int sockcnfd,char *buf,int size)
{
	int right,n;
	char *tempt=buf;
	right=size;
	while(right>0){
		if((n=read(sockcnfd,tempt,size))<0)
			return -1;
		else if(n==0)
			break;
		right-=n;
		tempt+=n;
	}
	return size-right;
}
