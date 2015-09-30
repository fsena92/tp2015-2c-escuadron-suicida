#include "funcionesPlanificador.h"
#include "estructuras.h"
#include "../../lib/libSocket.h"
#include <commons/string.h>

#define PACKAGESIZE 30
sem_t hayProgramas;

void definirMensaje(tpcb* pcb,char* message){
	//char *message=malloc(strlen(pcb[0].ruta)+(2*sizeof(int))+sizeof(testado)+10+1);
	message[0]='p';
	message[1]='c';
	message[2]=pcb[0].estado;
	message[6]=pcb[0].pid;
	message[10]=pcb[0].siguiente;
	message[14]=1;
	//message[18]=sizeof(pcb[0].ruta);
	strcpy(&message[22],pcb[0].ruta);
}

void *enviar(void *arg){
	tpcb* pcb; // che por mas que lo mire supongo que te falto escribir algo este pcb esta al pedo nunca se carga y asigna nada a message
	tParametroEnviar* parametros;
	parametros = (tParametroEnviar*) arg;
	puts("estas en el hilo bien por vos");
	while(1){
		sem_wait(&hayProgramas);
		puts("pasaste el semaforo");
		pcb=queue_pop(parametros->procesos);
		printf("saque pcb\n");
		//char* message=malloc(strlen(pcb[0].ruta)+(2*sizeof(int))+sizeof(testado)+10+1);
		char* message;
		//strcpy(message,"river campeon!!!\n");
		//definirMensaje(pcb,message);
		protocolo_planificador_cpu* package = malloc(sizeof(protocolo_planificador_cpu));
		int tamanio = 0;
		printf("estoy por serializqar\n");
		message = serializarPaqueteCPU(pcb,package, &tamanio);
		printf("serialice\n");
		send(parametros->socket,message,tamanio,0);
		printf("Envie paquete");
		//free(message);
		free(pcb);
	}
}

int main(){
	system("clear");
	int cantProc=1;
	t_queue* colaProcesos;
	pthread_t enviarAlCpu;
	pthread_attr_t attr;
	sem_init(&hayProgramas,0,0);

	//creacion de la instancia de log
	t_log *logPlanificador = log_create("../src/log.txt", "planificador.c", false, LOG_LEVEL_INFO);
	//logPlanificador->pid = 1;

	//leemos el archivo de configuracion
	tconfig_planif *configPlanificador = leerConfiguracion();

	//Inicia el socket para escuchar
	int serverSocket;
	server_init(&serverSocket, configPlanificador->puertoEscucha);
	printf("Planificador listo...\n");

	// loguea el inicio del planificador
	//log_info(logPlanificador, "planificador iniciado");

	//Inicia el socket para atender al CPU
	int socketCPU;
	server_acept(serverSocket, &socketCPU);
	printf("CPU aceptado...\n");

	colaProcesos=queue_create();

	tParametroEnviar envio;
	envio.socket=socketCPU;
	envio.procesos=colaProcesos;

	pthread_attr_init(&attr);
	pthread_create( &enviarAlCpu, &attr, enviar,(void*) &envio);

	int enviar2 = 1;
	char message[PACKAGESIZE];

	int nro_comando=0;

	while(enviar2){
		fgets(message, PACKAGESIZE, stdin);

		nro_comando = clasificarComando(&message[0]);

		procesarComando(nro_comando,&message[0],cantProc,colaProcesos,&hayProgramas);

		nro_comando=0;

		if (!strcmp(message,"exit\n")) enviar2 = 0;

	}



	close(serverSocket);
	close(socketCPU);
	queue_destroy(colaProcesos);
	return 0;
}
