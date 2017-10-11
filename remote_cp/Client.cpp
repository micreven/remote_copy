#include "Client.h"
void Client::network_init(char *s_addr,int s_p,char *c_addr,int c_p){
	this->server_port=s_p;
	this->client_port=c_p;
	
	this->server_addr=(char*)malloc(16);
	this->client_addr=(char*)malloc(16);

	memset(this->server_addr,16,0);
	memset(this->client_addr,16,0);

	strcpy(this->server_addr,s_addr);
	strcpy(this->client_addr,c_addr);

	this->key=(unsigned char*)malloc(KEY_LENGTH);

	struct sockaddr_in client_addr;
	bzero(&client_addr,sizeof(client_addr));
	client_addr.sin_family=AF_INET;
	client_addr.sin_addr.s_addr=inet_addr(this->client_addr);
	client_addr.sin_port=htons(this->client_port);
	this->sock=socket(AF_INET,SOCK_STREAM,0);
	if(this->sock<0){
		cout<<"Create socket failed"<<endl;
		exit(1);
	}
	if(bind(this->sock,(struct sockaddr*)&client_addr,sizeof(client_addr))){
		cout<<"Bind port failed"<<endl;
		exit(1);
	}
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	if(inet_aton(this->server_addr,&server_addr.sin_addr)==0){
		cout<<"Server ip address error"<<endl;
		exit(1);
	}
	server_addr.sin_port=htons(this->server_port);
	if(connect(this->sock,(struct sockaddr*)&server_addr,sizeof(server_addr))<0){
		cout<<"Cannot connect to "<<this->server_addr<<endl;
		exit(1);
	}
	cout<<"connect to "<<s_addr<<":"<<s_p<<" successfully"<<endl;
}
Client::Client(int argc, char** argv){
	int ch;
	while ((ch = getopt(argc, argv, "u:p:f:t:"))!= -1){
		switch(ch){
			case 'u':
				this->user=(unsigned char *)optarg;
				break;
			case 'p':
				this->pwd=(unsigned char *)optarg;
				break;
			case 'f':
				this->from=(unsigned char *)optarg;
				break;
			case 't':
				this->to=(unsigned char *)optarg;
				break;
			default:
				exit(1);

		}
	}
	// cout<<this->user<<endl<<this->pwd<<endl<<this->from<<endl<<this->to<<endl;
	if(strchr((char *)this->from,':')==NULL && strchr((char *)this->to,':')==NULL){
		cout<<"please specify the remote address"<<endl;
		exit(1);
	}
	unsigned char *pos=NULL;
	unsigned char *addr;
	unsigned char *path;
	pos=(unsigned char*)strchr((char *)this->from,':');
	if(pos){
		this->mode=GET;
		addr=(unsigned char*)malloc(pos-this->from+1);
		path=(unsigned char*)malloc(strlen((char*)pos));
		memset(addr,0,pos-this->from+1);
		memset(path,0,strlen((char*)pos));
		memcpy(addr,this->from,pos-this->from);
		memcpy(path,pos+1,strlen((char*)pos+1));
		this->from=path;
	}else{
		pos=(unsigned char *)strchr((char *)this->to,':');
		this->mode=POST;
		addr=(unsigned char*)malloc(pos-this->to+1);
		path=(unsigned char*)malloc(strlen((char*)pos));
		memset(addr,0,pos-this->to+1);
		memset(path,0,strlen((char*)pos));
		memcpy(addr,this->to,pos-this->to);
		memcpy(path,pos+1,strlen((char*)pos+1));
		this->to=path;
	}
	cout<<addr<<endl<<path<<endl;

	network_init("127.0.0.1",8888,(char *)addr,9999);
}
Client::~Client(){
	close(this->sock);
	
}
