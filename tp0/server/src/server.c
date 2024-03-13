#include "server.h"

t_config* iniciar_config()
{
	t_config* nuevo_config = config_create("server.config");

	return nuevo_config;
}

int main(void) {
	logger = log_create("log.log", "Servidor", 1, LOG_LEVEL_DEBUG);

	const char* keyIpServerConfig = "IP";
	const char* keyPuertoEscuchaConfig = "PUERTO_ESCUCHA";
	char* ipServer;
	char* puertoEscuchaServer;

	t_config* config = iniciar_config();

		// Usando el config creado previamente, leemos los valores del config y los
		// dejamos en las variables 'ip', 'puerto' y 'valor'

		// Loggeamos el valor de config

	if (config == NULL) {
		log_error(logger, "No existe la config");
		return -1;
	}

	if(!config_has_property(config, keyIpServerConfig)
			|| !config_has_property(config, keyPuertoEscuchaConfig)){
		log_error(logger, "No existe alguna de las properties a buscar");
	} else {
		ipServer = config_get_string_value(config, keyIpServerConfig);
		puertoEscuchaServer = config_get_string_value(config, keyPuertoEscuchaConfig);
	}

	int server_fd = iniciar_servidor(ipServer, puertoEscuchaServer);

	if (server_fd == -1) {
		log_error(logger, "Fallo el levantado de conexion del server");
		return -1;
	}

	log_info(logger, "Servidor listo para recibir al cliente");
	int socket_cliente = esperar_cliente(server_fd);
	int clientHandShakeRequest = -1;
	int clientHandShakeRequestExpected = HS_CLIENT;
	int serverHandShakeResponse = HS_SERVER;
	int isValidConnection = -1;

	char* textoSeConectoNuevoCliente = string_from_format("Se conecto un cliente %d", socket_cliente);
	log_info(logger, textoSeConectoNuevoCliente);

	//handshaking handling
	recv(socket_cliente, &clientHandShakeRequest, sizeof(int), MSG_WAITALL);
	char* textoHandShakeRequest = string_from_format("HS Request %d", clientHandShakeRequest);
	log_info(logger, textoHandShakeRequest);
	if (clientHandShakeRequestExpected == clientHandShakeRequest) {
		send(socket_cliente, &serverHandShakeResponse, sizeof(int), MSG_WAITALL);
	}

	recv(socket_cliente, &isValidConnection, sizeof(int), MSG_WAITALL);

	if (isValidConnection == HS_OK) {
		log_info(logger, "Valid connection");
	} else {
		log_info(logger, "Invalid connection");
		return -1;
	}

	//recibir valor clave desde el cliente

	//el malloc de paquete es el sizeof del struct
	//el malloc de buffer es el sizeof de buffer

	t_list* lista;
	while (1) {
		t_paquete* paquete = malloc(sizeof(t_paquete));
		paquete->buffer = malloc(sizeof(t_buffer));
		int cod_op = recibir_operacion(socket_cliente);
		paquete->codigo_operacion = cod_op;

		switch (cod_op) {
		case MENSAJE:
			//recibir_mensaje(paquete, socket_cliente); usar esta funcion por ejemplo para recibir un modelo con valores dinamicos dentro
			recibir_mensaje_sin_modelo(socket_cliente);
			break;
		case PAQUETE:
			lista = recibir_paquete(socket_cliente);
			log_info(logger, "Me llegaron los siguientes valores:\n");
			list_iterate(lista, (void*) iterator);
			break;
		case -1:
			log_error(logger, "el cliente se desconecto. Terminando servidor");
			return EXIT_FAILURE;
		default:
			log_warning(logger,"Operacion desconocida. No quieras meter la pata");
			sleep(4);
			break;
		}

		free(paquete->buffer->stream);
		free(paquete->buffer);
		free(paquete);
	}

	return EXIT_SUCCESS;
}


void iterator(char* value) {
	log_info(logger,"%s", value);
}
