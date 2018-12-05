#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

#define MAX_RSS_SIZE	150000
#define SMALL_BUFFER_SIZE	100


int cutAddr(char *address, char *host, char *path, char *domain)
{
	const char c[2]="/";
	char *token;
	//on extrait l'hote
	token = strtok(address,c);	//http
	token = strtok(NULL,c); //""
	strcpy(host,token);
	//on extrait le chemin 
	strncpy(path,"",1);
	while(token != NULL)
	{
		token = strtok(NULL,c);
		if(token != NULL)
		{
			strncat(path,"/",1);
			strcat(path,token);
		}
	}


	//On extrait le domaine
	char w[4] = "www"; 
	char *tmp;
	tmp = strstr(host, w);
	if(tmp != 0)
	{
		printf("Domaine trouvé : %s\n",&domain[4]);
		strcpy(domain,(&tmp[4]));
	}
	else
	{
		strcpy(domain,host);
	}
	return 1;
}

int downloadRSS(char *address, char *fichier)
{
	char host[SMALL_BUFFER_SIZE],path[SMALL_BUFFER_SIZE],domain[SMALL_BUFFER_SIZE], buf[MAX_RSS_SIZE];
	cutAddr(address,host,path,domain);
	printf("\n%s %s %s\n",host,path,domain);
	int statut = 0;
	int sock = 0;
	struct addrinfo *addrs,*addr;



	struct sockaddr_in *ip;
	char *ips;
	
	statut=getaddrinfo(domain,NULL,NULL,&addrs);
	if(statut==EAI_NONAME) return -1;
	if(statut<0)
	{ 
		perror("downloadRSS.getaddrinfo");
		strcpy(fichier, "ERREUR : Nom de domaine introuvable.");
		return -1;
	}
	struct addrinfo *p;

	for(p=addrs,addr=addrs;p!=NULL;p=p->ai_next)
	{
		if(p->ai_family==PF_INET)
		{ 
			ip = (struct sockaddr_in *)(p->ai_addr);
			ips = inet_ntoa(((struct sockaddr_in*)(ip))->sin_addr);
			printf("IP : %s\n",ips);
			addr=p; break; 
		}
	}	
	//on se connecte au port 80
	ip->sin_port = htons(80);
	printf("Port : %d\n",ip->sin_port);
	//création de la socket
	sock = socket(PF_INET,SOCK_STREAM,0);
	struct timeval timeout;      
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;

    if (setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0)
    {
        perror("downloadRSS.setsockopt ");
       	strcpy(fichier, "ERREUR : Impossible de paramétrer correctement le socket.");
    	return -1;
    }

	if(sock<0)
	{
		perror("downloadRSS.socket ");
		strcpy(fichier, "ERREUR : Impossible de créer le socket.");
		return -1;
	}
	
	
	//On se connecte au serveur sur le port 80
	if( connect(sock, (struct sockaddr *)ip, sizeof(struct sockaddr)) < 0)
	{
		perror("downloadRSS.connect ");
		strcpy(fichier, "ERREUR : Connexion au site web impossible.");
		return -1;
	} 	


	printf("connecté\n");
	//On est connecté, du coup on lui envoi une requête GET
	char request[2000];
	sprintf(request,"GET %s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\nKeep-Alive: 300\r\n\r\n",path,host);
	printf("%s\n",request);
	write(sock,request,2000);

	char reponse[40];
	int n = 0;
	char http[50],code[50],status[50];

	while ((n = read(sock, reponse, sizeof(reponse)-1)) > 0)
	{
		reponse[n] = 0;
		strcat(buf,reponse);
	} 

	if(sscanf(buf,"%s %s %s",http,code,status)!=3) exit(-1);


	//on vérifie la bonne réception :
	if(strcmp(code,"200") == 0 && strcmp(status,"OK") == 0)
	{
		printf("Page reçue\n");
		char *deb = 0;
		deb = strstr(buf,"\r\n\r\n");
		if(deb != 0)
		{
			strcpy(fichier,deb+4);
			return 0;
		}
		else{
			strcpy(fichier, "ERREUR : Le fichier n'est pas conforme.");
			return -1;
		}
	}else {
		strcpy(fichier, "ERREUR : Le flux d'actualités n'a pas pu être trouvé, vérifiez l'adresse. NOTE : Il se peut que certains liens https ne fonctionnent pas.");
		return -1;
	}
	close(sock);
	return 1;
}


int main()
{
	//createSocket("futura-sciences.com",&adresse);
	//char host[100],path[100],domain[100];
	char address[500] = "http://feeds.bbci.co.uk/news/world/rss.xml";
	char content[MAX_RSS_SIZE];
	//char address[500] = "https://www.francetvinfo.fr/titres.rss";
	downloadRSS(address,content);
	printf("%s\n",content);
	//https://www.futura-sciences.com/rss/espace/actualites.xml
	
	return 0;
}
