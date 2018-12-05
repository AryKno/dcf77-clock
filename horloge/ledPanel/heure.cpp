#include "include/led-matrix.h"
#include "include/graphics.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <signal.h>
#include <string>
#include <iostream>
#include <ctime>


#define FONT_PATH "fonts/6x13.bdf"
#define STR_LEN   10

using namespace rgb_matrix;
using namespace std;



struct sigaction interruption;
RGBMatrix *canvasSig = NULL;


void interrupt(int sig){
	if(sig == SIGINT)
	{
		printf("\nFin du programme !\n");
		canvasSig->Clear();
		exit(0);
	}
}


int main()
{

	interruption.sa_handler = interrupt;
	sigaction(SIGINT, &interruption,NULL);
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

  Font *font = new Font();
  font->LoadFont(FONT_PATH);

  Color color(255, 255, 0);



  RGBMatrix *canvas = new RGBMatrix(&io, rows, chain);
  canvasSig = canvas;

  canvas->SetPWMBits(1);
  canvas->Fill(0, 0, 0);


  cout<<"Height : "<<font->height()<<endl<<"baseline "<<font->baseline()<<endl;
  char str[STR_LEN];
  int w = 2;


  time_t timer;
  struct tm *date;
  while(1)
  {
    timer = time(NULL);
    date = localtime(&timer);
    strftime(str,STR_LEN,"%H:%M",date);
    DrawText(canvas,*font,w,font->baseline(),color,str);
    sleep(1);
	printf("Heure actuelle : %s, timestamp %ld\n",str,timer);
    canvas->Clear();
  }
  canvas->Clear();
  delete font;
  delete canvas;
  return 0;
}
