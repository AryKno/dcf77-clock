#include "ledPanel.h"

#include "rfcomm.h"
//#include "graphics.h"
//#include "led-matrix.h"
#include "errno.h"
extern "C"
{
	#include <pthread.h>
}

#define INFO_MSG_REQUEST	65
#define INFO_MSG_PERSO		66
#define INFO_RSS_REQUEST	67
#define INFO_RSS_PATH		68
#define INFO_END_MSG		69

#define	GAME_REQUEST		70
#define	GAME_BUTTON			71
#define	GAME_END			72

#define	CLOCK_REQUEST		73
#define	CLOCK_SEND			74
#define	CLOCK_REQUEST_SCORE		75

#define TIMEZONE_CHANGE		76

#define FONT_PATH	 "ledPanel/fonts/helvR12.bdf"
#define FONT_WIDTH	10
//constante ledmatrix
#define ROWS	16
#define CHAINS	1

#define LEDS_SCROLL_DELAY 50000


using namespace std;
using namespace tinyxml2;
using namespace rgb_matrix;

struct sigaction interruption;
int s,client;

struct bt_msg_send out;
struct bt_msg_recv in;

pthread_mutex_t mutex_in,mutex_out;

void bandeau(Canvas *canvas, char *msg);

void jeu(Canvas *canvas, char *tmpInMsg); //est appellée lorsque le joueur requisitionne le jeu
void bandeau(Canvas *canvas, char *msg);	//est appellée lorsque le joueur requisitionne le bandeau
void changementTimeZone(char * tmpInMsg);











