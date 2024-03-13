#include "client.h"

int main(void)
{
	/*---------------------------------------------------PARTE 2-------------------------------------------------------------*/

	const char* keyLogConfig = "CLAVE";
	const char* keyIpServerConfig = "IP";
	const char* keyPuertoServerConfig = "PUERTO";
	int server_socket;
	char* ip;
	char* puerto;
	char* valor;

	t_log* logger;
	t_config* config;

	/* ---------------- LOGGING ---------------- */

	logger = iniciar_logger();

	// Usando el logger creado previamente
	// Escribi: "Hola! Soy un log"

	log_info(logger, "Hola! Soy un log");
	/* ---------------- ARCHIVOS DE CONFIGURACION ---------------- */

	config = iniciar_config();

	// Usando el config creado previamente, leemos los valores del config y los 
	// dejamos en las variables 'ip', 'puerto' y 'valor'

	// Loggeamos el valor de config

	if (config == NULL) {
		log_error(logger, "No existe la config");
		return -1;
	}

	if(!config_has_property(config, keyLogConfig)
			|| !config_has_property(config, keyIpServerConfig)
			|| !config_has_property(config, keyPuertoServerConfig)){
		log_error(logger, "No existe alguna de las properties a buscar");
	} else {
		valor = config_get_string_value(config, keyLogConfig);
		ip = config_get_string_value(config, keyIpServerConfig);
		puerto = config_get_string_value(config, keyPuertoServerConfig);
	}

	log_info(logger, valor);


	/*---------------------------------------------------PARTE 3-------------------------------------------------------------*/

	// ADVERTENCIA: Antes de continuar, tenemos que asegurarnos que el servidor esté corriendo para poder conectarnos a él

	// Creamos una conexión hacia el servidor
	server_socket = crear_conexion(ip, puerto, logger);

	if (server_socket == -1 ){
		log_error(logger, "No se pudo conectar al servidor");
		return -1;
	}

	//handshaking

	int clientHandShake = HS_CLIENT;
	int serverHandShakeExpected = HS_SERVER;
	int serverHandShakeResponse = -1;
	int hsResult = -1;

	//mando quien soy
	send(server_socket, &clientHandShake, sizeof(int), 0);

	//recibo quien es el otro
	recv(server_socket, &serverHandShakeResponse, sizeof(int), MSG_WAITALL);

	//Si todo fue bien, mando un ok para ver si el server era el que esperaba y que el cliente no sea rechazado.
	if (serverHandShakeResponse == serverHandShakeExpected) {
		log_info(logger, "Handshaking okey entre cliente y servidor. Conexion aceptada");
		hsResult = HS_OK;
		send(server_socket, &hsResult, sizeof(int), 0);
	} else {
		log_info(logger, "Handshaking fallo entre cliente y servidor. Conexion rechazada");
		hsResult = HS_FAIL;
		send(server_socket, &hsResult, sizeof(int), 0);
		return -1;
	}

	// Enviamos al servidor el valor de CLAVE como mensaje


	/* ---------------- LEER DE CONSOLA ---------------- */

	leer_consola_y_hacer_envio(logger, server_socket);

	// Armamos y enviamos el paquete (un solo valor dinamico)
	//enviarPaqueteValorClave(server_socket, valor); usar esta funcion por ejemplo para enviar un modelo con valores dinamicos dentro
	enviar_mensaje(valor, server_socket);


	/*---------------------------------------------------PARTE 5-------------------------------------------------------------*/

	terminar_programa(server_socket, logger, config);
}

t_log* iniciar_logger(void)
{
	t_log* nuevo_logger = log_create("tp0.log", "client.c", true, LOG_LEVEL_INFO);

	return nuevo_logger;
}

t_config* iniciar_config(void)
{
	t_config* nuevo_config = config_create("cliente.config");

	return nuevo_config;
}

void leer_consola_y_hacer_envio(t_log* logger, int server_socket)
{
	t_paquete* paquete = crear_paquete();

	char* leido;

	leido = readline("> ");

	while (strcmp(leido, "") != 0){
		log_info(logger, leido);
		agregar_a_paquete(paquete, leido, strlen(leido) + 1);
		leido = readline("> ");
	}

	enviar_paquete(paquete, server_socket);
	eliminar_paquete(paquete);
}

void enviarPaqueteValorClave(int conexionServer, char* valorClave)
{
	// Ahora toca lo divertido!

	char* leido;
	t_paquete* paqueteValorClave;
	t_modeloCliente* modelo;

	paqueteValorClave = (t_paquete*) malloc(sizeof(t_paquete));

	modelo = (t_modeloCliente*) malloc(sizeof(t_modeloCliente));

	modelo->mensajeSingle = valorClave;
	modelo->mensajeSingleLongitud = strlen(modelo->mensajeSingle) + 1;

	t_buffer* buffer = malloc(sizeof(t_buffer));
	buffer->size = sizeof(int) + strlen(modelo->mensajeSingle) + 1; //size del int de la longitud, longitud mensaje

	void* stream = malloc(buffer->size);
	int offset = 0;

	//si tuviese que agregar un valor del modelo que no es dinamico (ejemplo int), directamente hacemos el memcpy:
	//memcpy(stream + offset, &(modelo->valorEstatico), sizeof(int));
	//offset += sizeof(int);

	memcpy(stream + offset, &(modelo->mensajeSingleLongitud), sizeof(int));
	offset += sizeof(int);
	memcpy(stream + offset, modelo->mensajeSingle, strlen(modelo->mensajeSingle) + 1);

	buffer->stream = stream;

	//free(modelo->mensajeSingle); como es el mismo en este caso no es necesario

	//hasta aca, arme el buffer, ahora hay que armar el paquete con codigo de operacion y buffer

	paqueteValorClave->buffer = buffer;
	paqueteValorClave->codigo_operacion = MENSAJE;

	void* aEnviar = malloc(buffer->size + sizeof(int) + sizeof(int)); //tamanio stream (para copiarlo), tamanio del int del size del buffer, tamanio del int del codigo op
	int offsetAEnviar = 0;

	memcpy(aEnviar + offsetAEnviar, &(paqueteValorClave->codigo_operacion), sizeof(int));
	offsetAEnviar += sizeof(int);
	memcpy(aEnviar + offsetAEnviar, &(buffer->size), sizeof(int));
	offsetAEnviar += sizeof(int);
	memcpy(aEnviar + offsetAEnviar, buffer->stream, buffer->size);

	//para que enviamos el size del buffer? para que dsps en la deserializacion hagamos un malloc de ese valor, y asi obtener una direccion adecuada al "peso" del stream
	//para que enviamos el size de la variable en el stream? Lo mismo, para poder hacer un malloc y obtener una direccion adecuada al "peso" del char* que estamos enviando como mensaje

	//una vez armado el paquete, envio todo
	send(conexionServer, aEnviar, buffer->size + sizeof(int) + sizeof(int), 0);

	// ¡No te olvides de liberar las líneas y el paqueteValorClave antes de regresar!
	
	free(aEnviar);
	free(paqueteValorClave->buffer->stream);
	free(paqueteValorClave->buffer);
	free(paqueteValorClave);
}

void terminar_programa(int conexion, t_log* logger, t_config* config)
{
	/* Y por ultimo, hay que liberar lo que utilizamos (conexion, log y config) 
	  con las funciones de las commons y del TP mencionadas en el enunciado */
	log_destroy(logger);
	config_destroy(config);
	close(conexion);
}
