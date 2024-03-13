#include "utils.h"


void* serializar_paquete(t_paquete* paquete, int bytes)
{
	void * magic = malloc(bytes);
	int desplazamiento = 0;

	memcpy(magic + desplazamiento, &(paquete->codigo_operacion), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, &(paquete->buffer->size), sizeof(int));
	desplazamiento+= sizeof(int);
	memcpy(magic + desplazamiento, paquete->buffer->stream, paquete->buffer->size);
	desplazamiento+= paquete->buffer->size;

	return magic;
}

int crear_conexion(char *ip, char* puerto, t_log* logger)
{
	struct addrinfo hints;
	struct addrinfo *server_info;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(ip, puerto, &hints, &server_info);

	// Ahora vamos a crear el socket.

	int socket_cliente = socket(server_info->ai_family,
	                    server_info->ai_socktype,
	                    server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo

	int res = connect(socket_cliente, server_info->ai_addr, server_info->ai_addrlen);
	if (res == -1) {
		log_error(logger, "Error al conectar el socket");
	}

	freeaddrinfo(server_info);

	return (res == -1) ? -1 : socket_cliente;
}

void enviar_mensaje(char* mensaje, int socket_cliente)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));

	paquete->codigo_operacion = MENSAJE;
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = strlen(mensaje) + 1; //size del buffer para el string que enviemos
	paquete->buffer->stream = malloc(paquete->buffer->size); //valor del mensaje
	memcpy(paquete->buffer->stream, mensaje, paquete->buffer->size); //del mensaje copiamos al paquete.buffer.stream el size del paquete.buffer.size

	int bytes = paquete->buffer->size + 2*sizeof(int); //se envia el size del buffer (que es lo que pesa el stream), otro int para el largo del buffer y otro para el codigo de operacion

	void* a_enviar = serializar_paquete(paquete, bytes); //si o si tenemos que armar un paquete para enviar la operacion, el size del buffer.stream y el propio stream

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
	eliminar_paquete(paquete);
}


void crear_buffer(t_paquete* paquete)
{
	paquete->buffer = malloc(sizeof(t_buffer));
	paquete->buffer->size = 0;
	paquete->buffer->stream = NULL;
}

t_paquete* crear_paquete(void)
{
	t_paquete* paquete = malloc(sizeof(t_paquete));
	paquete->codigo_operacion = PAQUETE;
	crear_buffer(paquete);
	return paquete;
}

void agregar_a_paquete(t_paquete* paquete, void* valor, int tamanio)
{
	paquete->buffer->stream = realloc(paquete->buffer->stream, paquete->buffer->size + tamanio + sizeof(int)); //de la direccion que ocupaba el buffer inicialmente, le agregas el size del nuevo valor y el size de int (porque necesitamos enviar el mismo size al server)

	memcpy(paquete->buffer->stream + paquete->buffer->size, &tamanio, sizeof(int)); //enviamos el tamanio del valor
	memcpy(paquete->buffer->stream + paquete->buffer->size + sizeof(int), valor, tamanio); //enviamos el valor

	paquete->buffer->size += tamanio + sizeof(int);
}

void enviar_paquete(t_paquete* paquete, int socket_cliente)
{
	int bytes = paquete->buffer->size + 2*sizeof(int); //el size del stream, el sizeof(int) del valor del size del stream, el sizeof(int) del size del valor del codigo de operacion
	void* a_enviar = serializar_paquete(paquete, bytes);

	send(socket_cliente, a_enviar, bytes, 0);

	free(a_enviar);
}

void eliminar_paquete(t_paquete* paquete)
{
	free(paquete->buffer->stream);
	free(paquete->buffer);
	free(paquete);
}

void liberar_conexion(int socket_cliente)
{
	close(socket_cliente);
}
