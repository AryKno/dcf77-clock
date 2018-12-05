
#include "include/led-matrix.h"
#include "include/graphics.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <ctime>
#define SIZE 50


#define CHAR_WIDTH 10


#define largeur 32
#define hauteur 16
#define rgb 3
using namespace rgb_matrix;
using namespace std;

int  matrice[hauteur][largeur+5][rgb];
int  position_dinosaure=0;
bool dino_mont=false;
bool perdu=false;
int cmp_cactus=0;
int  score =0;
void affiche (Canvas *canvas)
{
  int x=0;
  int y =0;
	 canvas->Clear();

  for(x=0;x<hauteur;x++)
    {
      for(y=0;y<largeur;y++)
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
  for(x=0;x<hauteur;x++)
    {
      for(y=0;y<largeur+5;y++)
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
   matrice[15][largeur+2][1]=255;

   matrice[14][largeur][1]=255;
   matrice[14][largeur+1][1]=255;
   matrice[14][largeur+2][1]=255;

   matrice[13][largeur][1]=255;
   matrice[13][largeur+2][1]=255;
   matrice[13][largeur+3][1]=255;
   matrice[13][largeur+4][1]=255;

   matrice[12][largeur+2][1]=255;
   matrice[12][largeur+4][1]=255;
}
void  reset_dino()
{
	int x;
  int y;
  for(x=0;x<hauteur;x++)
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
//vérification de  collition 
for(x=11;x<hauteur;x++)
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
int matrice_tmp[hauteur][largeur+5];
   for(x=0;x<hauteur;x++)
    {
      for(y=0;y<largeur+5;y++)
	{
	  matrice_tmp[x][y]=matrice[x][y][1];
	}
    }
   for(x=0;x<hauteur;x++)
    {
      for(y=0;y<largeur+4;y++)
	{
	  matrice[x][y][1]=matrice_tmp[x][y+1];
	}
      matrice[x][largeur+4][1]=0;
    }

   if (dino_mont)
     {
	if (position_dinosaure<9)
	{
	position_dinosaure++;
	     if (position_dinosaure<8)
		{
	   		Dinosaure();
	}	}
       else
	 {
		position_dinosaure=7;
	   dino_mont=false;
	 }
     }
   else
     {
	
       
       if ( position_dinosaure>0)
	 {
	   position_dinosaure--;
	   Dinosaure();
	 }
     }

  
}
void  ajout_aleatoire_cactus()
{
 int  random = (rand() % 100 + 1);
     cout<<random<<endl;

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
int main(int argc, char *argv[]) {
  /*
   * Set up GPIO pins. This fails when not running as root.
   */
  GPIO io;
  if (!io.Init())
    return 1;
    
  /*
   * Set up the RGBMatrix. It implements a 'Canvas' interface.
   */
  int rows = 16;   // A 32x32 display. Use 16 when this is a 16x32 display.
  int chain = 2;   // On affiche sur la première matrice de LEDs




  RGBMatrix *canvas = new RGBMatrix(&io, rows, chain);

  canvas->SetPWMBits(1);
  canvas->Fill(0, 0, 0);

 

  

 	srand (time(NULL));
	init_matrice();
        Dinosaure();
//	dino_mont=true;color




        
int  cmp=0;
//add_cactus();
while(1)
{
	usleep(100000);
	mouvement();
  affiche(canvas);
	ajout_aleatoire_cactus();
	if (cmp ==18)
	{
	
		dino_mont=true;
		cmp=0;
	}
	cmp++;
 } 

  // Animation finished. 
  canvas->Clear();
  delete canvas;
  return 0;
}


