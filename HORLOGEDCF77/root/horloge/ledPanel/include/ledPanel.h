
extern "C" {
	#include <unistd.h>
	#include <math.h>
}
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

#include "led-matrix.h"
#include "graphics.h"
#include "tinyxml2.h"



#define SIZE 500



#define CHAR_WIDTH 10
using namespace std;

using namespace tinyxml2;
using namespace rgb_matrix;


int initLEDGPIOs();
//initialise les GPIOs pour le panneau de LEDs


/******************************************************************************/
/**********************************JEU*****************************************/
/******************************************************************************/

#define LARGEUR 32
#define HAUTEUR 16
#define DEPASSEMENT 5
#define rgb 3


//variables
extern int  matrice[HAUTEUR][LARGEUR+DEPASSEMENT][rgb];
extern int  position_dinosaure; //par défaut 0
extern bool dino_mont;	//par défaut false
extern bool perdu;		//par défaut false
extern int cmp_cactus;		//par défaut 0
extern int  score;			//par défaut 0





void affiche (Canvas *canvas);
//Affiche la matrice "matrice" sur le panneau de LEDs

void init_matrice();
//Initialise la matrice "matrice" à 0

void add_cactus();
//Ajoute un cactus à l'extrémitée droite de la matrice "matrice"

void reset_dino();
//Efface uniquement le dinosaure

void Dinosaure();
//Dessine le dinosaure dans la matrice

void mouvement();
//fais avancer le jeu, d'un pixel

void  ajout_aleatoire_cactus();
//Ajoute un cactus sur la route du dinosaure, de manière aléatoire

/******************************************************************************/
/**********************************BANDEAU*************************************/
/******************************************************************************/



/*********************************TELECHARGEMENT RSS*****************************/






#define NODE_SIZE 30
#define TITLE_LIST_SIZE	10



int getTitles(char *filePath, char **titleList);	
//récupère les titres et les stocke dans un tableau, retourne -1 si ce n'est pas un flux RSS ou ATOM, 0 sinon

char ** initStrTab(int strNbr, int charNbr);
void freeStrTab(char **strTab, int strNbr);


