#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <time.h>
#include "Server.h"
#include "Util.h"
int main(int argc, char** argv){
	int ch;
	unsigned char *u=NULL;
	unsigned char *p=NULL;
	while ((ch = getopt(argc, argv, "u:p:"))!= -1){
		switch(ch){
			case 'u':
				u=(unsigned char *)optarg;
				break;
			case 'p':
				p=(unsigned char *)optarg;
				break;
			default:
				exit(1);

		}
	}
	if(u&&p){
		cout<<"saving account "<<u<<":"<<p<<endl;
		saveaccount(u,p);
		return 0;
	}


	Server server=Server("127.0.0.1",8888);
	int sock=server.get_client_connection();

	server.key=getsecretkey(sock,SERVER);
	int user_len=0;
	unsigned char *user=decrypt_recv(sock,server.key,KEY_LENGTH,&user_len);
	int pwd_len=0;
	unsigned char *pwd=decrypt_recv(sock,server.key,KEY_LENGTH,&pwd_len);

	unsigned char* h=pwdhash((unsigned char*)pwd,pwd_len);

	if(!check_account(user,user_len,h,strlen((char*)h))){
		cout<<"invalidate account"<<endl;
		return 0;
	}

	cout<<"account verified"<<endl;

	server.mode=decrypt_recv_int(sock,server.key,KEY_LENGTH);
	int fromlen=0;
	server.from=decrypt_recv(sock,server.key,KEY_LENGTH,&fromlen);
	int tolen=0;
	server.to=decrypt_recv(sock,server.key,KEY_LENGTH,&tolen);

	cout<<"method:"<<(server.mode==GET?"GET":"POST")<<endl;
	cout<<"from:"<<server.from<<endl;
	cout<<"to:"<<server.to<<endl;

	if(server.mode==GET){
		cout<<"posting "<<server.from<<" to remote client..."<<endl;
		int fsize=getfilesize((char*)server.from);
		cout<<"file size:"<<server.from<<endl;
		FILE *f=fopen((char*)server.from,"rb");
		unsigned char *buf=(unsigned char*)malloc(fsize);
		cout<<"hahaha?"<<endl;
		fread(buf,1,fsize,f);
		cout<<"hehehe?"<<endl;
		fclose(f);
		encrypt_send(sock,buf,fsize,server.key,KEY_LENGTH);

	}else{
		cout<<"getting "<<server.from<<" from remote server..."<<endl;
		FILE *f=fopen((char*)server.to,"wb");
		int buflen=0;
		unsigned char *buf=decrypt_recv(sock,server.key,KEY_LENGTH,&buflen);
		fwrite(buf,1,buflen,f);
		fclose(f);
	}
	cout<<"success"<<endl;
	close(sock);
	return 0;
}