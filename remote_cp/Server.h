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
#define CONFIGFILE "./config"
#define GET	 1
#define POST 0
class Server{
public:
	int server_port;
	char *server_addr;
	int server_socket;
	unsigned char *key;
	Server(char *s_addr,int s_p);
	int get_client_connection();
	int mode;
	unsigned char *from;
	unsigned char *to;
	~Server();
};