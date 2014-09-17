#include"net.h"
int make_server_socket(int port,int listen_num)
{
	int sockfd,setvalue=1;
	struct sockaddr_in soadd;
	sockfd=socket(AF_INET,SOCK_STREAM,0);
	if(setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,
	&setvalue,sizeof(setvalue))<0){
		perror("error in sersockop");
		exit(1);
	}
	soadd.sin_family=AF_INET;
	soadd.sin_port=htons(port);
	soadd.sin_addr.s_addr=INADDR_ANY;
	if(bind(sockfd,(struct sockaddr *)&soadd,sizeof(soadd))<0){
		perror("error in bind");
		exit(1);
	}
	if(listen(sockfd,listen_num)<0){
		perror("error in listen");
		exit(1);
	}
	return sockfd;
}
