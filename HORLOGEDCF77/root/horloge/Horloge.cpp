#include "Horloge.h"

 int  matrice[HAUTEUR][LARGEUR+DEPASSEMENT][rgb];
 int  position_dinosaure = 0; //par défaut 0
 bool dino_mont = false;	//par défaut false
 bool perdu = false;		//par défaut false
 int cmp_cactus = 0;		//par défaut 0
 int  score_dino = 0;			//par défaut 0
int score;

void interrupt(int sig){
	if(sig == SIGINT)
	{
		printf("\nFin du programme !\n");
		close(client);	//on ferme la socket client
		close(s); //On ferme la socket d'ecoute
		exit(0);
	}
}




void changementTimeZone(char * tmpInMsg)
{
        char path[50];
        if(((int)tmpInMsg[1]) < 0)
                sprintf(path,"/usr/share/zoneinfo/Etc/GMT%d",tmpInMsg[1]);
        else
                sprintf(path,"/usr/share/zoneinfo/Etc/GMT+%d",tmpInMsg[1]);
        int i = symlink(path,"/etc/localtime");
        if(i!=0 && errno == EEXIST)
        {
                remove("/etc/localtime");
                symlink(path,"/etc/localtime");
        }
}

void jeu(Canvas *canvas, char *tmpInMsg)
{
	srand(time(NULL));

	position_dinosaure = 0; //par défaut 0
	dino_mont = false;        //par défaut false
	perdu = false;            //par défaut false
	cmp_cactus = 0;            //par défaut 0
	score_dino = 0;                        //par défaut 0

	init_matrice();
	Dinosaure();
	//	dino_mont=true;color
	//add_cactus();
	printf("Matrice initialisée, juste avant boucle de jeu\n");
	while(1)
	{
	//	printf("Dans la boucle de jeu\n");
	//	printf("Etat des variables du jeu : position_dinosaure : %d\tdino_mon : %d\tperdu : %d\tcmp_cactus : %d\tscore : %d\n",position_dinosaure,dino_mont,perdu,cmp_cactus,score);
		usleep(50000);
		mouvement();
		score_dino++;
	//	printf("Avant affiche canvas\n");
		affiche(canvas);
	//	printf("Avant ajout aléatoire de cactus\n");
		ajout_aleatoire_cactus();
	//	printf("Avant mutex d'ecoute du boutton");
		memset(tmpInMsg,0,BUF_LEN);
		pthread_mutex_lock(&mutex_in);
		strcpy(tmpInMsg,in.msg);
		memset(in.msg,0,BUF_LEN);
		pthread_mutex_unlock(&mutex_in);

		if(tmpInMsg[0] == GAME_BUTTON)
			dino_mont=true;

		if(perdu == true)	//le joueur à perdu
		{
			pthread_mutex_lock(&mutex_out);
			sprintf(out.msg,"%d",score_dino);
			out.write = 1;
			pthread_mutex_unlock(&mutex_out);
			printf("Vous avez perdu ! Score : %d\n",score_dino);
			break;
		}
	} 

	// Animation finished. 
	canvas->Clear();
	return;

}


void bandeau(Canvas *canvas, char *msg)
{
	printf("On est dans bandeau\n");	
	//on test si le message commence par http
	char *deb = 0;
	char *tmp = (char*)malloc(sizeof(char)*BUF_LEN);
	char *fichier = (char*)malloc(sizeof(char)*MAX_RSS_SIZE);
	char *titles = (char*)malloc(sizeof(char)*1000);
	strcpy(tmp,&msg[1]);

	Font *font = new Font();
  	font->LoadFont(FONT_PATH);
  	Color color(255, 0, 0);

	if((deb = strstr(msg,"http://")) == &msg[1] || (deb = strstr(msg,"https://")) == &msg[1])	//le 0e caractère de la chaine correspond au code, la chaine commence réellement au 1er caractère
	{
		//on télécharge le contenu du fichier
		int i = downloadRSS(tmp,"rien",fichier);
		printf("Valeur de downloadRSS :%d\n",i);
		if(i == 1)	//si le fichier est xml et correctement chargé
		{
		 	char filePath[15] = "flux.rss";
			printf("Avant initialisation de titleList\n");
			char **titleList = initStrTab(TITLE_LIST_SIZE,500);
			//on stock le flux dans un fichier
			printf("Avant initialisation du fichier flux.rss\n");
			FILE *f = fopen(filePath, "w+");
			fprintf(f, "%s", fichier);
			fclose(f);
			printf("Avant fermeture du fichier\n");
			//récupère les titres et les stocke dans un tableau, retourne -1 si ce n'est pas un flux RSS ou ATOM, 0 sinon
			printf("Avant extraction des titres\n");
			getTitles(filePath, titleList);
			printf("Avant la copie des titres\n");
			//on boucle pour assembler les titres en une chaîne
			strcpy(titles,"\n");
			for(int i =0;i<TITLE_LIST_SIZE;i++)
			{
				printf("Juste avant le sprintf\n");
				sprintf(tmp,"%d. %s         ",i+1,titleList[i]);
				printf("Juste avant le strcat\n");
				strcat(titles,tmp);
				printf("Juste après le strcat\n");
			}
		}
		else	//si il y a eu une erreur lors du chargement
		{
			strcpy(titles,"Impossible de charger le fichier");
		}

		//on extrait les titres du flux
	}
	else	//le fichier ne commence pas par http ou https, le message est contenu dans msg
	{
		strcpy(titles,&msg[1]);	//on affiche le message
	}
	printf("Contenu de titles : %s\n",titles);
	int w = 0;
	int i = 0;
	i = DrawText(canvas,*font,w,font->baseline(),color,titles);
	//i = strlen(titles);
	printf("Taille de la chaine %d\n",i);
	printf("Contenu de la chaine ! %s\n",titles);
	char code = 0;
	do
	{
		printf("Itération des LEDs w=%d  i=%d\n",w,i);
		pthread_mutex_lock(&mutex_in);
		code = in.msg[0];
		pthread_mutex_unlock(&mutex_in);
		usleep(LEDS_SCROLL_DELAY);
		if(code == INFO_END_MSG)
			break;	//on quitte la boucle et donc le bandeau
		w--;
		if(w <= -LARGEUR-i)
		w=LARGEUR;
		//printf("%d\n",w);
		canvas->Clear();
		DrawText(canvas,*font,w,font->baseline(),color,titles);
		//printf("Taille du texte %d\n",i);
	}while(1);
	printf("Fin du bandeau\n");
	delete font;
	canvas->Clear();
	free(tmp);
	free(fichier);
	free(titles);
}


int main()
{

		
	//On surveille le SIGINT (Ctrl+C)
	interruption.sa_handler = interrupt;
	sigaction(SIGINT, &interruption,NULL);

	in.client = 0;
	in.msg = (char*)malloc(sizeof(char)*BUF_LEN);
	out.client = 0;
	out.msg = (char*)malloc(sizeof(char)*BUF_LEN);
	out.write = 0;
	//communication();

	pthread_t bt_thread;
	pthread_attr_t attr;

	pthread_mutex_init(&mutex_in,NULL);
	pthread_mutex_init(&mutex_out,NULL);


	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr,PTHREAD_CREATE_DETACHED);
	pthread_create(&bt_thread,&attr,communication,NULL);

	//on initialise les GPIOs du panneau de LED
	GPIO io;
	if (!io.Init())
	{
		exit(EXIT_FAILURE);
	}

	//On initialise le canvas
	int rows = ROWS;   // A 32x32 display. Use 16 when this is a 16x32 display.
	int chains = CHAINS;   // On affiche sur la première matrice de LEDs

	RGBMatrix *canvas = new RGBMatrix(&io, rows, chains);

	canvas->SetPWMBits(1);
	canvas->Clear();

	
	//boucle infinie du programme
	int tmpInClient = 0;
	char *tmpInMsg = (char*)malloc(sizeof(char)*BUF_LEN);
	printf("On entre dans la boucle d'ecoute des clients\n");
	while(1)
	{	
		do{//On attend un client
			pthread_mutex_lock(&mutex_in);
			tmpInClient = in.client;
			pthread_mutex_unlock(&mutex_in);
		}while(tmpInClient == 0);
		printf("Un client est arrivé !\n");
		//Un client est arrivé, on lit ce qu'il souhaite faire.
		while(1)
		{
		sleep(1);
		pthread_mutex_lock(&mutex_in);
		printf("Entrée dans le mutex\n");
		tmpInClient = in.client;
		strcpy(tmpInMsg,in.msg);
		printf("Valeur de in.msg : %s\n",tmpInMsg);
		printf("Debut de memset\n");
		memset(in.msg,0,BUF_LEN);
		printf("Fin de memset\n");
		pthread_mutex_unlock(&mutex_in);
		printf("Debut du testde requete \n");
		int testVide = strlen(tmpInMsg);
		printf("Requete commande %d\n",tmpInMsg[0]);	
		if(testVide != 0)
		{
		printf("La chaine n'est pas vide !\n");
		if(tmpInMsg[0] == INFO_MSG_REQUEST)
		{	//on demande le bandeau d'info
			printf("Bandeau requete : %d",tmpInMsg[0]);
			bandeau(canvas,tmpInMsg);
		}
		else if(tmpInMsg[0] == GAME_REQUEST)
		{	//on demande le jeu
			printf("Jeu demandé\n");
			jeu(canvas,tmpInMsg);
		}
		else if(tmpInMsg[0] == TIMEZONE_CHANGE)
		{
			changementTimeZone(tmpInMsg);	
		}
		else
		{
			printf("ERROR : Commande invalide.\n");
		}
		printf("fin de boucle de requete\n");
		}
		}

	}

	delete canvas;

	pthread_mutex_lock(&mutex_in);
	free(in.msg);
	pthread_mutex_unlock(&mutex_in);
	pthread_mutex_lock(&mutex_out);
	free(out.msg);
	pthread_mutex_unlock(&mutex_out);
	return 0;
}
