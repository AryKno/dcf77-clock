#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/socket.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>


#define BUF_LEN	1024	//Taille du contenu de bt_msg_send et bt_msg_recv


#define MAX_RSS_SIZE	150000
#define SMALL_BUFFER_SIZE	100



struct bt_msg_recv {
	uint8_t client;	//est à 1 si un client est présent, sinon 0
	char *msg;	//contenu du message bluetooth reçu ou à envoyer
};

struct bt_msg_send {
	uint8_t client;	//est à 1 si un client est présent, sinon 0
	uint8_t write;	//si il vaut 1, alors la donnée est à écrire, si il vaut 0 elle a été écrite
	char *msg;	//contenu du message bluetooth reçu ou à envoyer
};


//Variable présente en thread, penser à mettre des mutex
extern struct bt_msg_send out;
extern struct bt_msg_recv in;

extern pthread_mutex_t mutex_in,mutex_out;
extern int s,client; //on y stock la socket d'écoute et la socket de dialogue



void *communication(void*);
//Fonction qui lance une socket d'écoute, à l'arrivée d'un client, on boucle sur l'écoute tout en ayant un thread d'envoi un parallèle.

int downloadRSS(char *addresse, char *proxy, char *fichier);
int cutAddr(char *address, char *host, char *path, char *domain);

