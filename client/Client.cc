#include"Client"
int process_grmbuf(char *buf);
int store_in_grmbuf(const int &sockcnfd,char *buf);
int store_in_grmbuf(const int &sockcnfd,char *buf)
{
	static vector<string> grmbuf;
	unsigned int length=*(int *)buf;
	char *tempbuf=(char *)malloc(length+HEADBYTES);
	strncpy(tempbuf,buf,HEADBYTES);
	read(sockcnfd,tempbuf+HEADBYTES,length);
	grmbuf.push_back(string(tempbuf,tempbuf+HEADBYTES+length));
}
Client::Client(const string &name,const string &passwd):me(name,passwd)
{
	int sock;
	unsigned buf;
	struct sockaddr_in addr;
	addr.sin_family=AF_INET;
	addr.sin_port=htons(9999);
	inet_pton(AF_INET,"127.0.0.1",&buf);
	addr.sin_addr.s_addr=buf;
	sock=socket(AF_INET,SOCK_STREAM,0);
	connect(sock,(struct sockaddr *)&addr,sizeof(addr));
	sockcnfd=sock;
}
void Client::online_client()
{
	const int buflength=HEADBYTES+GRM_ONLINE_LENGTH;
	char buf[buflength];
	memset(buf,' ',buflength);
	sendonline(buf,buflength);
	unsigned int content_length;
	int type;
	while(1){
		type=gethead(buf,content_length);
		if(type<0)
			error("帐号名或密码错误",1);
		if(type!=0)
			store_in_grmbuf(sockcnfd,buf);
		else
			break;
	}
	setfriend(content_length);
}
void Client::sendonline(char *buf,const int &buflength)
{
	char *pt=sethead(buf,GRM_ONLINE_LENGTH,GRM_ONLINE);
	const char *tempt=me.user_name.c_str();
	strncpy(pt,tempt,USER_NAME);
	tempt=me.passwd.c_str();
	strncpy(pt+USER_NAME,tempt,PASSWD);
	int i=writen(buf,buflength);
	if(i<0)
		error("error in sendonline",1);
	me.passwd=string();
}
char *Client::sethead(char *buf,const int length,const int type)
{
	*(int *)buf=length;
	*(buf+sizeof(int))=type;
	return buf+HEADBYTES;
}
int Client::setfriend(const int &content_length)
{
	Friend tef;
	unique_ptr<string> upt(new string(content_length,' '));
	const char *tp=(*upt).c_str();
	const struct client_friend *pt=
		reinterpret_cast<const struct client_friend *>(tp);
	if(readn((char *)pt,content_length)<0)
		error("error in serfriend",1);
	const struct client_friend* endpt=
		(struct client_friend *)(tp+content_length);
	unsigned int temp_uid;
	me.face=pt->face;
	me.address=pt->address;
	user_id=pt->user_id;
	++pt;
	while(pt!=endpt){
		temp_uid=pt->user_id;
		if(strnlen(pt->user_name,USER_NAME)<USER_NAME)
			tef.user_name=string(pt->user_name);
		else
			tef.user_name=string(pt->user_name,USER_NAME);
		tef.online=pt->online;
		tef.face=pt->face;
		tef.address=pt->address;
		++pt;
		ff.insert(pair<unsigned int,Friend>(temp_uid,tef));
	}
	return ff.size();
}
void Client::print_me()
{
	mvwprintw(cc.fwin,1,1,"login id : %d ",user_id);
	mvwprintw(cc.fwin,1,15," name : %s",me.user_name.c_str());
	wrefresh(cc.fwin);
	/*
	cout<<"login id is "<<user_id <<endl;
	cout<<"name is "<<me.user_name<<endl;
	*/
}
void Client::print_friendlist()
{
	mvwprintw(cc.fwin,2,1,"friend:");
	int p=3;
	for(map<int,Friend>::const_iterator i=ff.cbegin();i!=ff.cend();++i,++p){
		mvwprintw(cc.fwin,p,1,"id %d : ",i->first);
		mvwprintw(cc.fwin,p,9," %s   %d   ",
				i->second.user_name.c_str(),(i->second.online?1:0));
	}
	mvwprintw(cc.iwin,0,0,"$$$:");
	wrefresh(cc.fwin);
	wrefresh(cc.mwin);
}
void Client::working()
{
	pthread_t ta;
	pthread_create(&ta,NULL,operate,this);
	deamon();
	pthread_join(ta,NULL);
}
void Client::deamon()
{
	char buf[HEADBYTES];
	unsigned int content_length,type;
	while(type=gethead(buf,content_length)){
	//	cout<<"type "<<type<<endl;
		switch(type){
			case 1:
				receive_message(content_length);
				break;
			case GRM_REFRESH:
				refresh_friend();
				break;
			case 2:
	//			get_friend();
				break;
			default:
				error("error in deamon",1);
				break;
		}
	}
}
void Client::receive_message(const unsigned int &content_length)
{
	char *buf=(char *)malloc(content_length);
	if(readn(buf,content_length)<=0)
		error("error in display_message",1);
	const char *content=buf+sizeof(int)*2;
	string rece_name=ff.find(*(int *)buf)->second.user_name;
	add_history(content,rece_name);
	print_message(content,rece_name);
	free(buf);
}
void Client::print_message(const char *content,string rece_name)
{
	string ss(content);
	rece_name=rece_name+':'+ss;
	wrefresh(cc.mwin);
	mymess.push_back(rece_name);
	int y=0;
	for(auto itb=mymess.crbegin();itb!=mymess.crend();++itb){
		y+=itb->size()/(cc.mx-2);
		if(itb->size()%(cc.mx-2)!=0)
			y+=1;
		int pot=cc.my-2-y;
		if(pot<1)
			break;
		mvwprintw(cc.mwin,pot,0,"%s\n",itb->c_str());
	}
	wrefresh(cc.mwin);
}
void Client::add_history(const char *content,string rece_name)
{
	if(access("./history/",F_OK)<0){
		mkdir("./history",0711);
		creat("./history/record",0700);
	}
	rece_name+=':';
	FILE *fp;
	if((fp=fopen("./history/record","a"))==NULL)
		error("error in history",1);
	fprintf(fp,"%s:%s\n",rece_name.c_str(),content);
	fclose(fp);
}
void Client::refresh_friend()
{
	char buf[GRM_REFRESH_LENGTH];
	if(readn(buf,GRM_REFRESH_LENGTH)<=0)
		error("error in refresh_friend",1);
	int cn_id=*(int *)buf;
	struct sockaddr address=*(struct sockaddr *)((int *)buf+1);
	auto fit=ff.find(cn_id);
	if(fit==ff.end())
		error("error in refresh",1);
	fit->second.online=1;
	fit->second.address=address;
	mvwprintw(cc.fwin,LINES-2,4,"%s  alrealy online ",fit->second.user_name.c_str());
	//传回客户端 刷新客户端列表 head|user_id
	char re_grm[HEADBYTES+GRM_REFRESH_SERVER_LENGTH];
	int *pt=(int *)sethead(re_grm,GRM_REFRESH_SERVER_LENGTH,4);
	*pt=cn_id;
	if(writen(re_grm,HEADBYTES+GRM_REFRESH_SERVER_LENGTH)<0)
		error("error in refresh",1);
	print_friendlist();
}

void *Client::operate(void *v)
{
	Client *cc=(Client *)v;
	int type,uid,online;
	while(type=cc->get_client_operate(uid,online)){
		switch(type){
			case 'p':
				cc->print_friendlist();
				break;
			case 'a':
			//	add_friend();
				break;
			case 't':
				cc->transfer_message(uid,online);
				break;
			case 'q':
				cout<<"欢迎您的使用"<<endl;
				exit(0);  //应该发信号
				break;
			default :
				error("error in operate",1);
		}
	}
}


int Client::get_client_operate(int &uid,int &online)
{
	char ibuf[20];
	mvwprintw(cc.iwin,0,0,"$$$:");
	wrefresh(cc.iwin);
	char ch;
	while(wgetnstr(cc.iwin,ibuf,20)>=0){
		wclear(cc.iwin);
		mvwprintw(cc.iwin,0,0,"$$$:");
		wrefresh(cc.iwin);
		if(strcmp(ibuf,"quit")==0){
			delwin(cc.iwin);
			delwin(cc.fwin);
			delwin(cc.mwin);
			endwin();
			exit(0);
		}else if(strncmp("t ",ibuf,2)==0||strncmp("t\t",ibuf,2)==0){
			string ss(ibuf);
			istringstream iss(ss);
			string sid;
			iss>>sid;
			iss>>sid;
				mvwprintw(cc.mwin,2,1,"%s",sid.c_str());
				wrefresh(cc.mwin);
			bool d=true;
			for(auto c:sid)
				if(!isdigit(c))
					d=false;
			if(d==true){
				int i=stoi(sid);
				map<int,Friend>::const_iterator it;
				if((it=ff.find(i))!=ff.end()){
					online=it->second.online;
					uid=i;
					return 't';
				}else{
				mvwprintw(cc.mwin,cc.my-2,1,"don't have this firend id ");
				wrefresh(cc.mwin);
				mvwprintw(cc.mwin,cc.my-2,1,"                           ");
				}
			}else{
				mvwprintw(cc.mwin,cc.my-2,1,"please into t and firend id");
				wrefresh(cc.mwin);
				mvwprintw(cc.mwin,cc.my-2,1,"                           ");
			}
		}
		else{
			mvwprintw(cc.mwin,cc.my-2,1,"please into currect");
			wrefresh(cc.mwin);
			mvwprintw(cc.mwin,cc.my-2,1,"                    ");
		}
	}
}
int Client::writen(char *message,const int size)
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
	return size-right;
}
int Client::transfer_message(const int &destid,const int &online)
{
	//head|destid|content
	char message[5000];
	mvwprintw(cc.iwin,0,0,"<<<<");
	wrefresh(cc.iwin);

	if((wgetnstr(cc.iwin,message,5000))>=0){
		int count=strlen(message);
		int length=count+sizeof(int)*2+1;
		char *grm=(char *)calloc(sizeof(char),length+HEADBYTES);
		char *content=sethead(grm,length,GRM_MESSAGE);
		*(int *)content=destid;
		*(int *)(content+sizeof(int))=online;
		strncpy(content+sizeof(int)*2,message,count);
		if(writen(grm,length+HEADBYTES)<=0)
			error("error in message",1);
		wclear(cc.iwin);
		wrefresh(cc.iwin);
		add_history(content+sizeof(int)*2,me.user_name);
		print_message(content+sizeof(int)*2,me.user_name);
		free(grm);
	}
}
int Client::readn(char *buf,const int size)
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
int Client::gethead(char *buf,unsigned int &content_length)
{
	//建立栈 缓冲上线过程有用户下线发送数据报
	if(read(sockcnfd,buf,HEADBYTES)<=0)
		return -1;
	content_length=*(int *)buf;
	return *(buf+sizeof(int));
}
void Client::error(const string &ss,const int &i)
{
	cout<<"error : "<<i<<endl;
	cout<<ss<<endl;
	exit(1);
}

Appearance::Appearance()
{
	initscr();  
	fy=0;
	fx=COLS/3;
	fwin=newwin(0,fx,0,0);
	my=LINES-LINES/3;
	mx=COLS/3*2;
	mwin=newwin(my,mx,0,COLS/3);
	iy=LINES/3;
	ix=0;
	iwin=newwin(iy,0,LINES-LINES/3,COLS/3);
	keypad(iwin,1);
	scrollok(iwin,1);
	wsetscrreg(iwin,0,iy-1);
	wborder(fwin,'|','|','-','-','+','+','+','+');
	wborder(mwin,'|','|','-','-','+','+','+','+');
}

