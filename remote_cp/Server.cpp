#include "Server.h"
Server::Server(char *s_addr,int s_p){
	this->server_port=s_p;

	this->server_addr=(char*)malloc(16);
	
	memset(this->server_addr,16,0);
	
	strcpy((char*)this->server_addr,s_addr);

	this->key=(unsigned char*)malloc(KEY_LENGTH);
	
	struct sockaddr_in server_addr;
	bzero(&server_addr,sizeof(server_addr));
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=inet_addr(this->server_addr);
	server_addr.sin_port=htons(this->server_port);

	this->server_socket=socket(AF_INET,SOCK_STREAM,0);
	if(this->server_socket<0){
		cout<<"Create socket failed"<<endl;
		exit(1);
	}
	if(bind(this->server_socket,(struct sockaddr*)&server_addr,sizeof(server_addr))){
		cout<<"Bind port "<<this->server_port<<" failed"<<endl;
		exit(1);
	}
	if(listen(this->server_socket,5)){
		cout<<"Server listen failed"<<endl;
		exit(1);
	}

}
int Server::get_client_connection(){
	struct sockaddr_in client_addr;
	socklen_t length=sizeof(client_addr);
	int client_sock=accept(this->server_socket,(struct sockaddr*)&client_addr,&length);
	if(client_sock<0){
		cout<<"Accept failed"<<endl;
		return -1;
	}
	char *addr;

	cout<<"Accept connection from "<<inet_ntoa(client_addr.sin_addr)<<":"<<ntohs(client_addr.sin_port)<<endl;
	return client_sock;

}
Server::~Server(){
	free(this->server_addr);
	free(this->key);
	free(this->from);
	free(this->to);
}