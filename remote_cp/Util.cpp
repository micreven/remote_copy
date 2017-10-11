#include "Util.h"
void hexprint(unsigned char *printBuf, int len)
{
    int i;
    for(i = 0; i < len; i++)
    {
        printf("%02x ", printBuf[i]);
    }
        printf("\n");
}
void recvfile(int sock,char *path,unsigned int filesize){
	int recv_size=0;
	FILE *file=fopen(path,"wb");
	while(recv_size<filesize){
		int mallocsize=(filesize-recv_size)<RECV_FILE_BUFF_LENGTH?(filesize-recv_size):RECV_FILE_BUFF_LENGTH;
		char *buf=(char*)malloc(mallocsize);
		int t=recv(sock,buf,mallocsize,0);
		if(t<0){
			cout<<"Error occured while recving file data"<<endl;
			fclose(file);
			free(buf);
			exit(0);
		}
		recv_size+=t;
		fwrite(buf,sizeof(char),t,file);
		free(buf);
	}
	fclose(file);
}
void sendfile(int sock,char *path,unsigned int filesize){
	int send_size=0;
	FILE *file=fopen(path,"rb");
	while(send_size<filesize){
		int mallocsize=(filesize-send_size)<RECV_FILE_BUFF_LENGTH?(filesize-send_size):RECV_FILE_BUFF_LENGTH;
		char *buf=(char *)malloc(mallocsize);
		int t=fread(buf,sizeof(char),mallocsize,file);
		if(t<0){
			cout<<"Error occured while reading file"<<endl;
			free(buf);
			fclose(file);
			exit(1);
		}
		send(sock,buf,mallocsize,0);
		send_size+=t;
		free(buf);
	}
	fclose(file);
}
void sendint(int sock,int num){
	send(sock,(void*)&num,8,0);
}
int recvint(int sock){
	int num=0;
	void *buf=recvn(sock,8);
	memcpy(&num,buf,8);
	return num;
}
void *recvn(int sock,int size){
	int recv_size=0;
	char *buf=(char *)malloc(size);
	while(recv_size<size){
		int t=recv(sock,buf+recv_size,size<RECV_BUFF_LENGTH?size:RECV_BUFF_LENGTH,0);
		if(t<0){
			cout<<"Error occured while recving data"<<endl;
			free(buf);
			exit(1);
		}
		recv_size+=t;
	}
	return buf;
}
unsigned int getfilesize(char *path){
	unsigned int filesize = -1;      
    struct stat statbuff;  
    if(stat(path, &statbuff) < 0){  
        return filesize;  
    }else{  
        filesize = statbuff.st_size;  
    }  
    return filesize;
}


//RSA part
unsigned char *getsecretkey(int sock,int role){
	if(role==CLIENT){
		system("openssl genpkey -genparam -algorithm DH -out ./dhp.pem");
		int dhp_size=getfilesize("./dhp.pem");
		cout<<"dhp.pem file size:"<<dhp_size<<endl;
		sendint(sock,dhp_size);
		sendfile(sock,"./dhp.pem",dhp_size);

		system("openssl genpkey -paramfile dhp.pem -out client_dhkey.pem");
		system("openssl pkey -in client_dhkey.pem -pubout -out client_pub.pem");

		int server_pub_size=recvint(sock);
		cout<<"server_pub.pem file size:"<<server_pub_size<<endl;
		recvfile(sock,"./server_pub.pem",server_pub_size);
		cout<<"recv server_pub.pem finish"<<endl;

		int client_pub_size=getfilesize("./client_pub.pem");
		cout<<"client_pub.pem file size:"<<client_pub_size<<endl;
		sendint(sock,client_pub_size);
		sendfile(sock,"./client_pub.pem",client_pub_size);
		
		system("openssl pkeyutl -derive -inkey client_dhkey.pem -peerkey server_pub.pem -out secret.bin");


	 }else if(role==SERVER){
	 	int dhp_size=recvint(sock);
	 	cout<<"dhp.pem size:"<<dhp_size<<endl;
	 	recvfile(sock,"./dhp.pem",dhp_size);
	 	cout<<"get dhp.pem"<<endl;

	 	system("openssl genpkey -paramfile dhp.pem -out server_dhkey.pem");
	 	system("openssl pkey -in server_dhkey.pem -pubout -out server_pub.pem");

	 	int server_pub_size=getfilesize("./server_pub.pem");
	 	sendint(sock,server_pub_size);
	 	sendfile(sock,"./server_pub.pem",server_pub_size);
	 	
	 	int client_pub_size=recvint(sock);
	 	recvfile(sock,"./client_pub.pem",client_pub_size);

	 	system("openssl pkeyutl -derive -inkey server_dhkey.pem -peerkey client_pub.pem -out secret.bin");
	 }
	 cout<<"sercet key generation finished"<<endl;
	 int secret_file_size=getfilesize("./secret.bin");
	 FILE *file=fopen("./secret.bin","rb");
	 unsigned char *buf=(unsigned char*)malloc(secret_file_size);
	 fread(buf,sizeof(unsigned char),secret_file_size,file);
	 fclose(file);
	 //clean
	 system("rm ./*.pem");
	 system("rm ./*.bin");
	 hexprint(buf,secret_file_size);
	 return buf; 

}
void encrypt_send(int sock,unsigned char *text,int textlen,unsigned char *key,int keylen){
	int enc_size=getpaddedsize(textlen);
	unsigned char *enc=(unsigned char*)malloc(enc_size);

	aesencrypt(text,textlen,key,keylen,enc,enc_size);
	
	sendint(sock,textlen);
	sendint(sock,enc_size);
	send(sock,enc,enc_size,0);
}
unsigned char* decrypt_recv(int sock,unsigned char *key,int keylen,int *len){
	int textlen=recvint(sock);
	int enc_size=recvint(sock);
	unsigned char *enc=(unsigned char*)recvn(sock,enc_size);;
	unsigned char *text=(unsigned char*)malloc(enc_size);
	*len=textlen;
	aesdecrypt(enc,enc_size,key,keylen,text,textlen);
	unsigned char *text_raw=(unsigned char*)malloc(textlen);
	memcpy(text_raw,text,textlen);
	return text_raw;
}
void encrypt_send_int(int sock,int num,unsigned char *key,int keylen){
	encrypt_send(sock,(unsigned char*)&num,8,key,keylen);
}
int decrypt_recv_int(int sock,unsigned char *key,int keylen){
	int buflen=0;
	unsigned char *buf=decrypt_recv(sock,key,keylen,&buflen);
	int num;
	memcpy(&num,buf,8);
	return num;
}

void aesencrypt(unsigned char *text,int textlen,unsigned char *key,int keylen,unsigned char *en,int enlen){
	AES_KEY aes;
	unsigned char iv[AES_BLOCK_SIZE];
	memset(iv,0,AES_BLOCK_SIZE);
	if(AES_set_encrypt_key(key,keylen*8,&aes)<0){
		cout<<"cannot set encryption key in AES"<<endl;
		exit(1);
	}
	AES_cbc_encrypt(text,en,textlen,&aes,iv,AES_ENCRYPT);
	// cout<<"in cryption:"<<endl;
	// hexprint(en,enlen);
}
void aesdecrypt(unsigned char *en,int len,unsigned char *key,int keylen,unsigned char *de,int delen){
	// cout<<"en:"<<endl;
	// hexprint(en,len);
	// cout<<"len:"<<len<<endl;
	// cout<<"key:"<<endl;
	// hexprint(key,keylen);
	// cout<<"keylen:"<<keylen<<endl;
	// cout<<"AES_BLOCK_SIZE:"<<AES_BLOCK_SIZE<<endl;
	
	unsigned char iv[AES_BLOCK_SIZE]={0};
	AES_KEY aes;
	if(AES_set_decrypt_key(key,keylen*8,&aes)<0){
		cout<<"cannot set decryption key in AES"<<endl;
		exit(1);
	}
	AES_cbc_encrypt(en,de,len,&aes,iv,AES_DECRYPT);
	//cout<<"in decryption:"<<endl<<de<<endl;
}
int getpaddedsize(int size){
	return ((size + AES_BLOCK_SIZE) / AES_BLOCK_SIZE) * AES_BLOCK_SIZE;
}
unsigned char *pwdhash(unsigned char* text,int textlen){
	unsigned char *hash;
	unsigned char *printable_hash;
	hash=(unsigned char*)malloc(SHA256_DIGEST_LENGTH);
	printable_hash=(unsigned char*)malloc(SHA256_DIGEST_LENGTH*2+1);
	memset(printable_hash,0,SHA256_DIGEST_LENGTH*2+1);
	SHA256(text, textlen, hash);
	int i=0;
    for(i = 0; i < SHA256_DIGEST_LENGTH; i++){
        sprintf((char *)(printable_hash+i*2), "%02x", (unsigned int)hash[i]);\
    }
    free(hash);
    return printable_hash;
}


bool check_account(unsigned char *user,int userlen,unsigned char *pwd,int pwdlen){
	int fsize=getfilesize(CONFIGFILE);
	if(fsize==0)
		return false;
	FILE *f=fopen(CONFIGFILE,"rb");
	char *fuser;
	char *fpwd;
	int readsize=0;
	char c=0;

	while(readsize<(fsize-1)){
		fseek(f,readsize,SEEK_SET);
		int pos=readsize;
		int fuserlen=0;
		int fpwdlen=0;
		while(pos<fsize){
			
			char c=fgetc(f);
			pos++;
			if(c==' ')
				break;
		}
		fuser=(char*)malloc(pos-readsize-1);
		fseek(f,readsize,SEEK_SET);
		fread(fuser,1,pos-readsize-1,f);
		fuserlen=pos-readsize-1;
		readsize=pos;
		fseek(f,readsize,SEEK_SET);

		while(pos<fsize){
			char c=fgetc(f);

			pos++;
			//cout<<pos<<" "<<c<<" "<<(int)c<<endl;
			if(c=='\n')
				break;
		}
		fpwd=(char*)malloc(pos-readsize-1);
		fseek(f,readsize,SEEK_SET);
		fread(fpwd,1,pos-readsize-1,f);
		fpwdlen=pos-readsize-1;
		if(fuserlen==userlen&&fpwdlen==pwdlen){
			if(memcmp(fuser,user,userlen)==0&&memcmp(fpwd,pwd,pwdlen)==0){
				free(fuser);
				free(fpwd);
				fclose(f);
				return true;
			}
		}
		free(fuser);
		free(fpwd);
		readsize=pos;
	}
	fclose(f);
	return false;

}
void saveaccount(unsigned char *user,unsigned char *pwd){
	unsigned char *h=pwdhash(pwd,strlen((char*)pwd));
	FILE *f=fopen(CONFIGFILE,"a+");
	fseek(f,0,SEEK_END);
	fwrite((char *)user,1,strlen((char*)user),f);
	fwrite(" ",1,1,f);
	fwrite((char*)h,1,strlen((char*)h),f);
	fwrite("\n",1,1,f);
	fclose(f);
}