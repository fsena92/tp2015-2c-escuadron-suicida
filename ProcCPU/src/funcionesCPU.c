/*
 * funcionesCPU.c
 *
 *  Created on: 23/9/2015
 *      Author: utnso
 */

#include <commons/error.h>
#include "estructuras.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "logueo.h"
#include "serializacion.h"
#include <pthread.h>
#include <stdbool.h>

void enviarAMemoria(protocolo_cpu_memoria* message) {
	int tamanio;
	void* empaquetado = serializarPaqueteMemoria(message, &tamanio);
	send(socketMemoria, empaquetado, tamanio, 0);
	free(empaquetado);
}

void actualizarOperacionPaquetePlanificador(protocolo_planificador_cpu* paquete, char tipoOperacion){
	paquete->tipoOperacion = tipoOperacion;
}

void actualizarOperacionPaquetePlanificadorIO(protocolo_planificador_cpu* paquete, char tipoOperacion, int IO){
	paquete->tipoOperacion = tipoOperacion;
	paquete->mensaje =malloc(sizeof(int));
	strcpy(paquete->mensaje,string_itoa(IO));
}

void enviarAPlanificador(protocolo_planificador_cpu* respuestaDeMemo,int socketPlanificador){

	int tamanio;
	void* empaquetado = serializarPaquetePlanificador(respuestaDeMemo,&tamanio);
	send(socketPlanificador, empaquetado, tamanio, 0);
	free(empaquetado);
}
//MODIFICAR ARMAR PAQUETE PARAMETROS
void armarPaqueteMemoria(protocolo_cpu_memoria* paquete,char codOperacion, int pid, int nroPagina, char* mensaje) {
	paquete->tipoOperacion = codOperacion;
	paquete->pid = pid;
	paquete->nroPagina = nroPagina;
	paquete->tamanioMensaje = strlen(mensaje) + 1;
	paquete->mensaje = malloc(paquete->tamanioMensaje);
	strcpy(paquete->mensaje, mensaje);
}

bool interpretarInstruccion(char* instruccion, protocolo_planificador_cpu* mensajeDePlanificador,protocolo_cpu_memoria* mensajeParaArmar,int socketPlanificador,t_log* logCpu) {

		char** linea = string_split(instruccion, ";");
		char** lineaFiltrada = string_split(linea[0]," ");
		free(*linea);
		bool entendio = false;

		if (string_starts_with(instruccion, "iniciar")) {
			int numero = atoi(lineaFiltrada[1]);
			armarPaqueteMemoria(mensajeParaArmar, 'i',mensajeDePlanificador->pid,numero , "-");
			entendio = true;
		}
		if (string_starts_with(instruccion, "leer")) {
			int numero = atoi(lineaFiltrada[1]);
			armarPaqueteMemoria(mensajeParaArmar, 'l',mensajeDePlanificador->pid, numero, "-");
			entendio = true;
		}
		if(string_starts_with(instruccion,"escribir")) {

			char * str = lineaFiltrada[2];
			int lng = strlen(str);
			char * aux = malloc(lng);
			int i;
			for(i = 1; i < lng; i++){
				if (str[i] == '\"'){
					aux[i-1] = '\0';
					break;
				}
				aux[i-1] = str[i];
			}

			int numero = atoi(lineaFiltrada[1]);
			armarPaqueteMemoria(mensajeParaArmar, 'e',mensajeDePlanificador->pid, numero,aux);
			entendio = true;

			free(aux);
		}
		if(string_starts_with(instruccion,"entrada-salida")) {
			int tiempo = atoi(lineaFiltrada[1]);
			actualizarOperacionPaquetePlanificadorIO(mensajeDePlanificador,'e',tiempo);
            enviarAPlanificador(mensajeDePlanificador,socketPlanificador);
            loguearPlanificadorIO(mensajeDePlanificador, tiempo,logCpu);
            entendio = true;
		}
		if (string_starts_with(instruccion, "finalizar;")) {
				armarPaqueteMemoria(mensajeParaArmar, 'f', mensajeDePlanificador->pid, 0, "-");
				entendio = true;
		}
		free(*lineaFiltrada);
		return entendio;
}

/*Lee del archivo la linea indicada por el Instruction Pointer*/
char* leerInstruccion(int* instructionPointer, FILE* archivo, int tam) {
	fseek(archivo,0,SEEK_SET);
	char* lineaLeida = malloc(tam);
	int cont = 1;

	while (!feof(archivo) && cont <= (*instructionPointer) ) {
		fgets(lineaLeida, tam, archivo);
		cont++;
	}

	(*instructionPointer) = (*instructionPointer) + 1;

	if (!string_starts_with(lineaLeida, "finalizar;")) lineaLeida[strlen(lineaLeida)-1] = '\0';

	return lineaLeida;

}
