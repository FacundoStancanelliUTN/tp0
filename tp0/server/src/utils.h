#ifndef UTILS_H_
#define UTILS_H_

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<unistd.h>
#include<netdb.h>
#include<commons/log.h>
#include<commons/collections/list.h>
#include<string.h>
#include"client_model.h"
#include<assert.h>

typedef enum
{
	MENSAJE,
	PAQUETE
}op_code;

typedef struct
{
	int size;
	void* stream;
} t_buffer;

typedef struct
{
	op_code codigo_operacion;
	t_buffer* buffer;
} t_paquete;

extern t_log* logger;

void* recibir_buffer(int*, int);

int iniciar_servidor(char* ip, char* puerto);
int esperar_cliente(int);
t_list* recibir_paquete(int);
//void recibir_mensaje(t_paquete*, int); caso que nos envien un mensaje en modelo
void recibir_mensaje_sin_modelo(int);
int recibir_operacion(int);

#endif /* UTILS_H_ */
