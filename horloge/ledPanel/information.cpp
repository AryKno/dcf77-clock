#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define TAILLE_MSG 500
#define NB 10
#define CLE_MSG (key_t)1000


typedef struct  {
long type;
char content[TAILLE_MSG];
} Message;

bool progStatus = false;


int main()
{

	int msgid =-1;
	Message msg;
	/*printf("\nPing : Attente de création de la FDM...\n");
	while(msgid == -1)
	{
		msgid = msgget(CLE_MSG, 0666);
	}
	printf("Ping : Pile de message créé!\n");

	strcpy(msg.content, "pifghngaaaa\n");
	msg.type = 1;	
	msgsnd(msgid, &msg, TAILLE_MSG, 0);
	printf("Ping : message envoyé !\n");
	//Renvoie un accusé de réception ACK à l'émetteur
	msg.type = 2;	//On définit en tant que ACK
	msgrcv(msgid, &msg, TAILLE_MSG, 2,0);
	printf("Ping : Message reçu : %s\n",msg.content);*/






	/*strcpy(msg.content, "ping\n");
	msg.type = 1;	
	msgsnd(msgid, &msg, TAILLE_MSG, 0);
	printf("Ping : msg envoyé !\n");
	//Renvoie un accusé de réception ACK à l'émetteur
	msg.type = 2;	//On définit en tant que ACK
	printf("Ping : Message reçu : %s\n",msg.content);*/

	

}
