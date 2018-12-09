#include "rfcomm.h"


struct sockaddr_rc controleur_addr;


/*char msg[BUF_LEN] = { 0 };
char msg_send[BUF_LEN] = { 0 };*/



/*
void interrupt(int sig){
	if(sig == SIGINT)
	{
		printf("\nFin de la file de messages !\n");
		close(client);	//on ferme la socket client
		close(s); //On ferme la socket d'ecoute
		exit(0);
	}
}*/


void *thread_send(void *unused);



void initBluetooth()
{
	//On instancie les variables contenant l'adresse controleurale (le contrôleur Bluetooth) et l'adresse distante (le smartphone)
	//On lie le socket au port 1 du premier contrôlleur Bluetooth trouvé
	bdaddr_t tmp = { };
	controleur_addr.rc_family = AF_BLUETOOTH;
	controleur_addr.rc_bdaddr = tmp;
	controleur_addr.rc_channel = (uint8_t) 1;
}

void createListeningSocket()
{
	// on cré le socket Bluetooth communiquant sur la couche RFCOMM (comparable à TCP, d'où le SOCK_STREAM)
	s = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
	bind(s, (struct sockaddr *)&controleur_addr, sizeof(controleur_addr));

	printf("bind() Socket activé\n");
	// On définie la taille de la liste d'attente à 1, en effet seul 1 utilisateur à la fois peut agir sur l'horloge
	listen(s, 1);
	perror("createListeningSocket.listen ");

	//On attend une connexion (fonction bloquante), on l'accepte, on récupère l'adresse "smartphone_addr"
	printf("listen() : Lancement de la file d'attente de taille 1\n");

}


void *communication(void*unused)
{
	initBluetooth();
	pthread_t thrd_send;
	pthread_attr_t attr_send;


	//La variable buf contient les donnée reçues 
	char buf[BUF_LEN] = { 0 };
	
	struct sockaddr_rc smartphone_addr;
	socklen_t spAddr_len = sizeof(smartphone_addr);
	//On attend une connexion (fonction bloquante), on l'accepte, on récupère l'adresse "smartphone_addr"

	while(1)
	{
		//On cré à chaque fois la socket d'écoute pour éviter que d'autres puissent se connecter alors que quelqu'un est déjà connecté
		createListeningSocket();
		client = accept(s, (struct sockaddr *)&smartphone_addr, &spAddr_len);
		//dès qu'il y a une connexion, on ferme la socket d'écoute pour éviter que d'autres se connectent
		close(s);


		pthread_mutex_lock(&mutex_in);
		in.client = 1;
		pthread_mutex_unlock(&mutex_in);
		pthread_mutex_lock(&mutex_out);
		out.client = 1;
		pthread_mutex_unlock(&mutex_out);



		pthread_attr_init(&attr_send);
		pthread_attr_setdetachstate(&attr_send,PTHREAD_CREATE_JOINABLE);	//Joignable
		pthread_create(&thrd_send,&attr_send,thread_send,NULL);




		perror("communication.accept : ");

		//printf("accept() : Connection accéptée\n");

		//ba2str permet de convertir une adresse Bluetooth en string
		ba2str( &smartphone_addr.rc_bdaddr, buf );
		fprintf(stdout, "Connexion acceptée de : %s\n", buf);
		memset(buf, 0, sizeof(buf));

		int ret = 0;
		do{
			memset(buf,0,BUF_LEN);
			ret = recv(client,buf,BUF_LEN,0);

			if(ret > 0)
			{
				if(ret < BUF_LEN && ret > 0)	//le message complet est arrivé
				{
					pthread_mutex_lock(&mutex_in);
					memset(in.msg,0,BUF_LEN);
					strcpy(in.msg,buf);
					pthread_mutex_unlock(&mutex_in);
					printf("Message reçu : %s\n",in.msg);
				}
			}else if(ret == 0)
			{
				printf("Connexion fermée par l'appareil distant.\n");
				break;
			}
			else
			{
				printf("Erreur ! Fin de la communication.\n");
				break;
			}

		}while(1);
		printf("fin de la communication client\n");
		pthread_mutex_lock(&mutex_in);
		in.client = 0;
		pthread_mutex_unlock(&mutex_in);
		printf("mutex in libéré \n");
		pthread_mutex_lock(&mutex_out);
		out.client = 0;
		pthread_mutex_unlock(&mutex_out);
		printf("En attente du thread send.\n");
		pthread_join(thrd_send,NULL);
		printf("Thread d'écoute terminée.\n");
		close(client);
			// on ferme la connexion avec le client

		sleep(2);

	}
}


void *thread_send(void *unused)
{
	int ret = 0;
	do{
		pthread_mutex_lock(&mutex_out);
		if(out.client == 0)
		{
			pthread_mutex_unlock(&mutex_out);
			break;	//Si il n'y a plus de client, on quitte
		}
		if(out.write == 1)
		{
			int i;
			printf("Envoi du message : %s\n",out.msg);
			i = send(client,out.msg,strlen(out.msg),0);
			printf("Nombre de caractères envoyées : %d",i);
			out.write = 0;
			memset(out.msg,0,BUF_LEN);
		}
		else if (ret == -1)
		{
			/* code */printf("thread_send.erreur");
			//On débloque le mutex d'abord
			pthread_mutex_unlock(&mutex_out);
			break;
		}
		pthread_mutex_unlock(&mutex_out);
	}while(1);

	//fin de boucle ? On a perdu la connexion, donc on quitte le thread d'envoi
	pthread_exit(NULL);
}

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


int downloadRSS(char *address, char *proxy, char *fichier)
{
	char host[SMALL_BUFFER_SIZE],path[SMALL_BUFFER_SIZE],domain[SMALL_BUFFER_SIZE], buf[MAX_RSS_SIZE];
	cutAddr(address,host,path,domain);
	printf("\n%s %s %s\n",host,path,domain);
	int statut = 0;
	int sock = 0;
	struct addrinfo *addrs,*addr;



	struct sockaddr_in *ip;
	char *ips;
	
	statut=getaddrinfo("proxy.polytech-lille.fr",NULL,NULL,&addrs);
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
	//on se connecte au port 3128 proxy polytech
	ip->sin_port = htons(3128);
	printf("Port : %d\n",ip->sin_port);
	//création de la socket
	sock = socket(PF_INET,SOCK_STREAM,0);
	struct timeval timeout;      
    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
	printf("Proxy trouvé !\n");
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


	int n = 0;
	printf("connecté\n");
	//On est connecté, du coup on lui envoi une requête GET
	
	char request[2000];
	char reponse[40];
	sprintf(request,"CONNECT %s HTTP/1.1\r\nHost: %s:80\r\n\r\n",host,host);
	printf("%s\n",request);
	write(sock,request,2000);
/*while ((n = read(sock, reponse, sizeof(reponse)-1)) > 0)
        {
                reponse[n] = 0;
                strcat(buf,reponse);
        }
	printf("Réponse du proxy : %s\n",buf);*/
        sprintf(request,"GET https://%s%s HTTP/1.1\r\nHost: %s\r\nConnection: keep-alive\r\nKeep-Alive: 300\r\n\r\n",host,path,host);
	write(sock,request,2000);
	char http[50],code[50],status[50];
	n = 0;
	buf[0] = '\0';
	while ((n = read(sock, reponse, sizeof(reponse)-1)) > 0)
	{
		reponse[n] = 0;
		strcat(buf,reponse);
	} 

	if(sscanf(buf,"%s %s %s",http,code,status)!=3) exit(-1);

	printf("Réponse : %s\n",buf);
	//on vérifie la bonne réception :
	if(strcmp(code,"200") == 0 && strcmp(status,"OK") == 0)
	{
		printf("Page reçue\n");
		char *deb = 0;
		deb = strstr(buf,"\r\n\r\n");
		if(deb != 0)
		{
			strcpy(fichier,deb+4);
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
