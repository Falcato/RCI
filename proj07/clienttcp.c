//clienttcp

#include "clienttcp.h"

#define max(A,B) ((A)>=(B)?(A):(B))

extern int errno;
extern int joinflag;
extern int showanel;

int dist(int a, int b){
	int dist=-1;
	
	if(b>=a) dist=b-a;
	if(b<a) dist=pow(2,6)+b-a;
	
	if(dist==-1) {printf("Erro no cálculo da distância\n"); exit(0);}
	else return dist;
}

int qry(int id, int idpredi, int meuid){
	
	if(dist(id,meuid)<=dist(id,idpredi)) return 1;
	else return 0;
}

void pesquisaID(struct in_addr iparq, int portonoarq, no meuno, udpserv udps){

	struct sockaddr_in addr;
	int idfd, n;
	int noid, notcp;
	char buffer[128];
	char resposta[128];
	char charmeuno[128];
	char temp[128];
	char noip[128];
	
	int nleft, nwritten;
	char* ptr;


	idfd=socket(AF_INET,SOCK_STREAM,0);
	if(idfd==-1){printf("error: %s\n",strerror(errno)); exit(1);}
	
	memset((void*)&addr,(int)'\0',sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr=iparq;
	addr.sin_port=htons(portonoarq);
	
	n=connect(idfd,(struct sockaddr*)&addr,sizeof(addr));
	if(n==-1){printf("error: %s\n",strerror(errno)); exit(1);}
	sprintf(charmeuno, "%d", meuno.id);
	bzero(buffer,128);
	strcpy(buffer, "ID ");
	strcat(buffer, charmeuno);
	strcat(buffer, "\n");
	
	printf("Enviei: %s", buffer);
	
	ptr=buffer;
	nleft=strlen(buffer);
	while(nleft>0){nwritten=write(idfd,ptr,nleft);
	if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
	nleft-=nwritten;
	ptr+=nwritten;}

	bzero(resposta,128);
	n=read(idfd,resposta,128);
	if(n<=0){printf("error: %s\n",strerror(errno)); exit(1);}
	resposta[n]='\0';
	
	sscanf(resposta,"%s %d %s %d", temp, &noid, noip, &notcp);
	
	printf("Recebi do nó de arranque: %s",resposta);
	
	//CASO O SUCC TENHA O MESMO ID O UTILIZADOR TEM DE ESCOLHER OUTRO
	if(noid==(meuno.id)){printf("Nó existente\n"); return;}

	close(idfd);
	
	servidortcp(meuno, resposta, 0, udps);

}


void servidortcp(no meuno, char* nosucc, int flagarq, udpserv udps){
	
	
	int fd, newfd, confd, udpsocket;
	int prediflag=0;
	int succflag=0;
	int predi;
	int succ;
	socklen_t addrlen;
	int maxfd=0, inicializacao=0, anelvazio=1, predeqsucc=0;
	int n, intmeuno, qryid, noid, notcp;
	struct in_addr noip;
	struct sockaddr_in addr;
	struct sockaddr_in addrudp;
	char buffer[128];
	char resposta[128];
	char charnoid[128];	
	char charnoip[128];
	char charnotcp[128];
	char charqryid[128];	
	char charmeuno[128];
	char meuip[128];
	char meutcpporto[128];	
	no predino;
	no succno;	
	char chartemp[128];
	char charsuccip[128];
	int searchflag=0;
	
	int nleft, nwritten;
	char* ptr;
	
	predino.id=meuno.id;
	succno.id=meuno.id;
	
	if((fd=socket(AF_INET,SOCK_STREAM,0))==-1){printf("error: %s\n",strerror(errno)); exit(1);}
	if((predi=socket(AF_INET,SOCK_STREAM,0))==-1){printf("error: %s\n",strerror(errno)); exit(1);}
	if((succ=socket(AF_INET,SOCK_STREAM,0))==-1){printf("error: %s\n",strerror(errno)); exit(1);}
	 
	memset ((void*)&addr,(int)'\0',sizeof(addr));
	addr.sin_family=AF_INET;
	addr.sin_addr.s_addr=htons(INADDR_ANY);
	addr.sin_port=htons(meuno.tcpporto);
	
	if(bind(fd,(struct sockaddr*)&addr,sizeof(addr))==-1){printf("error: %s\n",strerror(errno)); exit(1);}
	if(listen(fd,5)==-1){printf("error: %s\n",strerror(errno)); exit(1);}
	
	fd_set rfds;	
	
	while(1){
		
		//LIGA SE AO SUCC MAL INICIA O SERVIDOR (EXCEPTO SE FOR NO ARRANQUE)
		if(flagarq==0 && inicializacao==0){
			
			sscanf(nosucc, "%s %d %s %d", chartemp, &(succno.id), charsuccip, &(succno.tcpporto));
			inet_aton(charsuccip,&succno.ip);
			
			if(succno.id==meuno.id){printf("Nó existente\n"); return;}
			
			memset((void*)&addr,(int)'\0',sizeof(addr));
			addr.sin_family=AF_INET;
			addr.sin_addr=succno.ip;
			addr.sin_port=htons(succno.tcpporto);

			//LIGOU SE AO SUCC E ENVIOU NEW
			//SUCC = PREDI IRÁ ALTERAR QUANDO RECEBER O NEW
			n=connect(succ,(struct sockaddr*)&addr,sizeof(addr));
			if(n==-1){printf("error: %s\n",strerror(errno)); exit(1);}
			predi=succ;
			succflag=1;
			prediflag=1;

			predino.id=succno.id;
			predino.ip=succno.ip;
			predino.tcpporto=succno.tcpporto;
			
			
			strcpy(meuip,inet_ntoa(meuno.ip));
			sprintf(meutcpporto, "%d", meuno.tcpporto);
			sprintf(charmeuno, "%d", meuno.id);
			bzero(buffer, 128);
			strcpy(buffer,"NEW ");
			strcat(buffer, charmeuno);
			strcat(buffer, " ");
			strcat(buffer, meuip);
			strcat(buffer, " ");
			strcat(buffer, meutcpporto);
			strcat(buffer, "\n");
			
			printf("Enviei: %s", buffer);
			
			ptr=buffer;
			nleft=strlen(buffer);
			while(nleft>0){nwritten=write(succ,ptr,nleft);
			if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
			nleft-=nwritten;
			ptr+=nwritten;}
	
			inicializacao=1;
			anelvazio=0;
		}
		
		if(predino.id==(meuno.id)) anelvazio=1;
		
		FD_ZERO(&rfds);
		FD_SET(0,&rfds);
		FD_SET(fd,&rfds);
		maxfd=max(maxfd,fd);
		if(prediflag==1){FD_SET(predi,&rfds); maxfd=max(maxfd,predi);}
		if(succflag==1){FD_SET(succ,&rfds);	maxfd=max(maxfd,succ);}
		
		//SELECT
		if(select((maxfd+1),&rfds,(fd_set*)NULL,(fd_set*)NULL,(struct timeval*)NULL)==-1){printf("error: %s\n",strerror(errno)); exit(1);}
		
		//LISTEN ESTA PRONTO PARA LEITURA
		if (FD_ISSET(fd, &rfds)){

			addrlen=sizeof(addr);
		
			//ACEITA A NOVA LIGAÇAO
			newfd=accept(fd,(struct sockaddr*)&addr,&addrlen);
			if(newfd==-1){printf("error: %s\n",strerror(errno)); exit(1);}
		
			bzero(buffer,128);
			n=read(newfd,buffer,128);
			if(n<=0){printf("error: %s\n",strerror(errno)); exit(1);}
			printf("Recebi do listen: %s",buffer);
		
			//CASO SEJA NO ARRANQUE E ANEL ESTEJA VAZIO
			if(flagarq==1 && anelvazio==1){
				
				//RECEBE ID X
				if(strncmp(buffer,"ID ",3)==0){
							
					sprintf(charmeuno,"%d",meuno.id);
					strcpy(meuip,inet_ntoa(meuno.ip));
					sprintf(meutcpporto,"%d",meuno.tcpporto);
					bzero(resposta,128);
					strcpy(resposta,"SUCC ");
					strcat(resposta, charmeuno);
					strcat(resposta, " ");
					strcat(resposta, meuip);
					strcat(resposta, " ");
					strcat(resposta, meutcpporto);
					strcat(resposta, "\n");
					
					printf("Enviei: %s", resposta);
					
					ptr=resposta;
					nleft=strlen(resposta);
					while(nleft>0){nwritten=write(newfd,ptr,nleft);
					if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
					nleft-=nwritten;
					ptr+=nwritten;}
				}
			}
			//CASO SEJA NO ARRANQUE E ANEL NAO ESTEJA VAZIO
			if(flagarq==1 && anelvazio==0){
				
				//RECEBE ID X
				if(strncmp(buffer,"ID ",3)==0){
						
					//QRY
					sscanf(buffer,"%s %d", chartemp, &noid);
					
					//CASO O NO DE ARRANQUE SEJA RESPONSAVEL
					if(qry(noid, predino.id, meuno.id)==1){		
						
						//ENVIA O PROPRIO NO
						sprintf(charmeuno,"%d",meuno.id);
						strcpy(meuip,inet_ntoa(meuno.ip));
						sprintf(meutcpporto,"%d",meuno.tcpporto);
						bzero(resposta,128);
						strcpy(resposta,"SUCC ");
						strcat(resposta, charmeuno);
						strcat(resposta, " ");
						strcat(resposta, meuip);
						strcat(resposta, " ");
						strcat(resposta, meutcpporto);
						strcat(resposta, "\n");
						
						printf("Enviei: %s", resposta);
						
						ptr=resposta;
						nleft=strlen(resposta);
						while(nleft>0){nwritten=write(newfd,ptr,nleft);
						if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
						nleft-=nwritten;
						ptr+=nwritten;}

					//MANDA QRY PARA O SEU SUCC					
					}else{
						
						sprintf(charmeuno,"%d",meuno.id);
						sprintf(charnoid,"%d",noid);
						bzero(resposta,128);
						strcpy(resposta,"QRY ");
						strcat(resposta, charmeuno);
						strcat(resposta, " ");
						strcat(resposta, charnoid);
						strcat(resposta, "\n");
						
						printf("Enviei: %s", resposta);
						
						ptr=resposta;
						nleft=strlen(resposta);
						while(nleft>0){nwritten=write(succ,ptr,nleft);
						if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
						nleft-=nwritten;
						ptr+=nwritten;}			
					}
				}
			}
			
			//RECEBE NEW DO SEU NOVO PREDI.
			if(strncmp(buffer,"NEW ",4)==0){
				
				sscanf(buffer,"%s %d %s %d",chartemp,&noid,charnoip,&notcp);
				//ACTUALIZA ESTRUTURA DO PREDI
				predino.id=noid;
				inet_aton(charnoip,&predino.ip);
				predino.tcpporto=notcp;

				if(anelvazio==1) 
				{
					succno.id=noid;
					inet_aton(charnoip,&succno.ip);
					succno.tcpporto=notcp;
				}
				
				
				//CASO EXISTA ANTIGO PREDI
				//MANDA CON PARA RENOVAR LIGAÇOES.
				if(prediflag==1 && anelvazio==0){
					
					sprintf(charnoid, "%d", noid);
					sprintf(charnotcp, "%d", notcp);
					bzero(resposta,128);
					strcpy(resposta, "CON ");
					strcat(resposta, charnoid);
					strcat(resposta, " ");
					strcat(resposta, charnoip);
					strcat(resposta, " ");
					strcat(resposta, charnotcp);
					strcat(resposta, "\n");
					
					printf("Enviei: %s", resposta);
								
					ptr=resposta;
					nleft=strlen(resposta);
					while(nleft>0){nwritten=write(predi,ptr,nleft);
					if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
					nleft-=nwritten;
					ptr+=nwritten;}

					//TERMINA LIGAÇAO COM ANTIGO PREDI
					if(predi!=succ)	close(predi);
					
					//ACTUALIZA PARA NOVO PREDI
					predi=newfd;

				//CASO NAO EXISTA ANTIGO PREDI (ANEL VAZIO) O NOVO PREDI
				//SE FOR NO ARRANQUE TEM DE CRIAR SUCC					
				}else if(anelvazio==1 && flagarq==1){
					
					if(predi!=succ) close(predi);
					
					//CRIA PREDI E SUCC
					succ=newfd;
					succflag=1;					
					predi=newfd;
					prediflag=1;
					anelvazio=0;
				
				//SE NAO FOR NO ARRANQUE JA TEM O SUCC CORRECTO
				//PRECISA SO DE ACTUALIZAR PREDI
				}else if(anelvazio==1 && flagarq==0){
					
					if(predi!=succ)close(predi);
					
					//CRIA PREDI
					predi=newfd;
					prediflag=1;
					anelvazio=0;
				
				//CASO UM NO TENHA DADO LEAVE	
				}else if(prediflag==0 && anelvazio==0){
					//CRIA PREDI
					predi=newfd;
					prediflag=1;
				}
			}
		}
		
		//PREDI ESTA PRONTO PARA LEITURA
		if (prediflag==1 && FD_ISSET(predi, &rfds)){
			//SE RECEBEU ALGUMA COISA DO SEU PREDI SIGNIFICA QUE JA ESTA LIGADO A ALGUEM
			anelvazio=0;
			bzero(buffer,128);
			n=read(predi,buffer,128);
			if(n<=0){
				predino.id=meuno.id;
				prediflag=0;
				if(predi==succ)	anelvazio=1;
				printf("O predi saiu forçosamente\n");
				}
			buffer[n]='\0';
			printf("Recebi do predi: %s",buffer); 
			
			//RECEBE QRY DO PREDI
			if(strncmp(buffer, "QRY ", 4)==0){
				
				sscanf(buffer,"%s %d %d", chartemp, &noid, &qryid);
				
				//CASO SEJA RESPONSAVEL
				if(qry(qryid, predino.id, meuno.id)==1){
											
					sprintf(charnoid, "%d", noid);
					sprintf(charqryid, "%d", qryid);
					strcpy(meuip,inet_ntoa(meuno.ip));
					sprintf(meutcpporto, "%d", meuno.tcpporto);
					sprintf(charmeuno, "%d", meuno.id);
					bzero(resposta,128);
					strcpy(resposta, "RSP ");
					strcat(resposta, charnoid);
					strcat(resposta, " ");
					strcat(resposta, charqryid);
					strcat(resposta, " ");
					strcat(resposta, charmeuno);
					strcat(resposta, " ");
					strcat(resposta, meuip);
					strcat(resposta, " ");
					strcat(resposta, meutcpporto);
					strcat(resposta, "\n");
					
					printf("Enviei: %s", resposta);
					
					//ENVIO (RSP ID ARQ MEUNO MEUNO.IP MEUNO.TCP) PARA O MEU PREDI
					ptr=resposta;
					nleft=strlen(resposta);
					while(nleft>0){nwritten=write(predi,ptr,nleft);
					if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
					nleft-=nwritten;
					ptr+=nwritten;}
				
				}else{
					
					printf("Enviei: %s", buffer);
				
					//ENVIO O MESMO QRY PARA O MEU SUCC
					ptr=buffer;
					nleft=strlen(buffer);
					while(nleft>0){nwritten=write(succ,ptr,nleft);
					if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
					nleft-=nwritten;
					ptr+=nwritten;}
				}
			}//RECEBE BOOT DO NO ARRANQUE
			else if(strncmp(buffer,"BOOT",4)==0){
				//PASSO A SER NO ARRANQUE
				flagarq=1;
				printf("Sou nó de arranque\n");

				udpsocket=socket(AF_INET, SOCK_DGRAM, 0);//UDP socket
				if(udpsocket==-1){printf("error: %s\n",strerror(errno)); exit(1);}
				
				memset((void*)&addrudp,(int)'\0',sizeof(addrudp));
				addrudp.sin_family=AF_INET;
				addrudp.sin_addr=udps.ipudp;
				addrudp.sin_port=htons(udps.udpport);


				sprintf(charmeuno, "%d", meuno.id);
				sprintf(meutcpporto, "%d", meuno.tcpporto);
				strcpy(meuip,inet_ntoa(meuno.ip));
				sprintf(chartemp, "%d", showanel);
		
				bzero(resposta,128);		
				strcpy(resposta, "REG ");
				strcat(resposta, chartemp);
				strcat(resposta, " ");
				strcat(resposta, charmeuno);
				strcat(resposta, " ");
				strcat(resposta, meuip);
				strcat(resposta, " ");
				strcat(resposta, meutcpporto);
				
				//ENVIO REG DO MEU NO PARA O SERVIDOR UDP
				printf("Enviei para o UDP %s\n", resposta);
				n=sendto(udpsocket,resposta,strlen(resposta),0,(struct sockaddr*)&addrudp,sizeof(addrudp));
				if(n==-1){printf("error: %s\n",strerror(errno)); exit(1);}
				close(udpsocket);				
			}
		
			//CASO PREDI=SUCC (ANEL COM 2 NOS) PODE ACONTECER
			//SER O PREDI A MANDAR RSP OU CON
			
			if(predi==succ){
				
				//PREDI=SUCC JÁ NAO É NECESSARIO LER O QUE
				//SUCC TEM PARA DIZER, VISTO QUE JA FOI LIDO AQUI
				predeqsucc=1;
				
				if(strncmp(buffer,"RSP ",4)==0){
					sscanf(buffer,"%s %d %d %d %s %d",chartemp,&intmeuno,&qryid,&noid,charnoip,&notcp);
					
					//CASO SEJA NO ARRANQUE TEM DE ENVIAR SUCC AO NOVO NO
					if(intmeuno==meuno.id){
						if (searchflag==0){
					
							//ENVIA NO DO RSP PARA O NOVO NO
							sprintf(charnoid,"%d",noid);
							sprintf(charnotcp,"%d",notcp);
							bzero(resposta,128);
							strcpy(resposta,"SUCC ");
							strcat(resposta, charnoid);
							strcat(resposta, " ");
							strcat(resposta, charnoip);
							strcat(resposta, " ");
							strcat(resposta, charnotcp);
							strcat(resposta, "\n");
						
							printf("Enviei: %s", resposta);
						
							//ENVIA SUCC NORSP NORSP.IP NORSP.TCP
							ptr=resposta;
							nleft=strlen(resposta);
							while(nleft>0){nwritten=write(newfd,ptr,nleft);
							if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
							nleft-=nwritten;
							ptr+=nwritten;}

						}else if (searchflag==1) {printf("%d %s %d\n", noid, charnoip, notcp); searchflag=0;}
					}
				}
				//ANTIGO PREDI RECEBE CON
				else if(strncmp(buffer,"CON ",4)==0){
				
					sscanf(buffer,"%s %d %s %d",chartemp,&noid,charnoip,&notcp);
					//ACTUALIZA ESTRUTURA DO SUCC
					succno.id=noid;
					inet_aton(charnoip,&(succno.ip));
					succno.tcpporto=notcp;
					
					inet_aton(charnoip,&noip);
					
					confd=socket(AF_INET,SOCK_STREAM,0);
					if(confd==-1){printf("error: %s\n",strerror(errno)); exit(1);}

					memset((void*)&addr,(int)'\0',sizeof(addr));
					addr.sin_family=AF_INET;
					addr.sin_addr=noip;
					addr.sin_port=htons(notcp);
					
					n=connect(confd,(struct sockaddr*)&addr,sizeof(addr));
					if(n==-1){printf("error: %s\n",strerror(errno)); exit(1);}
					
					//ENVIA NEW PARA O NOVO SUCC
					sprintf(charmeuno, "%d", meuno.id);
					sprintf(meutcpporto, "%d", meuno.tcpporto);
					strcpy(meuip,inet_ntoa(meuno.ip));
					bzero(resposta,128);
					strcpy(resposta, "NEW ");
					strcat(resposta, charmeuno);
					strcat(resposta, " ");
					strcat(resposta, meuip);
					strcat(resposta, " ");
					strcat(resposta, meutcpporto);
					strcat(resposta, "\n");
					
					printf("Enviei: %s", resposta);
					
					ptr=resposta;
					nleft=strlen(resposta);
					while(nleft>0){nwritten=write(confd,ptr,nleft);
					if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
					nleft-=nwritten;
					ptr+=nwritten;}
					
					succ=confd;
				}
			}
		}
		
		//SUCC ESTA PRONTO PARA LEITURA
		if (predeqsucc==0 && succflag==1 && FD_ISSET(succ, &rfds)){
			//SE RECEBEU ALGUMA COISA DO SEU SUCC SIGNIFICA QUE JA ESTA LIGADO A ALGUEM
			anelvazio=0;
			bzero(buffer,128);
			n=read(succ,buffer,128);
			if(n<=0){
				succno.id=meuno.id;
				succflag=0;
				if(predi==succ)	anelvazio=1;
				printf("O succ saiu forçosamente\n");
			}
			buffer[n]='\0';
			printf("Recebi do succ: %s",buffer);
			
			//RECEBE RSP DO QRY FEITO ANTERIORMENTE	
			if(strncmp(buffer,"RSP ",4)==0){
				sscanf(buffer,"%s %d %d %d %s %d",chartemp,&intmeuno,&qryid,&noid,charnoip,&notcp);
				
				//CASO SEJA NO ARRANQUE TEM DE ENVIAR SUCC AO NOVO NO
				if(intmeuno==meuno.id){
					//ALTERAR
					if (searchflag==0){
						//ENVIA NO DO RSP PARA O NOVO NO
						sprintf(charnoid,"%d",noid);
						sprintf(charnotcp,"%d",notcp);
						bzero(resposta,128);
						strcpy(resposta,"SUCC ");
						strcat(resposta, charnoid);
						strcat(resposta, " ");
						strcat(resposta, charnoip);
						strcat(resposta, " ");
						strcat(resposta, charnotcp);
						strcat(resposta, "\n");
						
						printf("Enviei: %s", resposta);
					
						//ENVIA SUCC NORSP NORSP.IP NORSP.TCP
						ptr=resposta;
						nleft=strlen(resposta);
						while(nleft>0){nwritten=write(newfd,ptr,nleft);
						if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
						nleft-=nwritten;
						ptr+=nwritten;}
				
					}else if (searchflag==1) {printf("%d %s %d\n", noid, charnoip, notcp); searchflag=0;}
				}else{
				
					printf("Enviei: %s", buffer);
				
					//ENVIO O MESMO RSP PARA O MEU PREDI
					ptr=buffer;
					nleft=strlen(buffer);
					while(nleft>0){nwritten=write(predi,ptr,nleft);
					if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
					nleft-=nwritten;
					ptr+=nwritten;}
				}
			}

			//ANTIGO PREDI RECEBE CON
			else if(strncmp(buffer,"CON ",4)==0){
				
				sscanf(buffer,"%s %d %s %d",chartemp,&noid,charnoip,&notcp);
				//ACTUALIZA ESTRUTURA DO SUCC
				succno.id=noid;
				inet_aton(charnoip,&(succno.ip));
				succno.tcpporto=notcp;
				
				inet_aton(charnoip,&noip);
				
				confd=socket(AF_INET,SOCK_STREAM,0);
				if(confd==-1){printf("error: %s\n",strerror(errno)); exit(1);}

				memset((void*)&addr,(int)'\0',sizeof(addr));
				addr.sin_family=AF_INET;
				addr.sin_addr=noip;
				addr.sin_port=htons(notcp);
				
				n=connect(confd,(struct sockaddr*)&addr,sizeof(addr));
				if(n==-1){printf("error: %s\n",strerror(errno)); exit(1);}
				
				//ENVIA NEW PARA O NOVO SUCC
				sprintf(charmeuno, "%d", meuno.id);
				sprintf(meutcpporto, "%d", meuno.tcpporto);
				strcpy(meuip,inet_ntoa(meuno.ip));
				bzero(resposta,128);
				strcpy(resposta, "NEW ");
				strcat(resposta, charmeuno);
				strcat(resposta, " ");
				strcat(resposta, meuip);
				strcat(resposta, " ");
				strcat(resposta, meutcpporto);
				strcat(resposta, "\n");
				
				printf("Enviei: %s", resposta);
				
				ptr=resposta;
				nleft=strlen(resposta);
				while(nleft>0){nwritten=write(confd,ptr,nleft);
				if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
				nleft-=nwritten;
				ptr+=nwritten;}
				
				//ACTUALIZA SUCC
				if(succ!=predi) close(succ);
				succ=confd;
			}else if(strncmp(buffer,"BOOT",4)==0){
					
				//PASSO A SER NO ARRANQUE
				flagarq=1;
				printf("Sou nó de arranque\n");

				udpsocket=socket(AF_INET, SOCK_DGRAM, 0);//UDP socket
				if(udpsocket==-1){printf("error: %s\n",strerror(errno)); exit(1);}
				
				memset((void*)&addrudp,(int)'\0',sizeof(addrudp));
				addrudp.sin_family=AF_INET;
				addrudp.sin_addr=udps.ipudp;
				addrudp.sin_port=htons(udps.udpport);


				sprintf(charmeuno, "%d", meuno.id);
				sprintf(meutcpporto, "%d", meuno.tcpporto);
				strcpy(meuip,inet_ntoa(meuno.ip));
				sprintf(chartemp, "%d", showanel);

				bzero(resposta,128);		
				strcpy(resposta, "REG ");
				strcat(resposta, chartemp);
				strcat(resposta, " ");
				strcat(resposta, charmeuno);
				strcat(resposta, " ");
				strcat(resposta, meuip);
				strcat(resposta, " ");
				strcat(resposta, meutcpporto);
				
				//ENVIO REG DO MEU NO PARA O SERVIDOR UDP
				printf("Enviei para o UDP %s\n", resposta);
				n=sendto(udpsocket,resposta,strlen(resposta),0,(struct sockaddr*)&addrudp,sizeof(addrudp));
				if(n==-1){printf("error: %s\n",strerror(errno)); exit(1);}	
				close(udpsocket);
				
				if(predino.id==succno.id){succflag=0; close(succ);}			
			}
								
		}
		//RESET À VARIAVEL, SO A QUEREMOS ALTERAR QUANDO ELA ENTRA
		//NAS FUNÇOES CON E RSP NO PREDI
		predeqsucc=0;
		
		//STDIN ESTA PRONTO PARA LEITURA
		if (FD_ISSET(0, &rfds)){
			fgets(buffer,128,stdin);
			
			//LEAVE
			if(strncmp(buffer,"leave",5)==0){
				
				//SE FOR NO ARRANQUE E ANEL ESTIVER VAZIO
				if(flagarq==1 && anelvazio==1){
					
					udpsocket=socket(AF_INET, SOCK_DGRAM, 0);//UDP socket
					if(udpsocket==-1){printf("error: %s\n",strerror(errno)); exit(1);}
					memset((void*)&addrudp,(int)'\0',sizeof(addrudp));
					addrudp.sin_family=AF_INET;
					addrudp.sin_addr=udps.ipudp;
					addrudp.sin_port=htons(udps.udpport);
					
					//ENVIA PARA O SERVIDOR UDP UNR ANEL
					printf("Leave anel\n");
					sprintf(chartemp, "%d", showanel);
					bzero(resposta,128);
					strcpy(resposta, "UNR ");
					strcat(resposta, chartemp);
					
					n=sendto(udpsocket,resposta,strlen(resposta),0,(struct sockaddr*)&addrudp,sizeof(addrudp));
					if(n==-1){printf("error: %s\n",strerror(errno)); exit(1);}
					close(udpsocket);
					close(newfd);
					close(fd);
					joinflag=0;
					return;
				
				//SE FOR NO ARRANQUE E ANEL ESTIVER COM 2 NOS
				}else if(flagarq==1 && anelvazio==0 && (predi==succ || predino.id==succno.id)){
					
					printf("Leave anel\n");
					//ENVIA BOOT PARA O SUCC (NOVO NO ARRANQUE)
					bzero(resposta, 128);
					strcpy(resposta, "BOOT\n");
					
					printf("Enviei: %s", resposta);
					
					ptr=resposta;
					nleft=strlen(resposta);
					while(nleft>0){nwritten=write(succ,ptr,nleft);
					if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
					nleft-=nwritten;
					ptr+=nwritten;}
					
					close(succ);
					close(newfd);
					close(fd);
					joinflag=0;
					return;
				
				//SE FOR NO ARRANQUE E ANEL NAO ESTIVER VAZIO
				}else if(flagarq==1 && predi!=succ){
					
					printf("Leave anel\n");
					
					//ENVIA BOOT PARA O SUCC (NOVO NO ARRANQUE)
					bzero(resposta, 128);
					strcpy(resposta, "BOOT\n");
					
					printf("Enviei: %s", resposta);
					
					ptr=resposta;
					nleft=strlen(resposta);
					while(nleft>0){nwritten=write(succ,ptr,nleft);
					if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
					nleft-=nwritten;
					ptr+=nwritten;}
					
					//ESCREVE PARA PREDI CON AO SUCC
					sprintf(charnoid, "%d", succno.id);
					strcpy(charnoip,inet_ntoa(succno.ip));
					sprintf(charnotcp, "%d", succno.tcpporto);
					bzero(resposta,128);
					strcpy(resposta, "CON ");
					strcat(resposta, charnoid);
					strcat(resposta, " ");
					strcat(resposta, charnoip);
					strcat(resposta, " ");
					strcat(resposta, charnotcp);
					strcat(resposta, "\n");
					
					printf("Enviei: %s", resposta);
					
					ptr=resposta;
					nleft=strlen(resposta);
					while(nleft>0){nwritten=write(predi,ptr,nleft);
					if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
					nleft-=nwritten;
					ptr+=nwritten;}
					
					close(succ);
					close(predi);
					close(newfd);
					//close(confd);
					close(fd);
					joinflag=0;
					return;
					
				//SE NAO FOR NO ARRANQUE E SE O ANEL ESTIVER COM 2 NOS
				}else if(flagarq==0 && (predi==succ || predino.id==succno.id)){
					printf("Leave anel\n");
					
					close(predi);
					close(succ);
					close(newfd);
					close(fd);
					joinflag=0;
					return;
				
				//SE NAO FOR NO ARRANQUE E ANEL NAO ESTA VAZIO
				}else if(flagarq==0 && predi!=succ){
					
					printf("Leave anel\n");		
					//ESCREVE PARA PREDI CON AO SUCC
					sprintf(charnoid, "%d", succno.id);
					strcpy(charnoip,inet_ntoa(succno.ip));
					sprintf(charnotcp, "%d", succno.tcpporto);
					bzero(resposta,128);
					strcpy(resposta, "CON ");
					strcat(resposta, charnoid);
					strcat(resposta, " ");
					strcat(resposta, charnoip);
					strcat(resposta, " ");
					strcat(resposta, charnotcp);
					strcat(resposta, "\n"); 
					
					printf("Enviei: %s", resposta);
					
					ptr=resposta;
					nleft=strlen(resposta);
					while(nleft>0){nwritten=write(predi,ptr,nleft);
					if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
					nleft-=nwritten;
					ptr+=nwritten;}	
					
					close(succ);
					close(predi);
					close(newfd);
					//close(confd);
					close(fd);
					joinflag=0;
					return;
				}
			
			}else if(strncmp(buffer,"join",4)==0 && joinflag==1 ) printf("Já está encarregue de um nó\n");
			
			//SHOW
			else if((strcmp(buffer,"show\n")==0) && joinflag==1)
			{
				printf("ANEL: %d\n", showanel);
				printf("ID: %d\n", meuno.id);
				if( (meuno.id!=predino.id) ) {printf("Predi's ID: %d\n", predino.id);}
				if( (meuno.id!=succno.id) ) {printf("Successor's ID: %d\n", succno.id);}
			}//SEARCH K
			//Saber ID, ID.IP, ID.TCP do RESPONSAVEL por K
			else if(strncmp(buffer,"search",6)==0 && sscanf(buffer,"%s %d", chartemp, &qryid)>1){
				
				searchflag=1;
				
				//SE ELE PRÓPRIO É RESPONSÁVEL
				strcpy(meuip, inet_ntoa(meuno.ip));
				if(qry(qryid, predino.id, meuno.id)==1) {printf("%d %s %d\n", meuno.id, meuip, meuno.tcpporto); searchflag=0;}
				
				//SE N FOR RESPONSAVEL MANDA QRY i k AO SUC
				if(qry(qryid, predino.id, meuno.id)!=1) {

					sprintf(charmeuno,"%d",meuno.id);
					sprintf(charnoid,"%d",qryid);
					bzero(resposta,128);
					strcpy(resposta,"QRY ");
					strcat(resposta, charmeuno);
					strcat(resposta, " ");
					strcat(resposta, charnoid);
					strcat(resposta, "\n");
					
					printf("Enviei: %s", resposta);
					
					ptr=resposta;
					nleft=strlen(resposta);
					while(nleft>0){nwritten=write(succ,ptr,nleft);
					if(nwritten<=0){printf("error: %s\n",strerror(errno)); exit(1);}
					nleft-=nwritten;
					ptr+=nwritten;}	
				}
			}else if(strcmp(buffer,"exit\n")==0){printf("O programa irá agora sair por comando do utilizador\n"); exit(0);}
			
			else{ printf("Comando Inválido\n");}
		}
	}
}
