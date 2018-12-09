/*
*	Ce code est une adaptation du code disponible à l'adresse https://elinux.org/RPi_GPIO_Code_Samples#sysfs
*
*
*
*
*
*
*/


#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>

#define IN  0
#define OUT 1
 
#define LOW  0
#define HIGH 1
 
#define PIN  24 /* P1-18 */
#define POUT 4  /* P1-07 */
#define BUF_SIZE 3

#define DIRECTION_MAX 35
#define VALUE_MAX 30
#define ACTIVE_MAX 34


int GPIOuse(int pin);
int GPIOrelease(int pin);
int GPIOdirection(int pin, int dir);
int GPIOread(int pin);
int GPIOactive(int pin, int state);



