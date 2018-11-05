#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAXARG 30


typedef struct request
{
	//atributos
	int myId;
	int myPid;
	char argumentos[MAXARG];
	int tipo;//0 = inicio de sesion de un talker ------- 1 = peticion de un talker con sesion ya iniciada
	/*
	*1.List ----- hecho
	*2.List friends --- hecho
	*3.List GID
	*4.Rel IDi --hecho
	*5.Group ID1, ID2,..,IDN
	*6.Sent msg IDi
	*7.Sent msg GroupID
	*8.Salir--- hecho
	*/

}request;


struct request* Request(int myId, int myPid, char* argumentos, int tipo){
	struct request* solicitud=(struct request*)malloc(sizeof(struct request));
	solicitud->myId=myId;
	solicitud->myPid=myPid;
	
	//solicitud->argumentos=argumentos;
	strcpy(solicitud->argumentos,argumentos);
	printf("pipe en request %s\n",solicitud->argumentos);
	solicitud->tipo=tipo;
	return (solicitud);
}