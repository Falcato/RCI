#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <signal.h>
#include <math.h>

typedef struct NO
{
	int id;
	struct in_addr ip;
	int tcpporto;
	
}no;

typedef struct UDPSERV
{
	struct in_addr ipudp;
	int udpport;
	
}udpserv;

char * getip(char * smd);
void join(int fd, struct sockaddr_in addr, char * input, no meuno, udpserv udps);
void envio(struct in_addr ip, int porto, no meuno);
