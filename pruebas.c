
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
int main(int argc, char const *argv[])
{
	char* cadena=(char*)malloc(40*sizeof(char));
	strcat(cadena,"miercoles ");
	strcat(cadena,"jueves ");
	strcat(cadena,"viernes ");
	char idComoString[5];
	int numero=20;
	sprintf(idComoString,"%d",numero);
	strcat(cadena,idComoString);
	printf("resultado: %s\n", cadena);




	return 0;
}