#include "request.h"
#include "reply.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
int lectura;

void crearPipeTalker(char* nomPipe);

int validarArgumentos(int argc, char* argv[]);

void signalHandler ()
{
  char* mensaje=(char*)malloc(MAXCONT*sizeof(char));
  //printf("entra a manejador\n");
  int flag=read(lectura,mensaje,MAXCONT*sizeof(char));
  printf("\n>------------------------<\n");
  printf("%s\n", mensaje);
  printf("^------------------------^\n");
  printf("Ingrese la operacion(Help):\n");
}

int main(int argc, char *argv[])
{
	/*Habilitar recepcion de señales*/
  signal(SIGUSR1,signalHandler);
  /*Validar los argumentos recibidos*/
	if(!validarArgumentos(argc,argv)){
		printf("\nUsage: talker ID pipeNom\n");
    printf("ID: identificacion del usuario\n");
    printf("pipeNom: nombre del pipe nominal\n\n");
    perror("Se cancela la ejecución");
    exit(1);
	}
	/*Conversion de argumentos para luego ser utilizados*/
	char* nomPipeDeLectura=argv[1];
	int myId=atoi(argv[1]);
	char* nomPipe=argv[2];
	
	/*Conexión a PIPE en modo escritura*/
  	int escritura, flag;
  	int creado=0;
  	do {
    	escritura = open (nomPipe,O_WRONLY|O_NONBLOCK);
    	if (escritura == -1) {
        	perror("pipe");
        	//printf(" Se volvera a intentar despues\n");
        	printf("no se encontró el pipe\n");//-------------------
        	//sleep(5);
        	exit(1);//--------------------------------------
    	} else creado = 1;
  	} while (creado == 0);
    /*Creación del pipe por donde el manager retornará las respuestas*/
    crearPipeTalker(nomPipeDeLectura);
  	/*Envio al PIPE principal la solicitud de conexion*/
  	//printf("pipe como string %s\n", nomPipeDeLectura);//bandera
  	struct request* solicitud=Request(myId,getpid(),nomPipeDeLectura,0);
  	//printf("despues de guardar en request %s\n", solicitud->argumentos);//bandera
    //printf("antes de escribir\n");//bandera
  	flag=write(escritura,solicitud,sizeof(struct request));
    //printf("despues de escribir\n");//bandera
      
  	/*Conexion al PIPE de recepcion de respuestas*/ 
  	
  	creado=0;
  	do {
    	lectura = open (nomPipeDeLectura, O_RDONLY);
    	if (lectura == -1) {
        	perror("pipe");
        	printf(" Se volvera a intentar despues\n");
        	sleep(1);
    	} else creado = 1;
  	} while (creado == 0);
  	/*Lectura del PIPE secundario a la respuesta del envio anterior*/
  	struct reply* respuesta=(struct reply*)malloc(sizeof(struct reply));
  	flag=read(lectura,respuesta,sizeof(struct reply));
    printf(">------------------------<\n");
  	printf("%s\n", respuesta->contenido);//bandera
    printf("^------------------------^\n");
    /*Validar si el talker continuará en ejecución o no*/
    int salir;
    //printf("eliminacionDePipe %d\n", respuesta->eliminacionDePipe);
    if(respuesta->validacionDePeticion==0){
      salir=0;
    }else{
      salir=1;
    }
    free(respuesta);
    /*Ciclo de ejecucion del Talker*/
    char* cadena_operacion;
    while(salir){
      cadena_operacion = (char*)malloc(MAXARG*sizeof(char));
      printf("Ingrese la operacion(Help):\n");
      fgets (cadena_operacion, MAXARG, stdin);
      cadena_operacion[strcspn(cadena_operacion,"\n")]=0;
      if(strcmp(cadena_operacion,"Help")==0){
        printf(">------------------------<\n");
        printf("1.List\n");
        printf("2.List friends\n");
        printf("3.List GID\n");
        printf("4.Rel IDi\n");
        printf("5.Group ID1, ID2, ..., IDN\n");
        printf("6.Sent msg IDi\n");
        printf("7.Sent msg GroupID\n");
        printf("8.Salir\n");
        printf("^------------------------^\n");
      }else{
        //printf("cadena_operacion: %s\n", cadena_operacion);
        solicitud = Request(myId,getpid(),cadena_operacion,1);
        flag = write(escritura,solicitud,sizeof(struct request));
        respuesta=(struct reply*)malloc(sizeof(struct reply));
        flag=read(lectura,respuesta,sizeof(struct reply));
        printf(">------------------------<\n");
        printf("%s\n", respuesta->contenido);
        printf("^------------------------^\n");
        free(cadena_operacion);
        if(respuesta->eliminacionDePipe){
          salir=0;
        }
      }   
    }
    /*Cerrar el flujo de lectura y escritura*/
    close(escritura); 
    close(lectura);
	return 0;
}

int validarArgumentos(int argc, char* argv[]){
	/*AQUI SE DEBEN VALIDAR LOS ARGUMENTOS E IMPRIMIR CUAL ES EL ERROR EN ESTOS(SI ES QUE LOS HAY)*/
  if (argc != 3)
  {
    printf("La cantidad de argumentos es invalida.\n");
    return 0;
  }
  else if (atoi(argv[1])==0 || atoi(argv[2]) != 0)
  {
    printf("Los argumentos no cumplen con las condiciones solicitadas.\n");
    return 0;
  }
  return 1;
}

void crearPipeTalker(char* nomPipe){
  mode_t fifo_mode = S_IRUSR | S_IWUSR;
    //unlink(nomPipe); // por si ya existe
    if (mkfifo (nomPipe, fifo_mode) == -1) {
      perror("mkfifo");//QUITAR ESTO DESPUES
      //exit(1);
    }
}
