#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netdb.h>
#include <string.h>


#define MAXSIZE 1024

int sockf(int domain, int type, int protocol)
{
	int s=socket(domain,type,protocol);
	if(s<0)	
		perror("socket");
	return s;
}

int connt(int sockfd, struct sockaddr *serv_addr, int addrlen)
{
	int c=connect(sockfd,serv_addr,addrlen);
	if(c<0)
		perror("client-server:connect");
	return c;
}



void client(FILE *outfile,int new_fd){

	char buf[MAXSIZE];
	int bytes_recv=-2;
	char *mssg="GET / HTTP/1.0\r\n\r\n";
	int bytessent, s,len;
	char ipstr[INET6_ADDRSTRLEN];
	struct addrinfo hints, *res,*p;	
	memset(&hints,0,sizeof hints);
	hints.ai_family=AF_INET6;
	hints.ai_socktype=SOCK_STREAM;	
	
	//precizam unde ne vom conecta si modul in care se face aceasta conexiune
	if(getaddrinfo("www.neverssl.com","http", &hints, &res )==-1)
	{
		perror("site:info");
		exit(1);
	}

	//if(res->ai_family==AF_INET)
	//	return 0;

	//cream file descriptor-ul pentru server-ul la care ne vom connecta
	s=sockf(res->ai_family,res->ai_socktype,0);
	if(s<0)
		exit(1);

	//se realizeaza conexiunea
	int c = connt(s, res->ai_addr, res->ai_addrlen);
	if (c<0)
		exit(1);
	
	//trimitem mesajul de request 
	len=strlen(mssg);
	if(send(s,mssg,len,0)==-1)
	{
		perror("HTTP:REQ");
		exit(1);
	}
	
	
	//se asteapta ca server-ul sa ne raspunda  la request
	while(bytes_recv!=0)
	{
		bytes_recv=recv(s,buf,MAXSIZE-1,0);
		if(bytes_recv==-1)
		{
			perror("receive:server");
			break;
		}

		//in momentul in care se primeste un pachet de la server, acesta va fi trimis inapoi clientului ipv4
		len=strlen(buf);
		if(bytessent=send(new_fd,buf,len,0)==-1)
		{
			perror("send:client");
			break;
		}

		//in acelasi timp se va scrie si in fisier datele primite de la server
		if(len!=0 && outfile)
		{
			fwrite(buf,strlen(buf),1,outfile);
			fflush(outfile);
		}
		memset(buf,0,sizeof(buf));

	}
	
	printf("\n\n");
	close(s);

}
