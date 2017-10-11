#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <stdio.h>        // for printf
#include <stdlib.h>        // for exit
#include <string.h>        // for bzero
#include <time.h>                //for time_t and time
#include <arpa/inet.h>
#include <iostream>
#include <sys/stat.h>  

#include <openssl/rsa.h>  
#include <openssl/err.h>  
#include <openssl/pem.h>
#include <openssl/dh.h>
#include <openssl/aes.h>
#include <openssl/sha.h>
#include <unistd.h>


using namespace std;
#define RECV_FILE_BUFF_LENGTH 4096
#define RECV_BUFF_LENGTH 	  4096
#define DH_KEY_LENGTH		  256
#define SERVER				  1
#define CLIENT				  2
#define KEY_LENGTH 			  16
#define CONFIGFILE "./config"

void recvfile(int sock,char *path,unsigned int filesize);
void sendfile(int sock,char *path,unsigned int filesize);
unsigned int getfilesize(char *path);

unsigned char *getsecretkey(int sock,int role);
void hexprint(unsigned char *printBuf, int len);
void *recvn(int sock,int size);
int recvint(int sock);
void sendint(int sock,int num);
void aesencrypt(unsigned char *text,int textlen,unsigned char *key,int keylen,unsigned char *en,int enlen);
void aesdecrypt(unsigned char *en,int len,unsigned char *key,int keylen,unsigned char *de,int delen);
int getpaddedsize(int size);
unsigned char* decrypt_recv(int sock,unsigned char *key,int keylen,int *len);
void encrypt_send(int sock,unsigned char *text,int textlen,unsigned char *key,int keylen);
unsigned char * pwdhash(unsigned char* text,int textlen);

void saveaccount(unsigned char *user,unsigned char *pwd);
bool check_account(unsigned char *user,int userlen,unsigned char *pwd,int pwdlen);
void encrypt_send_int(int sock,int num,unsigned char *key,int keylen);
int decrypt_recv_int(int sock,unsigned char *key,int keylen);