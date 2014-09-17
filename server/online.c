#include"online.h"
#include"mysqlconn.h"
static int add_in_server(int conn_id,cnfd_addr *cd);
static int to_client_message(int sockcnfd,struct client_friend *my,rb_tree *friendlist);
static int get_content_cid(int sockcnfd,char *head,int *conn_id);
static void fill_client_firend(data *datapt,void **arg,int size);
static void refresh_other_client(cnfd_addr *cd,const int conn_id,rb_tree *friendlist);


int client_online(cnfd_addr *cd,rb_tree *friendlist,char *head,struct client_friend *my)
{
    int conn_id;
    if(get_content_cid(cd->sockcnfd,head,&conn_id)<0)
        return -1;
    if(add_in_server(conn_id,cd)<0)
        return -1;
    if(getfriendlist(conn_id,my,friendlist)<0)
        return -1;
    if(to_client_message(cd->sockcnfd,my,friendlist)<0)
        return -1;
    refresh_other_client(cd,conn_id,friendlist);
}
int get_content_cid(int sockcnfd,char *head,int *conn_id)
{
    //数据报是 head1|char[10]用户名|char[16]密码
    int content_length=getlength(head);
	printf("content length is %d\n",content_length);
    char buf[GRM_ONLINE_LENGTH];
    if(content_length!=GRM_ONLINE_LENGTH){
        fprintf(stderr,"error in get_content_cid\n");
        return -1;
    }
    if(readn(sockcnfd,buf,content_length)!=content_length)
        return -1;
	*(buf+content_length-1)='\0';
	printf("content is %s\n",buf);
    if((*conn_id=try_userid_to_database(buf))<0)
        return -1;
    return 0;
}
int add_in_server(int conn_id,cnfd_addr *cd)
{
    data *d=makedata(conn_id,(void *)cd,sizeof(cnfd_addr));
    if(d==NULL)
        return -1;
    pthread_mutex_lock(&lock);
    insertree(&uid_sock,d);
    pthread_mutex_unlock(&lock);
    return 0;
}
int to_client_message(int sockcnfd,struct client_friend *my,rb_tree *friendlist)
{
    struct client_friend *tempfp;
    int content_length=sizeof(struct client_friend)*(friendlist->size+1);
    char *pt=(char *)malloc(HEADBYTES+content_length);
    struct client_friend *fp=(struct client_friend *)set_head(pt,content_length,GRM_ONLINE);
    (*fp)=*my;
    fp+=1;
    tempfp=fp;
    traversaltree(friendlist,fill_client_firend,(void **)&tempfp,0);
    if(writen(sockcnfd,pt,HEADBYTES+content_length)<0)
        return -1;
	free(pt);
    return 0;
}
void fill_client_firend(data *datapt,void **arg,int size)
{
    struct client_friend *fp=(struct client_friend *)(*arg);
    struct friend *sfp=(struct friend *)datapt->content;
    fp->user_id=datapt->key;
    strncpy(fp->user_name,sfp->user_name,USER_NAME);
    fp->face=sfp->face;
	fp->online=sfp->online;
    fp->address=sfp->address;
	++(*(struct client_friend **)arg);
}
void refresh_other_client(cnfd_addr *cd,const int conn_id,rb_tree *friendlist)
{
    //head4|onlineid|address;
    char message[HEADBYTES+GRM_REFRESH_LENGTH];
    int *pt=(int *)set_head(message,GRM_REFRESH_LENGTH,GRM_REFRESH);
    *pt=conn_id;
	*(struct sockaddr *)(pt+1)=cd->client_soaddr;
	void *arg=message;
    traversaltree(friendlist,transfer_refresh_message,(void **)&arg,sizeof(message));
}

