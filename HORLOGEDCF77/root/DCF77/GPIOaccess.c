/*
*	Ce code est une adaptation du code disponible à l'adresse https://elinux.org/RPi_GPIO_Code_Samples#sysfs
*
*
*
*
*
*
*/


#include "libGPIOaccess.h"


int GPIOuse(int pin)	//Prend le port GPIO
{
	char buffer[BUF_SIZE];
	uint8_t bytes_written;
	int fd;
	fd = open("/sys/class/gpio/export", O_WRONLY);
	if (fd ==-1) {
		perror("GPIOuse, open");
		return -1;
	}
 
	bytes_written = snprintf(buffer, BUF_SIZE, "%d", pin);
	write(fd, buffer, bytes_written);

	close(fd);
	return 0;
}

int GPIOrelease(int pin)	//Relache le port GPIO
{
	char buffer[BUF_SIZE];
	uint8_t bytes_written;
	int fd;
	fd = open("/sys/class/gpio/unexport", O_WRONLY);
	if (fd ==-1) {
		perror("GPIOrelease, open()");
		return -1;
	}
 
	bytes_written = snprintf(buffer, BUF_SIZE, "%d", pin);
	write(fd, buffer, bytes_written);
	close(fd);
	return 0;
}


int GPIOdirection(int pin, int dir)	//Défini si le port GPIO est une entrée ou une sortie
{
	static const char s_directions_str[]  = "in\0out";
 
	char path[DIRECTION_MAX];
	int fd;
 
	snprintf(path, DIRECTION_MAX, "/sys/class/gpio/gpio%d/direction", pin);
	fd = open(path, O_WRONLY);
	if (fd == -1) {
		perror("GPIOdirection, open()");
		return -1;
	}
 
	if (-1 == write(fd, &s_directions_str[IN == dir ? 0 : 3], IN == dir ? 2 : 3)) {
		fprintf(stderr, "Failed to set direction!\n");
		perror("GPIOdirection, write()");
		exit(EXIT_FAILURE);
	}
 
	close(fd);
	return 0;
}

int GPIOread(int pin)	//Retourne la valeur d'un GPIO
{
	char path[VALUE_MAX];
	char value_str[3];
	int fd;
 
	snprintf(path, VALUE_MAX, "/sys/class/gpio/gpio%d/value", pin);
	fd = open(path, O_RDONLY);
	if (fd == -1) {
		perror("GPIOread, open()");
		return -1;
	}
 
	if (-1 == read(fd, value_str, 3)) {
		fprintf(stderr, "Failed to read value!\n");
		return -1;
	}
 
	close(fd);
 
	return(atoi(value_str));
}

int test()
{
	return 0;
}

int GPIOactive(int pin, int state)	//Défini la valeur de retour pour un état électrique BAS
{
	if(state != 0 && state != 1)
	{
		fprintf(stderr, "Wrong state value");
		perror("GPIOactive");
		return -1;
	}
	char path[ACTIVE_MAX];
	char buffer[BUF_SIZE];
	uint8_t bytes_written;
	int fd;
	snprintf(path, ACTIVE_MAX, "/sys/class/gpio/gpio%d/active_low", pin);
	fd = open(path, O_WRONLY);
	if (fd ==-1) {
		perror("GPIOactive test, open");
		printf("Chemin du fichier : %s\n",path);
		return -1;
	}
 
	bytes_written = snprintf(buffer, BUF_SIZE, "%d", state);
	write(fd, buffer, bytes_written);
	close(fd);
	return 0;
}

/*int main()
{
	
	GPIOuse(17);
	GPIOdirection(17,IN);
	GPIOactive(17,LOW);

	int i;
	int r;
	for(i=0;i<300;i++)
	{
		r = GPIOread(17);
		printf("%d\n",r);
		usleep(100000);
	}

	GPIOrelease(17);
	return 0;
}*/
