#include <netinet/in.h>    // for sockaddr_in
#include <sys/types.h>    // for socket
#include <sys/socket.h>    // for socket
#include <stdio.h>        // for printf
#include <stdlib.h>        // for exit
#include <string.h>        // for bzero
#include <time.h>                //for time_t and time
#include <arpa/inet.h>
#include <iostream>
#include "Util.h"
#include <getopt.h>

using namespace std;
#define GET	 1
#define POST 0
class Client{
public:
	int server_port;
	int client_port;
	char *server_addr;
	char *client_addr;
	unsigned char *user;
	unsigned char *pwd;
	unsigned char *from;
	unsigned char *to;
	int mode;
	unsigned char *key;
	int sock;
	void network_init(char *s_addr,int s_p,char *c_addr,int c_p);
	Client(int argc, char** argv);
	~Client();

};