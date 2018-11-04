#include "manager.h"


int validarArgumentos(int argc, char* argv[]);
void enviarRespuesta(struct reply* respuesta);
int main(int argc, char *argv[])
{
	/*Validar los argumentos recibidos*/
	if(!validarArgumentos(argc,argv)){
    printf("\nUsage: manager N pipeNom\n");
    printf("N: es el numero maximo de usuarios\n");
    printf("pipeNom: nombre del pipe nominal\n\n");
		perror("Se cancela la ejecución");
		exit(1);
	}
	/*Conversion de argumentos para luego ser utilizados*/
	int maximoDeUsuarios=atoi(argv[1]);
	char* nomPipe=argv[2];
	/*Creacion del manager*/
	struct manager* gestor=Manager(maximoDeUsuarios,nomPipe);
	/*Creacion del PIPE*/
	crearPipe(nomPipe);
  	/*Conexión a PIPE en modo lectura*/
  	int lectura;
  	int creado=0;
  	do {
    	lectura = open (nomPipe, O_RDONLY);
    	if (lectura == -1) {
        	perror("pipe");
        	printf(" Se volvera a intentar despues\n");
        	sleep(5);
    	} else creado = 1;
  	} while (creado == 0);
  	/*Ciclo que recibe las solicitudes de los talkers*/
  	int flag;
  	struct request* solicitud;
  	do{
    	/*Leer la solicitud de los talkers*/
    	solicitud=(struct request*)malloc(sizeof(struct request));
    	flag=read(lectura,solicitud,sizeof(request));
    	if(flag!=0)
      {
        
        printf("Id %d\n", solicitud->myId);   
        printf("Pid %d\n", solicitud->myPid);
        printf("tipo %d\n", solicitud->tipo);
      
        /*Realizar los cambios necesarios en GESTOR y generar la respuesta de la solicitud anterior*/
        struct reply* respuesta=procesarSolicitud(gestor, solicitud);//aqui se imprime el mensaje que debe aparecer en la consola del manager
        enviarRespuesta(respuesta);
      }
      free(solicitud);
  	}while(1);

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

void enviarRespuesta(struct reply* respuesta) {

	//aqui se debe lanzar la señal al talker para que reciba la respuesta

  	int creado=0, escritura;
  	do {
    	escritura = open (respuesta->nomPipe, O_WRONLY|O_NONBLOCK);
    	if (escritura == -1) {
        perror("pipe");
        printf(" Se volvera a intentar despues\n");
        printf("PIPE %s\n",respuesta->nomPipe);
        sleep(1);
    	} else creado = 1;
  } while (creado == 0);
  write(escritura,respuesta,sizeof(struct reply));
  int flag;
  if(respuesta->eliminacionDePipe){
    //close(escritura);
    flag=remove(respuesta->nomPipe);
  }
}

