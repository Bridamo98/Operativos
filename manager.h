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
#define MAXWORDS 2000

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
		if (tamanoVectorArgumentos >= 3){
			if(atoi(vectorArgumentos[tamanoVectorArgumentos-1])!=0){
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
void enviarMensajeAsincrono(manager* gestor,int idDest,char* contenido){
	kill(gestor->usuariosRegistrados[idDest-1]->pid,SIGUSR1);
	sleep(1);
	int creado=0, escritura;
  	do {
    	escritura = open (gestor->usuariosRegistrados[idDest-1]->nomPipe, O_WRONLY|O_NONBLOCK);
    	if (escritura == -1) {
        perror("pipe");
        printf(" Se volvera a intentar despues\n");
        //printf("PIPE %s\n",respuesta->nomPipe);
        sleep(1);
    	} else creado = 1;
  } while (creado == 0);
  write(escritura,contenido,MAXCONT*sizeof(char));

  
}
void enviarMensajeAsincronoAGrupo(manager* gestor,int myId,struct talker** miembros,int cantidadMiembros,char* contenido){
	for (int i = 0; i < cantidadMiembros; ++i)
	{
		if(miembros[i]->id!=myId){
			kill(miembros[i]->pid,SIGUSR1);
		}
	}
	sleep(1);
	int escritura;
	for (int i = 0; i < cantidadMiembros; ++i)
	{
		if(miembros[i]->id!=myId){
			escritura = open (miembros[i]->nomPipe, O_WRONLY|O_NONBLOCK);
			write(escritura,contenido,MAXCONT*sizeof(char));
		}
	}
}
void enviarMensajeAsincronoAGrupoDiferente(manager* gestor,int myId,struct talker** miembros,int cantidadMiembros,char** mensajes){
	for (int i = 0; i < cantidadMiembros; ++i)
	{
		if(miembros[i]->id!=myId){
			kill(miembros[i]->pid,SIGUSR1);
			printf("envia senal \n");
		}

	}
	sleep(1);
	int escritura;
	int contadorMensajes=0;
	for (int i = 0; i < cantidadMiembros; ++i)
	{
		if(miembros[i]->id!=myId){
			escritura = open (miembros[i]->nomPipe, O_WRONLY|O_NONBLOCK);
			write(escritura,mensajes[contadorMensajes],MAXCONT*sizeof(char));
			contadorMensajes++;
			printf("envia mensaje \n");
		}
	}
	printf("no se traba\n");
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

		free(contenido);
		return NULL;
	}
	return contenido;
}
void cerrarSesion(manager* gestor,int myId){
	gestor->usuariosRegistrados[myId-1]->conectado=0;
}
char* agregarAmigo(manager* gestor,int myId,char** vectorArgumentos){
	int idAmigo=atoi(vectorArgumentos[1]);
	char* contenido=(char*)malloc(MAXCONT*sizeof(char));
	
	if(myId==idAmigo){
		free(contenido);
		return "No es necesario hacer esto";
	}
	if(gestor->usuariosRegistrados[idAmigo-1]!=NULL){
		if(gestor->usuariosRegistrados[myId-1]->amigos[idAmigo-1]==NULL){
			if(gestor->usuariosRegistrados[idAmigo-1]->conectado==1){
			gestor->usuariosRegistrados[myId-1]->amigos[idAmigo-1]=gestor->usuariosRegistrados[idAmigo-1];
			gestor->usuariosRegistrados[myId-1]->cantidadAmigos++;
			printf(" CANTIDAD AMIGOS ACTUAL yo %d\n", gestor->usuariosRegistrados[myId-1]->cantidadAmigos);

			gestor->usuariosRegistrados[idAmigo-1]->amigos[myId-1]=gestor->usuariosRegistrados[myId-1];
			gestor->usuariosRegistrados[idAmigo-1]->cantidadAmigos++;
			printf(" CANTIDAD AMIGOS ACTUAL otro %d\n", gestor->usuariosRegistrados[idAmigo-1]->cantidadAmigos);

			strcat(contenido,"Se establece una relacion entre los Talkers ");
			char idComoString[5];
			sprintf(idComoString,"%d",myId);
			strcat(contenido,idComoString);
			strcat(contenido," y ");
			strcat(contenido,vectorArgumentos[1]);
			enviarMensajeAsincrono(gestor,idAmigo,contenido);
			return contenido;
			}else{
				free(contenido);
				return "El usuario especificado no se encuentra conectado";
			}	
		}else{
			free(contenido);
			return "El usuario especificado ya fue agregado";
		}
		
	}else{
		free(contenido);
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

		free(contenido);
		free(indicesAmigos);
		return NULL;
	}
	return contenido;
}
char* crearGrupo(manager* gestor, char** vectorArgumentos, int myId, int tamanoVectorArgumentos){
	struct  talker** auxMiembrosGrupo=(struct talker**)malloc((tamanoVectorArgumentos)*sizeof(struct talker*));
	char* contenido=(char*)malloc(MAXCONT*sizeof(char));

	char** mensajes=(char**)malloc((tamanoVectorArgumentos-1)*sizeof(char*));


	char idComoString[5];
	sprintf(idComoString,"%d",gestor->cantidadGrupos);

	char myIdComoString[5];
	sprintf(myIdComoString,"%d",myId);

	strcat(contenido,"Fue creado el grupo con Id G");
	strcat(contenido, idComoString);
	strcat(contenido," con integrantes: ");
	strcat(contenido, myIdComoString);
	int idMiembro;
	int encontrado=0;
	auxMiembrosGrupo[0]=gestor->usuariosRegistrados[myId-1];

	char* id=(char*)malloc(MAXWORDS*sizeof(char));
	strcat(id,"G");
	strcat(id,idComoString);

	if(tamanoVectorArgumentos-1>gestor->usuariosRegistrados[myId-1]->cantidadAmigos){
		free(auxMiembrosGrupo);
		free(contenido);
		return NULL;
	}
	for (int i = 1; i < tamanoVectorArgumentos; ++i)
	{
		idMiembro=atoi(vectorArgumentos[i]);
		if(idMiembro<1){
			free(auxMiembrosGrupo);
			free(contenido);
			return NULL;
		}
		if(gestor->usuariosRegistrados[myId-1]->amigos[idMiembro-1]==NULL){
			free(auxMiembrosGrupo);
			free(contenido);
			return NULL;
		}else{

			if(idMiembro!=myId){
				strcat(contenido,", ");
				auxMiembrosGrupo[i]=gestor->usuariosRegistrados[idMiembro-1];
				char idComoString2[5];
				sprintf(idComoString2,"%d",idMiembro);
				strcat(contenido, idComoString2);

				char* mensaje=(char*)malloc(MAXCONT*sizeof(char));
				strcat(mensaje,"Talker ");
				strcat(mensaje,idComoString2);
				strcat(mensaje," forma parte del grupo ");
				strcat(mensaje,id);
				mensajes[i-1]=mensaje;
				
			}
		}
	}
	
	struct group* nuevoGrupo=Group(id,tamanoVectorArgumentos,auxMiembrosGrupo);
	enviarMensajeAsincronoAGrupoDiferente(gestor,myId,auxMiembrosGrupo,tamanoVectorArgumentos,mensajes);
	gestor->grupos[gestor->cantidadGrupos]=nuevoGrupo;
	gestor->cantidadGrupos++;

	return contenido;
}
char* listarGrupo(manager* gestor,int myId,char** vectorArgumentos){

	char* contenido=(char*)malloc(MAXCONT*sizeof(char));
	strcat(contenido,"Los integrantes del grupo ");
	strcat(contenido, vectorArgumentos[1]);
	strcat(contenido, "son: ");
	int encontrado=0;
	if(gestor->cantidadGrupos==0){
		free(contenido);
		return "No hay grupos registrados actualmente";
	}
	for (int i = 0; i < gestor->cantidadGrupos; ++i)
	{
		printf("en gestor %s y en vector %s\n",gestor->grupos[i]->id, vectorArgumentos[1]);
		if(strcmp(gestor->grupos[i]->id,vectorArgumentos[1])==0){
			printf("se encontroooooooo\n");

			for (int j = 0; j < gestor->grupos[i]->cantidadMiembros; ++j)
			{
				char myIdComoString[5];
				sprintf(myIdComoString,"%d",gestor->grupos[i]->miembros[j]->id);
				if(encontrado){
					strcat(contenido, ", ");
				}
				encontrado=1;
				strcat(contenido,myIdComoString);
			}
			return contenido;

		}
	}
	free(contenido);
	return NULL;
	
}
char* enviarMensajeAmigo(manager* gestor, int myId,char** vectorArgumentos, int tamanoVectorArgumentos){
	int idAmigo=atoi(vectorArgumentos[tamanoVectorArgumentos-1]);
	char* contenido=(char*)malloc(MAXCONT*sizeof(char));
	strcat(contenido,"Talker ");
	char myIdComoString[5];
	sprintf(myIdComoString,"%d",myId);
	strcat(contenido,myIdComoString);
	strcat(contenido, " envia: ");
	for (int i = 1; i < tamanoVectorArgumentos-1; ++i)
	{
		strcat(contenido,vectorArgumentos[i]);
		strcat(contenido," ");
	}
	if(gestor->usuariosRegistrados[myId-1]->amigos[idAmigo-1]==NULL){
		free(contenido);
		return NULL;
	}else if(gestor->usuariosRegistrados[myId-1]->amigos[idAmigo-1]->conectado==0){
		free(contenido);
		return "El usuario especificado no se encuentra conectado en este momento";
	}else{
		enviarMensajeAsincrono(gestor, idAmigo, contenido);
		return "El mensaje ha sido enviado satisfactoriamente";
	}
}
char* enviarMensajeGrupo(manager* gestor, int myId,char** vectorArgumentos, int tamanoVectorArgumentos){
	char* idGrupo=vectorArgumentos[tamanoVectorArgumentos-1];
	char* contenido=(char*)malloc(MAXCONT*sizeof(char));
	for (int i = 1; i < tamanoVectorArgumentos-1; ++i)
	{
		strcat(contenido,vectorArgumentos[i]);
		strcat(contenido," ");
	}
	strcat(contenido," viene de ");
	strcat(contenido, idGrupo);
	int pertenece=0;
	for (int i = 0; i < gestor->cantidadGrupos; ++i)
	{
		if(strcmp(gestor->grupos[i]->id,idGrupo)==0){

			for (int j = 0; j < gestor->grupos[i]->cantidadMiembros; ++j)
			{
				if(gestor->grupos[i]->miembros[j]->id==myId){
					pertenece=1;
				}
			}
			if(pertenece){

				enviarMensajeAsincronoAGrupo(gestor,myId,gestor->grupos[i]->miembros,gestor->grupos[i]->cantidadMiembros,contenido);
				return contenido;

			}else{

				return "No puede hacer esto ya que no pertenece a este grupo";	
			}
			
		}
	}
	return NULL;

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
		char*  contenido = listarGrupo(gestor,solicitud->myId,vectorArgumentos);
		if(contenido==NULL){
			return Reply("No se encontro el grupo especificado",0,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,0);
		}else{
			return Reply(contenido,1,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,0);
		}
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
		char* contenido=enviarMensajeAmigo(gestor, solicitud->myId,vectorArgumentos, tamanoVectorArgumentos);
		if(contenido==NULL){
			return Reply("El usuario especificado no es amigo",0,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,0);
		}else{
			return Reply(contenido,0,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,0);
		}
		
	}
	if(operacion==7){
		char* contenido=enviarMensajeGrupo(gestor, solicitud->myId,vectorArgumentos, tamanoVectorArgumentos);
		if(contenido==NULL){
			return Reply("El grupo especificado no existe",0,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,0);
		}else{
			return Reply(contenido,0,gestor->usuariosRegistrados[solicitud->myId-1]->nomPipe,0);
		}
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
