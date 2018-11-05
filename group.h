#include <stdlib.h>
#include "talker.h"
#define MAXGID 10

typedef struct group
{
	//atributos
	struct talker** miembros;
	int cantidadMiembros;
	char id[MAXGID];
	//metodos

	
}group;

struct group* Group(char* id, int cantidadMiembros, struct talker** miembros){
	struct group* grupo=(struct group*)malloc(sizeof(struct group));
	grupo->miembros=miembros;
	grupo->cantidadMiembros=cantidadMiembros;
	strcpy(grupo->id,id);
	return grupo;
}
