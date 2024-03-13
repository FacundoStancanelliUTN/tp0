#include"utils.h"

t_log* logger;

int iniciar_servidor(char* ip, char* puertoEscucha)
{

	int socket_servidor;

	struct addrinfo hints, *server_info, *p;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	getaddrinfo(NULL, puertoEscucha, &hints, &server_info);

	int socket_server = socket(server_info->ai_family,
		                    server_info->ai_socktype,
		                    server_info->ai_protocol);

	// Ahora que tenemos el socket, vamos a conectarlo

	log_trace(logger, "Listo para escuchar a mi cliente");

	bind(socket_server, server_info->ai_addr, server_info->ai_addrlen);

	listen(socket_server, SOMAXCONN);

	freeaddrinfo(server_info);

	return socket_server;
}

int esperar_cliente(int socket_servidor)
{

	// Aceptamos un nuevo cliente
	int socket_cliente = accept(socket_servidor, NULL, NULL);
	log_info(logger, "Se conecto un cliente!");

	return socket_cliente;
}

int recibir_operacion(int socket_cliente)
{
	int codigoOperacion = -1;

	recv(socket_cliente, &(codigoOperacion), sizeof(int), MSG_WAITALL);

	return codigoOperacion;
}

void* recibir_buffer(int* size, int socket_cliente)
{
	void * buffer;

	recv(socket_cliente, size, sizeof(int), MSG_WAITALL);
	buffer = malloc(*size);
	recv(socket_cliente, buffer, *size, MSG_WAITALL);

	return buffer;
}

void recibir_mensaje_sin_modelo(int socket_cliente) {
	int size;

	char* valor = (char*) recibir_buffer(&size, socket_cliente);

	log_info(logger, "valor recibido:");
	log_info(logger, valor);
}

t_list* recibir_paquete(int socket_cliente)
{
	int size;
	int desplazamiento = 0;
	void * buffer;
	t_list* valores = list_create();
	int tamanio;

	buffer = recibir_buffer(&size, socket_cliente);
	while(desplazamiento < size)
	{
		memcpy(&tamanio, buffer + desplazamiento, sizeof(int));
		desplazamiento+=sizeof(int);
		char* valor = malloc(tamanio);
		memcpy(valor, buffer+desplazamiento, tamanio);
		desplazamiento+=tamanio;
		list_add(valores, valor);
	}
	free(buffer);
	return valores;
}

/*CASO QUE NOS ENVIEN UN MODELO DESDE EL CLIENTE, PARA DESERIALIZARLO
t_modeloCliente* obtenerModeloCliente(t_buffer* buffer){

	t_modeloCliente* cliente = malloc(sizeof(t_modeloCliente));
	void* stream = buffer->stream;

	memcpy(&cliente->mensajeSingleLongitud, stream, sizeof(int));
	stream+=sizeof(int);
	cliente->mensajeSingle = malloc(cliente->mensajeSingleLongitud);
	memcpy(cliente->mensajeSingle, stream, cliente->mensajeSingleLongitud);

	return cliente;
}

void recibir_mensaje(t_paquete* paquete, int socket_cliente)
{
	recv(socket_cliente, &(paquete->buffer->size), sizeof(int), MSG_WAITALL);
	paquete->buffer->stream = malloc(paquete->buffer->size);
	//como deciamos en la parte del cliente, el size del buffer va a servir para hacer el recv (no sirve el sizeof porque hay parametros dinamicos)
	recv(socket_cliente, paquete->buffer->stream, paquete->buffer->size, MSG_WAITALL);

	t_modeloCliente* modelo = obtenerModeloCliente(paquete->buffer);

	char* mensajeObtenido = modelo->mensajeSingle;

	log_info(logger, "valor recibido: ");
	log_info(logger, mensajeObtenido);

	free(modelo->mensajeSingle);
	free(modelo);
}
esto es en caso que nos envien un modelito
*/

