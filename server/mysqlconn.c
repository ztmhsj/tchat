#include"mysqlconn.h"
#include<string.h>
MYSQL mysql;
MYSQL_RES *result;
MYSQL_ROW row;
char sql[1000];
static int getmy(char *arrid,struct client_friend *my);
static int getlist(char *arrid,rb_tree *friendlist);
int try_userid_to_database(const char *buf)
{
	char user_name[USER_NAME],passwd[PASSWD],*pt;
	int user_id,column;
	strncpy(user_name,buf,USER_NAME);
	strncpy(passwd,buf+USER_NAME,PASSWD);
	printf("get user_name is %s ,get passwd is %s \n",user_name,passwd);
	mysql_init(&mysql);
	mysql_real_connect(&mysql,MYHOST,MYUSER,MYPASS,MYDB,MYPORT,NULL,0);
	strcpy(sql,"select user_id,passwd from user where user_name='");
	strncat(sql,user_name,USER_NAME);
	strcat(sql,"'");
	printf("sql is %s\n",sql);
	//一下是error 是测试
	if(mysql_query(&mysql,sql)!=0){
		fprintf(stderr,"error in select1\n");
		return -1;
	}
	if((result=mysql_store_result(&mysql))==NULL){
		fprintf(stderr,"error in select2\n");
		return -1;
	}
	if((column=mysql_num_fields(result))!=2){ //空行column也是2
		mysql_free_result(result);
		fprintf(stderr,"error in select3\n");
		return -1;
	}
	printf("column column %d\n",column);
	if((row=mysql_fetch_row(result))==NULL){
		mysql_free_result(result);
		fprintf(stderr,"error in select 4");
		return -1;
	}
	printf("get passwd is %s\n",passwd);
	printf("database passwd is %s\n",row[1]);
	if(strncmp(row[1],passwd,PASSWD)!=0){
		fprintf(stderr,"error passwd2\n");
		return -1;
	}
	mysql_free_result(result);
	printf("user_name %s id is %d\n",user_name,atoi(row[0]));
	return atoi(row[0]);
}
int getfriendlist(int userid,struct client_friend *my,rb_tree *friendlist)
{
	int i;
	char arrid[11];
	sprintf(arrid,"%d",userid);
	if(getmy(arrid,my)<0){
		return -1;
		fprintf(stderr,"error\n");
	}
	i=getlist(arrid,friendlist);
	printf("get friend list count is %d \n",i);
	return i;
}
static int getmy(char *arrid,struct client_friend *my)
{
	data *datapt;
	strcpy(sql,"select user_id,user_name,face from user where user_id=");
	strcat(sql,arrid);
	printf("sql is :%s\n",sql);
	mysql_query(&mysql,sql);
	if((result=mysql_store_result(&mysql))==NULL){
		printf("unbliveble\n");
		return -1;
	}
	row=mysql_fetch_row(result);
	my->user_id=atoi(row[0]);
	strcpy(my->user_name,row[1]);
	my->face=atoi(row[2]);
	datapt=searchdata(&uid_sock,my->user_id);
	printf("data key is %d\n",datapt->key);
	my->address=((cnfd_addr *)datapt->content)->client_soaddr;
	printf("my : %d , %s , %d \n",my->user_id,my->user_name,my->face);
	mysql_free_result(result);
	return 0;
}
static int getlist(char *arrid,rb_tree *friendlist)
{
	struct friend ff;
	data *datapt;
	strcpy(sql,"select user_id,user_name,face from user where user_id in (select friend_id from friend where user_id=");
	strcat(sql,arrid);
	strcat(sql,")");
	printf("sql is : %s\n",sql);
	mysql_query(&mysql,sql);
	if((result=mysql_store_result(&mysql))==NULL)
		return 0;
	while(row=mysql_fetch_row(result)){
		ff.user_id=atoi(row[0]);
		strcpy(ff.user_name,row[1]);
		printf("ff.user_name is  %s\n",ff.user_name);
		ff.face=atoi(row[2]);
		printf("friend user_id %d\n",ff.user_id);
		if((datapt=searchdata(&uid_sock,ff.user_id))!=NULL){
			ff.address=((cnfd_addr *)datapt->content)->client_soaddr;
			ff.sockcnfd=((cnfd_addr *)datapt->content)->sockcnfd;
			ff.online=1;
		}else{
			memset(&ff.address,0,sizeof(ff.address));
			ff.sockcnfd=-1;
			ff.online=0;
		}
		datapt=makedata(ff.user_id,&ff,sizeof(struct friend));
		insertree(friendlist,datapt);
	}
	mysql_free_result(result);
	return friendlist->size;
}

		






