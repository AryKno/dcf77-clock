#include "tinyxml2.h"
#include <cerrno>
#include <stdlib.h>
#include <string.h>
#include <time.h>
using namespace std;
using namespace tinyxml2;


//Constantes de taille de tableaux
#define SIZE	500
#define NODE_SIZE 30
#define TITLE_LIST_SIZE	10

char errorOutput[SIZE]; //message d'erreur affiché sur la matrice de LEDs


int getTitles(char *filePath, char **titleList)	//récupère les titres et les stocke dans un tableau, retourne -1 si ce n'est pas un flux RSS ou ATOM, 0 sinon
{
	//Ouverture du fichier
	XMLDocument *doc = new XMLDocument();
	XMLError error;

	doc->NewText(filePath);
	
	//Extraction des titres

	char itemNode[NODE_SIZE];


	XMLNode *mainNode = doc->FirstChildElement("rss");
	XMLNode *scndNode;
	if(mainNode != 0)
	{	//C'est un flux RSS
		scndNode = mainNode->FirstChildElement("channel");	//On rentre dans "rss" puis dans "channel"
		strcpy(itemNode, "item");
	}
	else if((scndNode = doc->FirstChildElement("feed")) != 0)	//C'est un flux ATOM, on rentre dans "feed"
	{
		strcpy(itemNode, "entry");
	}
	else
	{
		return -1;	//Ce n'est ni un flux RSS, ni ATOM, on quitte la fonction
	}

	XMLNode *item = scndNode->FirstChildElement(itemNode);	//item vaut soir "entry" soit "item"
	if(item != 0)
	{	//si item existe on boucle sur les titres
		int i=0;
		do {
			strcpy(titleList[i],item->FirstChildElement("title")->GetText());
			i++;
			item = item->NextSiblingElement(itemNode);
		}while(i < TITLE_LIST_SIZE && item != 0);
	}
	delete doc;
	
	return 0;



	//printf("%d", item);
}

char ** initStrTab(int strNbr, int charNbr)
{
	char **titleList = (char**)malloc(strNbr*sizeof(char*));
	int i;
	for(i=0;i<strNbr;i++)
	{
		titleList[i] = (char*)malloc(charNbr*sizeof(char));
	}
	return titleList;
}

void freeStrTab(char **strTab, int strNbr)
{
	int i;
	for(i=0;i<strNbr;i++)
	{
		free(strTab[i]);
	}
	free(strTab);
}
int main()
{
	char filePath[SIZE];

	strcpy(filePath, "actualites.xml");



	char **titleList = initStrTab(TITLE_LIST_SIZE,SIZE);

	//On récupère les titres :

	if(getTitles("CACA",titleList) != 0)
	{
		strcpy(errorOutput,"Fichier RSS/ATOM incorrect");

	}
	int i;

	for(i=0;i<TITLE_LIST_SIZE;i++)
	printf("%s\n", titleList[i]);

	freeStrTab(titleList,TITLE_LIST_SIZE);
	return 0;
}
