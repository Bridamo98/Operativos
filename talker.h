#include <stdio.h>
#define maxline 30


struct talker
{
	int id;
	int pid;
	struct talker** amigos;
	int cantidadAmigos;
	char* nomPipe;
	int conectado;
	int cantMaxAmigos;
}talker;


struct talker* Talker(int id, int pid, int cantMaxAmigos,char* nomPipe){
	struct talker* usuario=(struct talker*)malloc(sizeof(struct talker));
	usuario->id=id;
	usuario->pid=pid;
	usuario->amigos=(struct talker**)malloc(cantMaxAmigos*sizeof(struct talker*));
	for (int i = 0; i < cantMaxAmigos; ++i)
	{
		usuario->amigos[i]=NULL;
	}
	usuario->nomPipe=(char*)malloc(maxline*sizeof(char));
	strcpy(usuario->nomPipe,nomPipe);
	printf("NOBRE PIPE %s\n", usuario->nomPipe);
	usuario->conectado=1;
	usuario->cantidadAmigos=0;
	usuario->cantMaxAmigos=cantMaxAmigos;
	return usuario;

}