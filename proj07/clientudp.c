//clientudp

#include "clienttcp.h"

//VARIAVEL GLOBAL DO JOIN
extern int joinflag;
extern int showanel;


char * getip(char * smd){
	
	int fd;
    struct ifreq ifr;
    char iface[] = "eth0";
     
    fd = socket(AF_INET, SOCK_DGRAM, 0);
 
    //Type of address to retrieve - IPv4 IP address
    ifr.ifr_addr.sa_family = AF_INET;
 
    //Copy the interface name in the ifreq structure
    strncpy(ifr.ifr_name , iface , IFNAMSIZ-1);
 
    ioctl(fd, SIOCGIFADDR, &ifr);
 
    close(fd);
 
    //display result

	strcpy(smd, inet_ntoa(( (struct sockaddr_in*)&ifr.ifr_addr )->sin_addr) ); 
	
	return smd;
}



void join(int fd, struct sockaddr_in addr, char * input, no meuno, udpserv udps){
	
	char msg[128];
	char buffer[128];
	int var1, noarq, tcparq, n;
	int succid;
	int succtcp;
	char charsuccid[128];
	char charsucctcp[128];
	char succip[128];
	char anel[128];
	char noid[128];
	char join[128];
	char sent[128];
	char iparq[128];
	char brsp[128];
	char tcpporto[128];
	socklen_t addrlen;
	struct in_addr ipnoarq;



	//JOIN X I SUCC SUCC.IP SUCC.TCP
	if(sscanf(input,"%s %d %d %d %s %d",join,&var1,&meuno.id,&succid,succip,&succtcp)==6){
		sprintf(charsuccid,"%d",succid);
		sprintf(charsucctcp,"%d",succtcp);
		bzero(buffer,128);
		strcpy(buffer, "SUCC ");
		strcat(buffer, charsuccid);
		strcat(buffer, " ");
		strcat(buffer, succip);
		strcat(buffer, " ");
		strcat(buffer, charsucctcp);
		
		servidortcp(meuno, buffer, 0, udps);
	
	}
	
	//JOIN X I
	else if(sscanf(input,"%s %d %d",join,&var1,&meuno.id)==3){
		sprintf(anel, "%d", var1);
		sprintf(noid, "%d", meuno.id);
		sprintf(tcpporto,"%d", meuno.tcpporto);
		bzero(sent,128);
		strcpy(sent,"BQRY ");
		strcat(sent, anel);
		
		showanel=var1;
			
		n=sendto(fd,sent,strlen(sent),0,(struct sockaddr*)&addr,sizeof(addr));
		if(n==-1){printf("error: %s\n",strerror(errno)); exit(1);}
		
		printf("%s\n",sent);
		
		addrlen=sizeof(addr);
		n=recvfrom(fd,msg,128,0,(struct sockaddr*)&addr,&addrlen);
		if(n==-1){printf("error: %s\n",strerror(errno)); exit(1);}
		msg[n]='\0';
		
		printf("%s\n",msg);
		
		if(strncmp(msg,"EMPTY",5)==0){							//ainda nao existe anel

			bzero(sent,128);		
			strcpy(sent, "REG ");
			strcat(sent, anel);
			strcat(sent, " ");
			strcat(sent, noid);
			strcat(sent, " ");
			strcat(sent, getip(buffer));
			strcat(sent, " ");
			strcat(sent, tcpporto);
			
			inet_aton(buffer,&meuno.ip);
			
			n=sendto(fd,sent,strlen(sent),0,(struct sockaddr*)&addr,sizeof(addr));
			if(n==-1){printf("error: %s\n",strerror(errno)); exit(1);}
			printf("%s\n", sent);
			n=recvfrom(fd,msg,128,0,(struct sockaddr*)&addr,&addrlen);
			if(n==-1){printf("error: %s\n",strerror(errno)); exit(1);}
			msg[n]='\0';
			close(fd);
			if(strcmp(msg,"OK")==0){printf("Registado\n"); servidortcp(meuno,msg,1,udps);}
			else printf("Não Registado\n");
			
		}else if(strncmp(msg,"BRSP",4)==0){							//SE JA EXISTE ANEL
			
			sscanf(msg,"%s %d %d %s %d",brsp, &var1, &noarq, iparq, &tcparq);
			inet_aton(getip(buffer),&meuno.ip);
			inet_aton(iparq,&ipnoarq);
			close(fd);
			if((noarq-meuno.id)==0){printf("Nó existente\n"); return;}	//no pedido é no de arranque
			else pesquisaID(ipnoarq,tcparq,meuno,udps);
			
		}

	}
	
	else{printf("Join errado\n"); exit(1);}
}
	
	

void envio(struct in_addr ip, int porto, no meuno){

	int fd;
	struct sockaddr_in addr;
	udpserv udps;
	char input[128];

	while(1){
		
		fd=socket(AF_INET, SOCK_DGRAM, 0);//UDP socket
		if(fd==-1){printf("error: %s\n",strerror(errno)); exit(1);}
		
		memset((void*)&addr,(int)'\0',sizeof(addr));
		addr.sin_family=AF_INET;
		addr.sin_addr=ip;
		addr.sin_port=htons(porto);
		
		udps.ipudp=ip;
		udps.udpport=porto;
		
		fgets(input,128,stdin);
	
	//--------------JOIN--------------
			
		if(strncmp(input,"join",4)==0){joinflag=1; join(fd, addr, input, meuno, udps);}
			
		
	//---------------LEAVE------------
		
		else if(strncmp(input,"leave",5)==0 && joinflag==0) printf("Ainda não foi feito nenhum join\n");		
			
	
	//------------SHOW----------------
		
		else if(strncmp(input,"show",4)==0 && joinflag==0)	printf("Ainda não foi feito nenhum join\n");
		
	//------------SEARCH----------------
	
		else if(strncmp(input,"search",6)==0 && joinflag==0)	printf("Ainda não foi feito nenhum join\n");
		
	//------------EXIT----------------
		
		else if(strncmp(input,"exit",4)==0){printf("O programa irá agora sair por comando do utilizador\n"); exit(1);}
		
		else printf("Mensagem Inválida\n");
	}
	
	return;
	
}

