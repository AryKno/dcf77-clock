#include "ledPanel.h"


int initLEDGPIOs()
{
	GPIO io;
	if (!io.Init())
		return -1;
	return 1;
}

/******************************************************************************/
/**********************************JEU*****************************************/
/******************************************************************************/




void affiche(Canvas *canvas)
{
	int x=0;
	int y =0;
	canvas->Clear();

	for(x=0;x<HAUTEUR;x++)
	{
		for(y=0;y<LARGEUR;y++)
		{	  
			canvas->SetPixel(y,x,matrice[x][y][0],matrice[x][y][1],matrice[x][y][2]);
		}
	}
}


void init_matrice()
{
	int x;
	int y;
	int color;
	for(x=0;x<HAUTEUR;x++)
	{
		for(y=0;y<LARGEUR+DEPASSEMENT;y++)
		{
			for (color=0;color<rgb;color++)
			{
				matrice[x][y][color]=0;
			}
		}
	}
}

void add_cactus()
{
   matrice[15][LARGEUR+2][1]=255;

   matrice[14][LARGEUR][1]=255;
   matrice[14][LARGEUR+1][1]=255;
   matrice[14][LARGEUR+2][1]=255;

   matrice[13][LARGEUR][1]=255;
   matrice[13][LARGEUR+2][1]=255;
   matrice[13][LARGEUR+3][1]=255;
   matrice[13][LARGEUR+4][1]=255;

   matrice[12][LARGEUR+2][1]=255;
   matrice[12][LARGEUR+4][1]=255;
}

void reset_dino()
{
	int x;
	int y;
	for(x=0;x<HAUTEUR;x++)
	{
		for(y=0;y<8;y++)
		{
			matrice[x][y][0]=0;
		}
	}
}

void Dinosaure()
{
	reset_dino();
	matrice[15- position_dinosaure][2][0]=255;

	matrice[14- position_dinosaure][2][0]=255;
	matrice[14- position_dinosaure][3][0]=255;
	matrice[14- position_dinosaure][4][0]=255;

	matrice[13- position_dinosaure][1][0]=255;
	matrice[13- position_dinosaure][2][0]=255;
	matrice[13- position_dinosaure][3][0]=255;
	matrice[13- position_dinosaure][4][0]=255;
	matrice[13- position_dinosaure][5][0]=255;

	matrice[12- position_dinosaure][0][0]=255;
	matrice[12- position_dinosaure][1][0]=255;
	matrice[12- position_dinosaure][2][0]=255;
	matrice[12- position_dinosaure][3][0]=255;
	matrice[12- position_dinosaure][4][0]=255;
	matrice[12- position_dinosaure][5][0]=255;
	matrice[12- position_dinosaure][6][0]=255;

	matrice[11- position_dinosaure][3][0]=255;
	matrice[11- position_dinosaure][4][0]=255;
	matrice[11- position_dinosaure][5][0]=255;

	matrice[10- position_dinosaure][5][0]=255;

	matrice[9- position_dinosaure][5][0]=255;
	matrice[9- position_dinosaure][6][0]=255;

	matrice[8- position_dinosaure][5][0]=255;
	matrice[8- position_dinosaure][6][0]=255;
	matrice[8- position_dinosaure][7][0]=255;

	matrice[7- position_dinosaure][5][0]=255;
	matrice[7- position_dinosaure][6][0]=255;
	matrice[7- position_dinosaure][7][0]=255;
}

void mouvement()
{
	int x;
	int y;
	//vérification de  collision 
	for(x=11;x<HAUTEUR;x++)
	{
		for(y=0;y<8;y++)
		{
			if (matrice[x][y][0]>0 && matrice[x][y][1]>0)
			{
				perdu=true;	
			}
			else
			{
				score++;
			}
		}
	}


	int matrice_tmp[HAUTEUR][LARGEUR+DEPASSEMENT];
	for(x=0;x<HAUTEUR;x++)
	{
		for(y=0;y<LARGEUR+DEPASSEMENT;y++)
		{
			matrice_tmp[x][y]=matrice[x][y][1];
		}
	}

	for(x=0;x<HAUTEUR;x++)
	{
		for(y=0;y<LARGEUR+4;y++)
		{
			matrice[x][y][1]=matrice_tmp[x][y+1];
		}
		matrice[x][LARGEUR+4][1]=0;
	}

	if(dino_mont)
	{
		if(position_dinosaure<9)
		{
			position_dinosaure++;
			if(position_dinosaure<8)
			{
				Dinosaure();
			}	
		}
		else
		{
			position_dinosaure=7;
			dino_mont=false;
		}
	}
	else
	{
		if(position_dinosaure>0)
		{
			position_dinosaure--;
			Dinosaure();
		}
	}


}


void  ajout_aleatoire_cactus()
{
	int  random = (rand() % 100 + 1);
	if(cmp_cactus<19)
	{
		cmp_cactus++;
	}
	else
	{
		if(random<(cmp_cactus))
		{
			//cout<<random<<endl;
			add_cactus();
			cmp_cactus=0;
		}
		else 
		{
			cmp_cactus++;
		}
	}
}


/******************************************************************************/
/**********************************BANDEAU*************************************/
/******************************************************************************/



/********************************TELECHARGEMENT RSS******************************/

int getTitles(char *filePath, char **titleList)	//récupère les titres et les stocke dans un tableau, retourne -1 si ce n'est pas un flux RSS ou ATOM, 0 sinon
{
	//Ouverture du fichier
	XMLDocument *doc = new XMLDocument();
	XMLError error;

	error = doc->LoadFile(filePath);
	if(error != XML_SUCCESS)
	{
		printf("LoadFile error :");
		doc->PrintError();
		exit(1);
	}
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



