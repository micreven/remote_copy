#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <time.h>
#include "Client.h"
#include "Util.h"
int main(int argc, char** argv){
	Client client=Client(argc, argv);
	client.key=	getsecretkey(client.sock,CLIENT);
	cout<<"send user:"<<client.user<<endl;
	encrypt_send(client.sock,client.user,strlen((char*)client.user),client.key,KEY_LENGTH);
	cout<<"send pwd:"<<client.pwd<<endl;
	encrypt_send(client.sock,client.pwd,strlen((char*)client.pwd),client.key,KEY_LENGTH);

	cout<<"method:"<<(client.mode==GET?"GET":"POST")<<endl;
	cout<<"from:"<<client.from<<endl;
	cout<<"to:"<<client.to<<endl;

	encrypt_send_int(client.sock,client.mode,client.key,KEY_LENGTH);
	encrypt_send(client.sock,client.from,strlen((char*)client.from)+1,client.key,KEY_LENGTH);
	encrypt_send(client.sock,client.to,strlen((char*)client.to)+1,client.key,KEY_LENGTH);

	if(client.mode==GET){
		cout<<"getting "<<client.from<<"from remote server..."<<endl;
		int buflen=0;
		unsigned char *buf=decrypt_recv(client.sock,client.key,KEY_LENGTH,&buflen);
		FILE *f=fopen((char*)client.to,"wb");
		fwrite(buf,1,buflen,f);
		fclose(f);
	}else{
		cout<<"posting "<<client.from<<" to remote server..."<<endl;
		int fsize=getfilesize((char*)client.from);
		cout<<client.from<<":"<<fsize<<endl;
		FILE *f=fopen((char*)client.from,"rb");
		unsigned char *buf=(unsigned char*)malloc(fsize);
		fread(buf,1,fsize,f);
		encrypt_send(client.sock,buf,fsize,client.key,KEY_LENGTH);
		fclose(f);
	}
	cout<<"success"<<endl;

	return 0;
}