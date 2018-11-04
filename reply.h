#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define MAXCONT 2000
#define MAXNOMPIPE 30
typedef struct reply
{
	//atributos
	char contenido[MAXCONT];
	char nomPipe[MAXNOMPIPE];
	int validacionDePeticion;//0=LA PETICION NO PUDO SER PROCESADA DEBIDO A INCONSISTENCIAS --- 1=LA PETICION FUE PROCESADA SATISFACTORIAMENTE
	int eliminacionDePipe;//0=NO SE REQUIERE ELIMINAR EL PIPE --- 1=SI SE REQUIERE ELIMINAR EL PIPE
}reply;

struct reply* Reply(char* contenido, int validacionDePeticion,char* nomPipe,int eliminacionDePipe){
	struct reply* respuesta=(struct reply*)malloc(sizeof(struct reply));
	strcpy(respuesta->contenido,contenido);
	strcpy(respuesta->nomPipe,nomPipe);
	respuesta->validacionDePeticion=validacionDePeticion;
	respuesta->eliminacionDePipe=eliminacionDePipe;
	return (respuesta);
}
