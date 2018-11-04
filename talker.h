#include <stdio.h>
#define maxline 10


struct talker
{
	int id;
	int pid;
	struct talker** amigos;
	int cantidadAmigos;
	char* nomPipe;
	int conectado;
}talker;


struct talker* Talker(int id, int pid, int cantMaxAmigos,char* nomPipe){
	struct talker* usuario=(struct talker*)malloc(sizeof(struct talker));
	usuario->id=id;
	usuario->pid=pid;
	usuario->amigos=(struct talker**)malloc(cantMaxAmigos*sizeof(struct talker*));
	usuario->nomPipe=(char*)malloc(maxline*sizeof(char));
	strcpy(usuario->nomPipe,nomPipe);
	printf("NOBRE PIPE %s\n", usuario->nomPipe);
	usuario->conectado=1;
	return usuario;

}