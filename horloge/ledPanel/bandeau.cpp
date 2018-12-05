#include "include/led-matrix.h"
#include "include/graphics.h"

#include <unistd.h>
#include <math.h>
#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>
#include <cstring>
#include <iostream>

#define SIZE 50

#define FONT_PATH "helvR12.bdf"

#define CHAR_WIDTH 10
#define STR_LEN	1024

using namespace rgb_matrix;
using namespace std;


int strHorizontalScroll(Canvas *canvas, Font &font, Color &color,char  *str, int delay)
{
  int w = 32;
  int strW =strlen(str)*CHAR_WIDTH;
  int i = 0;
  i = DrawText(canvas,font,w,font.baseline(),color,str);
  do
  {
    usleep(delay);
    w--;
    if(w <= -32-i)
    w=32;
    //printf("%d\n",w);
    canvas->Clear();
    DrawText(canvas,font,w,font.baseline(),color,str);
	//printf("Taille du texte %d\n",i);
  }while(1);
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
  int chain = 1;   // On affiche sur la première matrice de LEDs

  Font *font = new Font();
  font->LoadFont(FONT_PATH);

  Color color(255, 0, 0);



  RGBMatrix *canvas = new RGBMatrix(&io, rows, chain);

  canvas->SetPWMBits(1);
  canvas->Fill(0, 0, 0);

  /*char str[SIZE];
  strcpy(str,"Hello");*/

  char str[STR_LEN] = "Test jp";


  cout<<"Height : "<<font->height()<<endl<<"baseline "<<font->baseline()<<endl;
 // DrawText(canvas,*font,0,10,color,str);

  strHorizontalScroll(canvas,*font,color,str,50000);

  /*Canvas *canvas = new RGBMatrix(&io, rows, chain);

  DrawOnCanvas(canvas);    // Using the canvas.
  rgb_matrix::DrawText(canvas, font, x, y + font.baseline(), color, line);

  // Animation finished. Shut down the RGB matrix.
  canvas->Clear();
  delete canvas;*/
  canvas->Clear();
  delete font;
  delete canvas;
  return 0;
}

/*int strWidth(string *str, Font *font)
{ //Retourne la largeur de la chaîne, en LED
  int res = 0;
  for(int i = 0; i < str->size(); i++)
  {
    res += font->CharacterWidth(str[i]);
  }
}*/



