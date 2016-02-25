//DDT

#include "clientudp.h"
int joinflag;
int showanel;

int main(int argc, char *argv[])
{
	
	int k, i;
	int dotcount=0;
	struct hostent *address;
	struct in_addr *ipptr=NULL;
	int port, tcpport=58000;
	char cport[128], ctcpport[128];
	char buffteste[128];
	struct in_addr ip;
	no meuno;
	int argctcount=0;
	int argcpcount=0;
	int argcicount=0;
	
	address=gethostbyname("tejo.tecnico.ulisboa.pt");
	ipptr=(struct in_addr*)address->h_addr_list[0];
	ip=*ipptr;
	port=58000;
	
	void (*old_handler)(int); //interrupt handler
	if((old_handler=signal(SIGPIPE,SIG_IGN))==SIG_ERR) exit(1); //error
	
	if (argc!=3 && argc!=5 && argc!= 7) {printf("Invocar com ./ddt [-t ringport] ([-i bootip]) ([-p bootport])\n"); exit(1);}

	
	if(argc==3 && strcmp(argv[1],"-t")==0){		
		strcpy(ctcpport,argv[2]);
		for(i=0;i<strlen(ctcpport);i++){
			if(ctcpport[i]>'9' || ctcpport[i]<'0'){
				printf("TCP porto inválido\n");
				exit(1);
			}
		}
		tcpport=atoi(ctcpport);
	}
	
	else if(argc==5){
			
			for(k=1;k<4;k+=2){
				if(strcmp(argv[k],"-t")==0){
					argctcount+=1;
					strcpy(ctcpport,argv[k+1]);
					for(i=0;i<strlen(ctcpport);i++){
						if(ctcpport[i]>'9' || ctcpport[i]<'0'){
							printf("TCP porto inválido\n");
							exit(1);
						}
					}
					if (argctcount>1) {printf("Invocar com ./ddt [-t ringport] ([-i bootip]) ([-p bootport])\n"); exit(1);}
					tcpport=atoi(ctcpport);
				}
				else if(strcmp(argv[k],"-p")==0){
					argcpcount+=1;
					strcpy(cport,argv[k+1]);
					for(i=0;i<strlen(cport);i++){
						if(cport[i]>'9' || cport[i]<'0'){
							printf("Porto inválido\n");
							exit(1);
						}
					}
					if (argcpcount>1)  {printf("Invocar com ./ddt [-t ringport] ([-i bootip]) ([-p bootport])\n"); exit(1);}
					port=atoi(cport);
				}
				else if(strcmp(argv[k],"-i")==0){
					argcicount+=1;
					strcpy(buffteste,argv[k+1]);
					
					for(i=0;i<strlen(buffteste);i++){
						if(buffteste[0]=='.') {printf("IP inválido\n"); exit(1);}
						if((buffteste[i]=='.')&&(buffteste[i-1]=='.')) {printf("IP inválido\n"); exit(1);};
						if(buffteste[i]=='.') {dotcount+=1;}	
						//Se existirem caracteres que nao sejam pontos ou números	
						if((buffteste[i]>'9' || buffteste[i]<'0') && buffteste[i]!='.'){
							printf("IP inválido\n");
							exit(1);
						}
						//Se o último caracter for um ponto
						if((i==strlen(buffteste)-1)&&(buffteste[i]=='.')) { printf("IP inválido\n"); exit(1); }	
						//Se não existirem exactamente 3 pontos
						if((i==strlen(buffteste)-1)&&(dotcount!=3)) {printf("IP inválido\n"); exit(1);}
						if (argcicount>1) {printf("Invocar com ./ddt [-t ringport] ([-i bootip]) ([-p bootport])\n"); exit(1);}
					}
					inet_aton(argv[k+1],&ip);
				}else{
					printf("Invocar com ./ddt [-t ringport] ([-i bootip]) ([-p bootport])\n");
					exit(1);
				}
			}
			if((strcmp(argv[1],"-t")!=0)&&(strcmp(argv[3],"-t")!=0)){
					printf("Invocar com ./ddt [-t ringport] ([-i bootip]) ([-p bootport])\n"); exit(1);
			}
	}

	else if(argc==7){

		for(k=1;k<6;k+=2){
			if(strcmp(argv[k],"-t")==0){
				argctcount+=1;
				strcpy(ctcpport,argv[k+1]);
				for(i=0;i<strlen(ctcpport);i++){
					if(ctcpport[i]>'9' || ctcpport[i]<'0'){
						printf("TCP porto inválido\n");
						exit(1);
					}
				}
				if (argctcount>1) {printf("Invocar com ./ddt [-t ringport] ([-i bootip]) ([-p bootport])\n"); exit(1);}
				tcpport=atoi(ctcpport);
			}else if(strcmp(argv[k],"-p")==0){
				argcpcount+=1;
				strcpy(cport,argv[k+1]);
				for(i=0;i<strlen(cport);i++){
					if(cport[i]>'9' || cport[i]<'0'){
						printf("Porto inválido\n");
						exit(1);
					}
				}
				if (argcpcount>1) {printf("Invocar com ./ddt [-t ringport] ([-i bootip]) ([-p bootport])\n"); exit(1);}
				port=atoi(cport);
			}else if(strcmp(argv[k],"-i")==0){
				argcicount+=1;
				strcpy(buffteste,argv[k+1]);
				
				for(i=0;i<strlen(buffteste);i++){
					if(buffteste[0]=='.') {printf("IP inválido\n"); exit(1);}
					if((buffteste[i]=='.')&&(buffteste[i-1]=='.')) {printf("IP inválido\n"); exit(1);};
					if(buffteste[i]=='.') {dotcount+=1;}	
					//Se existirem caracteres que nao sejam pontos ou números	
					if((buffteste[i]>'9' || buffteste[i]<'0') && buffteste[i]!='.'){
						printf("IP inválido\n");
						exit(1);
					}
					//Se o último caracter for um ponto
					if((i==strlen(buffteste)-1)&&(buffteste[i]=='.')) { printf("IP inválido\n"); exit(1); }	
					//Se não existirem exactamente 3 pontos
					if((i==strlen(buffteste)-1)&&(dotcount!=3)) {printf("IP inválido\n"); exit(1);}
				}
				if (argcicount>1) {printf("Invocar com ./ddt [-t ringport] ([-i bootip]) ([-p bootport])\n"); exit(1);}
				inet_aton(argv[k+1],&ip);
			}else{
				printf("Invocar com ./ddt [-t ringport] ([-i bootip]) ([-p bootport])\n"); exit(1);
			}
		}
	}
	else{printf("Invocar com ./ddt [-t ringport] ([-i bootip]) ([-p bootport])\n"); exit(1);}
	
	printf("IP: %s PORTO %d TCP %d\n",inet_ntoa(ip),port,tcpport);
	meuno.tcpporto=tcpport;
	envio(ip, port, meuno);

	exit(0);
}
