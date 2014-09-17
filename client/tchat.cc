#include"Client"
int main(int argc,char **argv)
{
	if(argc!=3){
		cout<<"error input"<<endl;
		return 0;
	}
	Client cc(argv[1],argv[2]);
	cc.online_client();
	cc.print_me();
	cc.print_friendlist();
	cc.working();

}
