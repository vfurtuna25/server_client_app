#include "cl_ipv6.h"

#define backlog 2
#define Port "22111"

//de inclus comentarii in timp ce scriem. sunt obligatorii


void main(){
	FILE *outfile;
	char buf[MAXSIZE];
	int bytes_recv;
	struct sockaddr_storage their_addr;
	socklen_t addr_size=sizeof their_addr;	
	int s,b,new_fd;
	struct addrinfo hints, *res,*p;	
	memset(&hints,0,sizeof hints);	
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;
	
	//setam modul in care se va face comunicarea cu server-ul si pe care port vom primi cereri
	if(getaddrinfo(NULL,Port, &hints, &res )==-1)
	{
		perror("server:info");
		exit(1);
	}


	//creare socket
	s = sockf( res->ai_family,res->ai_socktype, 0);
	if(s<0)
	{
		exit(1);
	}

	//setam ca adresa sa fie reutilizata
	int optval=1;
	setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof optval);
		
	//asociere socket cu un port 
	b=bind(s,res->ai_addr,res->ai_addrlen);
	if(b==-1)
	{
		perror("server:bind");
		close(s);
		exit(1);
	}
		
	//eliberam memoria
	freeaddrinfo(res);

	//asteptare conexiuni de pe port
	int l=listen(s,backlog);

	if(l==-1)
	{
		perror("listen:");
		exit(1);	
	}
	

	//bucla de tratare a clientilor
	while(1)
	{

		new_fd = accept(s,(struct sockaddr *)&their_addr,&addr_size);
		if(new_fd==-1){
			perror("accept");
			continue;
		}

		//deschidem fisierul pentru a putea scrie in el
		outfile = fopen("site.html","w+");
		if(outfile == NULL)
		{
			fprintf(stderr, "\nError opened file\n");
		}	

		bytes_recv=-2;
	
		//cream cate un proces pentru fiecare client conectat
		if (!fork())
		{
			//close(s);
			while(bytes_recv!=0)
          	{
				//se asteapta ca clientul sa trimita o comanda
            	bytes_recv=recv(new_fd,buf,MAXSIZE-1,0);
                if(bytes_recv==-1)
                {
                   perror("receive:server");
                   break;
                }
                
				printf("%s",buf);
		  		
				if(strstr(buf,"06#")!=0)
		 			client(outfile,new_fd);
				else
					if(send(new_fd,"Comanda nemplementata",strlen("Comanda nemplementata"),0)==-1)
					{
						perror("send:client");
						break;
					}

                memset(buf,0,sizeof(buf));
          	}
		}	
		/*
		In momentul in care se conecteaza un client, se cloneaza procesul si se va astepta
		o noua conexiune.
		*/
		fclose(outfile);
		close(new_fd);
	}
}
