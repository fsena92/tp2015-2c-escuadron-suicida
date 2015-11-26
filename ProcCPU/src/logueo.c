/*
 * logueo.c
 *
 *  Created on: 25/10/2015
 *      Author: utnso
 */

#include "estructuras.h"
#include <stdlib.h>
#include <string.h>
#include <commons/log.h>
#include <commons/string.h>

void logueoRecepcionDePlanif(protocolo_planificador_cpu* contextoDeEjecucion,int tid,t_log* logCpu) {
	char* logueoContexto = string_new();
	char* estado = string_new();
	if (contextoDeEjecucion->estado == LISTO) {
		string_append(&estado, "LISTO");
	}
	if (contextoDeEjecucion->estado == IO) {
		string_append(&estado, "IO");
	}
	if (contextoDeEjecucion->estado == EJECUTANDO) {
		string_append(&estado, "EJECUTANDO");
	}
	if (contextoDeEjecucion->estado == FINALIZADO) {
		string_append(&estado, "FINALIZADO");
	}

	string_append(&logueoContexto, "Contexto de ejecucion recibido: \nPID: ");
	string_append_with_format(&logueoContexto, "%d", contextoDeEjecucion->pid);
	string_append(&logueoContexto, "\nInstruccion: ");
	string_append_with_format(&logueoContexto, "%d", contextoDeEjecucion->counterProgram);
	string_append(&logueoContexto, " \nQuantum: ");
	string_append_with_format(&logueoContexto, "%d", contextoDeEjecucion->quantum);
	string_append(&logueoContexto, " \nEstado: ");
	string_append(&logueoContexto, estado);
	string_append(&logueoContexto, " \nRuta: ");
	string_append(&logueoContexto, contextoDeEjecucion->mensaje);
	string_append_with_format(&logueoContexto," Recibido por el CPU %d",tid);

	log_info(logCpu, logueoContexto);
	free(estado);
    free(logueoContexto);

}

void loguearEstadoMemoria(protocolo_memoria_cpu* respuestaMemoria, char*instruccionLeida,t_log* logCpu){

	char* logueoMemoria = string_new();

	string_append(&logueoMemoria, "mProc: ");
	string_append_with_format(&logueoMemoria, "%d", respuestaMemoria->pid);
	string_append(&logueoMemoria, " - ");

	if (respuestaMemoria->codOperacion == 'i' && respuestaMemoria->codAux != 'a') {
		string_append(&logueoMemoria, "Iniciado");
	}
	if (respuestaMemoria->codOperacion == 'l') {
		string_append(&logueoMemoria, "Pagina: ");
		string_append_with_format(&logueoMemoria, "%d", respuestaMemoria->numeroPagina);
		string_append(&logueoMemoria, " leida: ");
		string_append(&logueoMemoria, respuestaMemoria->mensaje);
	}

	if (respuestaMemoria->codOperacion == 'e'){
		string_append(&logueoMemoria, "Pagina: ");
		string_append_with_format(&logueoMemoria, "%d", respuestaMemoria->numeroPagina);
		string_append(&logueoMemoria, " escrita: ");
		string_append(&logueoMemoria, respuestaMemoria->mensaje);
	}

	if ((respuestaMemoria->codOperacion == 'i') && (respuestaMemoria->codAux == 'a')) {
		string_append(&logueoMemoria, "Fallo al iniciar\n");
	}

	if (respuestaMemoria->codOperacion == 'f') {
		string_append(&logueoMemoria, "Finalizado");
	}
	log_info(logCpu, logueoMemoria);
	free(logueoMemoria);

}

void loguearPlanificadorIO(protocolo_planificador_cpu* mensajeDePlanificador, int tiempo,t_log* logCpu){
	char* logueoIO = string_new();
	string_append(&logueoIO, "mProc: ");
	string_append_with_format(&logueoIO, "%d", mensajeDePlanificador->pid);
	string_append(&logueoIO, " en entrada-salida de tiempo ");
	string_append_with_format(&logueoIO, "%d", tiempo);

	log_info(logCpu, logueoIO);
	free(logueoIO);
}
