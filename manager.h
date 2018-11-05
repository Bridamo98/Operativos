#include "request.h"
#include "reply.h"
#include "group.h"
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#define MAXWORDS 20

typedef struct manager
{
	//atributos
	struct talker** usuariosRegistrados;
	int cantidadRegistrados;
	struct group** grupos;
	int cantidadGrupos;
	char* nomPipe;
	int maximoDeUsuarios;

}manager;

//----------------------------------METODOS-------------------------------------------------

/* COSTRUCTOR*/
manager* Manager(int maximoDeUsuarios, char* nomPipe){
	manager* gestor=(manager*)malloc(sizeof(manager));
	gestor->usuariosRegistrados=(struct talker**)malloc(maximoDeUsuarios*sizeof(struct talker*));
	gestor->cantidadRegistrados=0;
	gestor->cantidadGrupos=0;
	gestor->grupos=(struct group**)malloc(maximoDeUsuarios*sizeof(struct group*));
	gestor->maximoDeUsuarios=maximoDeUsuarios;
	for (int i = 0; i < maximoDeUsuarios; ++i)
	{
		gestor->usuariosRegistrados[i]=NULL;
	}
	gestor->nomPipe=(char*)malloc(MAXWORDS*sizeof(char));
	strcpy(gestor->nomPipe,nomPipe);
	return gestor;
}
/*Se obtiene el apuntador del talker con el id dado*/
struct talker* buscarTalker(struct manager* gestor,int id){//Esto es opcional(LA BUSQUEDA PUEDE SER DIRECTA)
	struct talker* usuarioAux=NULL;
	for (int i = 0; i < gestor->cantidadRegistrados; ++i)
	{
		if(gestor->usuariosRegistrados[i]->id==id){
			usuarioAux=gestor->usuariosRegistrados[i];
		}
	}
	return usuarioAux;
}
/*Para crear un PIPE dado el nombre*/
void crearPipeManager(char* nomPipe){
	mode_t fifo_mode = S_IRUSR | S_IWUSR;
  	unlink(nomPipe); // por si ya existe
  	if (mkfifo (nomPipe, fifo_mode) == -1) {
    	perror("mkfifo");
    	exit(1);
  	}
}

/*Separa los parametros de la solicitud por espacios para luego procesar la solicitud*/
char** separar_cadena(struct request* solicitud, int* tamanoVectorArgumentos)
{
	*tamanoVectorArgumentos = 0;
	char** vectorArgumentos = (char**)malloc(MAXWORDS*sizeof(char*));
	char* argumentos = solicitud->argumentos;
	argumentos[strcspn(argumentos,"\n")]=0;
	const char s[2] = " ";
    char *token;
    token = strtok(argumentos, s);
    while( token != NULL ) {
    	vectorArgumentos[*tamanoVectorArgumentos] = token;
        *tamanoVectorArgumentos = *tamanoVectorArgumentos +1;
        token = strtok(NULL, s);
   }
   return vectorArgumentos;

}
/*Obtiene el numero de operacion que el talker desea realizar(ver la estructura REQUEST)*/
int obtenerNumeroDeOperacion(char** vectorArgumentos,int tamanoVectorArgumentos){
	//printf("Entra %s\n", vectorArgumentos[1]);
	if (strcmp(vectorArgumentos[0],"List") == 0)
	{
		printf("Se cumplio-> List\n");
		if (tamanoVectorArgumentos==1){
			return 1;
		}
		else if(tamanoVectorArgumentos==2){
			if(strcmp(vectorArgumentos[1],"friends") == 0){
				return 2;
			}else{
				return 3;
			}
		}else{
			return 0;
		}
	}
	if (strcmp(vectorArgumentos[0],"Sent") == 0)
	{
		printf("Se cumplio-> Sent\n");
		if (tamanoVectorArgumentos==3){
			if(atoi(vectorArgumentos[2])!=0){
				return 6;
			}else{
				return 7;
			}
		}
		else{
			return 0;
		}

	}
	if (strcmp(vectorArgumentos[0],"Rel") == 0)
	{
		printf("Se cumplio-> Rel\n");
		if (tamanoVectorArgumentos==2){
			return 4;
		}
		else{
			return 0;
		}

	}
	if (strcmp(vectorArgumentos[0],"Group") == 0)
	{
		printf("Se cumplio-> Group\n");
		if (tamanoVectorArgumentos>1){
			return 5;
		}
		else{
			return 0;
		}

	}
	if (strcmp(vectorArgumentos[0],"Salir") == 0)
	{
		printf("Se cumplio-> Salir\n");
		if (tamanoVectorArgumentos==1){
			return 8;
		}
		else{
			return 0;
		}
	}
	printf("No se cumplio nada\n");
	return 0;
}

char* lista_usuarios_conectados(manager* gestor, int myId)
{
	char* contenido = (char*)malloc(MAXCONT*sizeof(char));
	//contenido = "Los usuario actualmente conectados en el sistema son: ";
	struct talker** talkers = gestor->usuariosRegistrados;
	printf ("tamaño %d\n", gestor->cantidadRegistrados);
	int tam = gestor->maximoDeUsuarios;
	char** indicesDeConectados=(char**)malloc(tam*sizeof(char*));
	int encontrado=0;
	for (int i = 0; i < tam; i++)
	{
		struct talker* t = talkers[i];
		if(t!=NULL){

			printf("No es nulo!\n");
			if (t->conectado == 1 && t->id != myId)
			{
				
				printf("entra a if\n" );
				if(encontrado){
					strcat(contenido, ", ");
				}else{
					strcat(contenido,"Los usuario actualmente conectados en el sistema son: ");
				}
				encontrado=1;
				char idComoString[5];
				sprintf(idComoString,"%d",t->id);
				//indicesDeConectados[contadorConectados]=idComoString;
				printf("id como string %s\n", idComoString);
				//contadorConectados++;
				strcat(contenido, idComoString);
				
				/*if(i!=tam-1){
					strcat(contenido, ", ");
				}*/
				//printf("carajooo %s\n", contenido);
			}
		}else{
			printf("Es nulo!\n");
		}

	}
	printf(" resultado %s\n", contenido);
	if(encontrado==0){

		return NULL;
	}
	return contenido;
}
void cerrarSesion(manager* gestor,int myId){
	gestor->usuariosRegistrados[myId-1]->conectado=0;
}
char* agregarAmigo(manager* gestor,int myId,char** vectorArgumentos){
	int idAmigo=atoi(vectorArgumentos[1]);
	if(gestor->usuariosRegistrados[idAmigo-1]!=NULL){
		if(gestor->usuariosRegistrados[myId-1]->amigos[idAmigo-1]==NULL){
			if(gestor->usuariosRegistrados[idAmigo-1]->conectado==1){
			gestor->usuariosRegistrados[myId-1]->amigos[idAmigo-1]=gestor->usuariosRegistrados[idAmigo-1];
			gestor->usuariosRegistrados[myId-1]->cantidadAmigos++;
			printf(" CANTIDAD AMIGOS ACTUAL yo %d\n", gestor->usuariosRegistrados[myId-1]->cantidadAmigos);

			gestor->usuariosRegistrados[idAmigo-1]->amigos[myId-1]=gestor->usuariosRegistrados[myId-1];
			gestor->usuariosRegistrados[idAmigo-1]->cantidadAmigos++;
			printf(" CANTIDAD AMIGOS ACTUAL otro %d\n", gestor->usuariosRegistrados[idAmigo-1]->cantidadAmigos);
			return "Se establecio la relacion";
			}else{
				return "El usuario especificado no se encuentra conectado";
			}	
		}else{
			return "El usuario especificado ya fue agregado";
		}
		
	}else{
		return NULL;
	}
}
char* obtenerAmigos(manager* gestor,int myId){
	char* contenido = (char*)malloc(MAXCONT*sizeof(char));
	//contenido = "Los usuario actualmente conectados en el sistema son: ";
	struct talker** amigos = gestor->usuariosRegistrados[myId-1]->amigos;
	//printf ("tamaño %d\n", gestor->cantidadRegistrados);
	int tam = gestor->usuariosRegistrados[myId-1]->cantMaxAmigos;
	char** indicesAmigos=(char**)malloc(tam*sizeof(char*));
	int encontrado=0;
	for (int i = 0; i < tam; i++)
	{
		struct talker* t = amigos[i];
		if(t!=NULL){

			printf("entra a if\n" );
			if(encontrado){
				strcat(contenido, ", ");
			}else{
				strcat(contenido,"Actualmente mis amigos son: ");
			}
			encontrado=1;
			char idComoString[5];
			sprintf(idComoString,"%d",t->id);
			//indicesDeConectados[contadorConectados]=idComoString;
			printf("id como string %s\n", idComoString);
			//contadorConectados++;
			strcat(contenido, idComoString);
		}
	}
	printf(" resultado %s\n", contenido);
	if(encontrado==0){

		return NULL;
	}
	return contenido;
}
char* crearGrupo(manager* gestor, char** vectorArgumentos, int myId, int tamanoVectorArgumentos){
	struct  talker** auxMiembrosGrupo=(struct talker**)malloc((tamanoVectorArgumentos-1)*sizeof(struct talker*));
	char* contenido=(char*)malloc(MAXCONT*sizeof(char));

	char idComoString[5];
	sprintf(idComoString,"%d",gestor->cantidadGrupos);

	strcat(contenido,"Fue creado el grupo con Id G");
	strcat(contenido, idComoString);
	strcat(contenido," e integrantes: ");
	int idMiembro;
	int encontrado=0;
	if(tamanoVectorArgumentos-1!=gestor->usuariosRegistrados[myId-1]->cantidadAmigos){
		return NULL;
	}
	for (int i = 1; i < tamanoVectorArgumentos; ++i)
	{
		idMiembro=atoi(vectorArgumentos[i]);
		if(idMiembro==0){
			return NULL;
		}
		if(gestor->usuariosRegistrados[myId-1]->amigos[idMiembro-1]==NULL){
			return NULL;
		}else{
			if(i!=1){
				strcat(contenido,", ");
			}
			auxMiembrosGrupo[i-1]=gestor->usuariosRegistrados[idMiembro-1];
			char idComoString2[5];
			sprintf(idComoString2,"%d",idMiembro);
			strcat(contenido, idComoString2);
		}
	}
	char* id=(char*)malloc(MAXWORDS*sizeof(char));
	strcat(id,"G");
	strcat(id,idComoString);
	struct group* nuevoGrupo=Group(id,tamanoVectorArgumentos-1,auxMiembrosGrupo);
	gestor->grupos[gestor->cantidadGrupos]=nuevoGrupo;
	gestor->cantidadGrupos++;

	return contenido;
}
struct reply* procesar_operacion(int operacion, char** vectorArgumentos, struct request* solicitud, manager* gestor, int tamanoVectorArgumentos)
{
	if (operacion == 1)
	{
		char* contenido = lista_usuarios_conectados(gestor,solicitud->myId);
		if(contenido==NULL){
			printf("NOM PIPE%s\n", gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe);
			return Reply("No hay usuarios conectados",1,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,0);
		}else{
			return Reply(contenido,1,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,0);
		}
	}
	if(operacion==2){
		char* contenido = obtenerAmigos(gestor,solicitud->myId);
		if(contenido==NULL){
			return Reply("En el momento no tengo amigos",1,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,0);
		}else{
			return Reply(contenido,1,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,0);
		}
	}
	if(operacion==3){
		
	}
	if(operacion==4){
		char* contenido = agregarAmigo(gestor,solicitud->myId,vectorArgumentos);
		if(contenido==NULL){
			return Reply("El usuario especificado no se encuentra en el sistema",0,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,0);
		}else{
			return Reply(contenido,0,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,0);
		}
	}
	if(operacion==5){
		char* contenido= crearGrupo(gestor, vectorArgumentos, solicitud->myId, tamanoVectorArgumentos);
		if(contenido==NULL){
			return Reply("El grupo no pudo ser creado",0,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,0);
		}else{
			return Reply(contenido,0,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,0);
		}
	}
	if(operacion==6){
		
	}
	if(operacion==7){
		
	}
	if(operacion==8){
		cerrarSesion(gestor,solicitud->myId);
		return Reply("Se ha cerrado la sesion",0,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,1);
	}
	
}

/*Se genera la respuesta a la solicitud dada*/
struct reply* procesarSolicitud(struct manager* gestor,struct request* solicitud){
	struct reply* respuesta = (struct reply*)malloc(sizeof(struct reply));


	if(solicitud->tipo==0){//INICIO DE SESION (O REGISTRO) DE UN TALKER
		printf("inicio de sesion o registro\n");
		if(solicitud->myId>=1 && solicitud->myId<=gestor->maximoDeUsuarios){//validar que el id se encuentre en el rango permitido
			printf("el rango es el permitido\n");
			//struct talker* usuarioAux=buscarTalker(gestor,solicitud->myId); //PUEDE HACERCE DIRECTAMENTE
			if(gestor->usuariosRegistrados[solicitud->myId-1]==NULL){//EL talker no ha sido registrado en el sistema
				gestor->usuariosRegistrados[solicitud->myId-1]=Talker(solicitud->myId,solicitud->myPid,gestor->maximoDeUsuarios-1,solicitud->argumentos);
				gestor->cantidadRegistrados++;
				printf("el talker no ha sido registrado\n");
				printf("argumentos %s\n", solicitud->argumentos);
				respuesta=Reply("El usuario ha sido registrado",1,solicitud->argumentos,0);
				printf("aqui\n");
				printf("el nombre del pipe resulta siendo %s\n", respuesta->nomPipe);
			}else{//El talker SI esta registrado en el sistema
				printf("el talker ya fue registrado\n");
				if(gestor->usuariosRegistrados[solicitud->myId-1]->conectado==1){//El usuario ya se encuentra conectado
					printf("ya se encuentra conectado\n");
					respuesta=Reply("El usuario ya se encuentra conectado ",0,solicitud->argumentos,0);
				}else{//el usuario no esta conectado
					printf("no se encuentra conectado\n");
					gestor->usuariosRegistrados[solicitud->myId-1]->conectado=1;
					gestor->usuariosRegistrados[solicitud->myId-1]->pid=solicitud->myPid;
					respuesta=Reply("EL usuario cambia su estado a conectado",1,solicitud->argumentos,0);
				}
			}
		}else{//el id ingresado no se encuentra entre el rango permitido
			printf("el rango no es el permitido\n");
			respuesta=Reply("El ID ingresado no se encuentra entre el rango permitido",0,solicitud->argumentos,1);
		}
	}else{//PETICION DE UN TALKER CON SESION YA INICIADA
		printf("Se va a validar la cadena\n");
		printf("MY ID%d\n", gestor->usuariosRegistrados[solicitud->myId-1]->id);
		int* tamanoVectorArgumentos = (int*)malloc(sizeof(int));
		char** vectorArgumentos = separar_cadena(solicitud, tamanoVectorArgumentos);
		printf("tamaño vector %d\n", *tamanoVectorArgumentos);
		int operacion = obtenerNumeroDeOperacion(vectorArgumentos,*tamanoVectorArgumentos);
		if(operacion!=0){
			printf("Operacion %d\n",operacion);
			respuesta = procesar_operacion(operacion, vectorArgumentos, solicitud,gestor, *tamanoVectorArgumentos);
			//nota: probar la funcion anterior y procesar cada operacion segun numero
			printf ("Contenido %s\n", respuesta->contenido);	
		}else{
			respuesta=Reply("El comando ingresado no puede reconocerse",0,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,0);
		}
		

		


	}
	return respuesta;
}
