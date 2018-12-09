#include "libGPIOaccess.h"
#include <time.h>
#include <signal.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>
#define DCF77IN 	24
#define	SAMPLES_SIZE	1200
#define HALF_SAMPLES_SIZE	600

#define SIGPERIOD SIGRTMIN	//On défini le signal à déclencher à chaques période comme étant le premier signal temps réel
#define SAMPLE_PERIOD_NSEC	0
#define SAMPLE_PERIOD	100000	//x ms = x*1000
#define DATA_SIZE	60
struct sigaction interruption;

void hand(int sig){
	if(sig == SIGINT)
	{
		printf("\nMessage : fin du programme");
		GPIOrelease(DCF77IN);
		exit(sig);
	}
}

/*struct dcf77{
	uint8_t min;
	uint8_t hour;
	uint8_t day;
	uint8_t month;
	uint8_t year;
	uint8_t dayOfWeek;
};*/

void initGPIOpin(int pin)
{//initialise la pin du GPIO
	GPIOuse(pin);
	usleep(10);
	GPIOdirection(pin,IN);
	usleep(10);
	GPIOactive(pin,LOW);
	usleep(10);
}

void printTab(uint8_t *tab,int size)
{//imprime un tableau dans la console (utilisé pour débugger les fonctions de décodage)
	int i;
	for(i=0;i<size;i++)
	{
		printf("%d",tab[i]);
		fflush(stdout);
	}
	printf("\n");
}


void getSample(uint8_t *samples,int pin)
{//Effectue régulièrement une mesures sur la pin "pin" du GPIO et enregistre le résultat dans "samples"
	int i;
	long long t = 0;
	struct timeval t1, t2;
	for(i=0;i<SAMPLES_SIZE;i++)
	{	
		samples[i] = GPIOread(pin);
	//	printf("%d",samples[i]);
	//	fflush(stdout);
		gettimeofday(&t1, NULL);
		do{
			gettimeofday(&t2, NULL);
			//printf("Boucle temporelle %ld\n",t2.tv_usec);
			t = ((t2.tv_sec * 1000000) + t2.tv_usec)-((t1.tv_sec * 1000000) + t1.tv_usec);
		}while(t<SAMPLE_PERIOD);
		//printf("Test  %lld\n",t);
	}

}

int parityCalculator(int start, int end, uint8_t *data)
{//Effectue la somme des valeurs de "data" comprises entre start et end
	int k,p=0;
	for(k = start; k<=end;k++)
	{
		p += data[k];
	}
	return p;
}

int decodeSample(uint8_t *sample, uint8_t *data) //bit 0 toujours à 0, bit 20 toujours à 1, aucune valeur au bit 59 ou alors bit 0 si ajout d'une seconde intercalaire
{//récupère une trame depuis "sample", retourne la trame de taille 60 dans data
	int i = 0,j;
	while(i+59*10+1 < SAMPLES_SIZE)
	{
		if((sample[i] == 1 && sample[i+1] == 0) && (sample[i+20*10] == 1 && sample[i+20*10+1] == 1) && (sample[i+59*10] == 0 && sample[i+59*10+1] == 0))
		{	//Si le bit 0 est à 0, si le bit 20 est à 1, si le bit 59 n'a aucune valeurs, alors i est l'indice de début de trame
			//On vérifie les paritée, elle serviront à nous assurer que nous ne sommes pas décalé dans la trame

			int pm = 0, ph = 0, pd = 0;
			
			for(j=0;j<DATA_SIZE;j++)
			{
				if(sample[i+j*10] == 1 && sample[i+j*10+1] == 1)
					data[j] = 1;
				else if(sample[i+j*10] == 1 && sample[i+j*10+1] == 0)
					data[j] = 0;
				printf("%d",data[j]);
				fflush(stdout);
			}
			printTab(data,DATA_SIZE);
			//parité sur les minutes
			pm = parityCalculator(21, 28, data);

			//parité sur les heures
			ph = parityCalculator(29, 35, data);

			//parité sur la date
			pd = parityCalculator(36, 58, data);

			if(pm%2 == 0 && ph%2 == 0 && pd%2 == 0 && data[59] == 0)
			{
		                printf("Echantillon correct\n");
				printf("décalage %d\n",SAMPLES_SIZE - i + 600);
				return (SAMPLES_SIZE - i+600);	//permet de retrouver le retard pour qu'il soit compensé.
			}
			//Si la parité n'est pas bonne ou que le dernier bit n'est pas à 0 on repart dans la boucle while
		}
		i++;
	}
        printf("Echantillon incorrect\n");
	return -1; //Si nous n'avons pas de trame correct, on quitte la fonction de decodage

}




void getTime(uint8_t *data, struct tm *info)
{	//A partir de data, récupère l'heure et la stock dans info

	info->tm_min = data[21] + data[22]*2 + data[23]*4 + data[24]*8 + data[25]*10 + data[26]*20 + data[27]*40;
	info->tm_hour =  data[29] + data[30]*2 + data[31]*4 + data[32]*8 + data[33]*10 + data[34]*20;
	info->tm_mday = data[36] + data[37]*2 + data[38]*4 + data[39]*8 + data[40]*10 + data[41]*20; 
	info->tm_mon = data[45] + data[46]*2 + data[47]*4 + data[48]*8 + data[49]*10-1;
	info->tm_year = data[50] + data[51]*2 + data[52]*4 + data[53]*8 + data[54]*10 + data[55]*20 + data[56]*40 + data[57]*80+100;
	info->tm_wday = data[42] + data[43]*2 + data[44]*4-1; 
	info->tm_sec = 0;
	if(info->tm_wday == 0)
		info->tm_wday = 1;
	if(info->tm_wday == 6)
		info->tm_wday = 0;
	printf("%d %d %d %d %d %d %d\n", info->tm_hour, info->tm_min, info->tm_sec, info->tm_mday, info->tm_mon, info->tm_year, info->tm_wday);
}

int isConsistent(struct tm *info)
{	//test la cohérence de la date reçu, retoune 1 si le résultat est cohérent, 0 sinon
	if(info->tm_min < 60 && info->tm_hour < 24 && info->tm_mday < 31 && info->tm_mon < 12 && info->tm_mon > -1 && info->tm_year > 100 && info->tm_wday > -1)
		return 1;
	else
		return -1;
}

int main()
{
	//Interruption sur la fin du programme
	interruption.sa_handler = hand;
	sigaction(SIGINT, &interruption,NULL);

	//Declaration du signal periodique

	char buffer[26] = {0};
	printf("Instance du GPIO 24 :\n");
	initGPIOpin(DCF77IN);
	uint8_t samples[SAMPLES_SIZE] = {0};
	uint8_t data[DATA_SIZE] = {0};
	int ret = 0;

	struct tm t;
	printf("\n");
	printf("Récupération de l'échantillon sur 130 secondes...\n");
	do
	{
		do
		{
			printf("Décodage de l'échantillon...\n");
			getSample(samples,DCF77IN);
			printf("Echantillon : \n");
			printTab(samples,SAMPLES_SIZE);
			printf("\n");
		}while((ret = decodeSample(samples,data)) < 0);
		printf("Trame récupérée : \n");
		printTab(data,60);
		getTime(data, &t);
		strftime(buffer, 26,"%c",&t); //"%Y-%m-%d %H:%M:%S",&time);
		printf("La date récupérée est : %s\n",buffer);
	}
	while(isConsistent(&t) != 1);
	printf("Date cohérente\n");
	GPIOrelease(DCF77IN);


	//on transforme l'heure en timestamp
	time_t timestamp = mktime(&t);
	stime(&timestamp);
	printf("Il est :  %d heure(s) et %d minute(s)\n",t.tm_hour,t.tm_min);
	/*if(ret > 0)
	{
		time_t newTime = time(NULL) + ret;
		stime(&newTime);
	}*/

	return 0;
}
