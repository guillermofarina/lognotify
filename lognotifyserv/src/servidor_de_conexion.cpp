/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include "servidor_de_conexion.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>
#include <cstring>
#include <memory>
#include <thread>

#include "tabla_de_clientes.h"

using namespace std;
namespace lognotify
{
	
ServidorDeConexion::ServidorDeConexion (void): descriptor_socket_escucha_(-1) {}

bool ServidorDeConexion::inicializar (const unsigned short puerto, std::shared_ptr<TablaDeClientes> destino)
{
	//Si ya está inicializado, primero cierra el socket previo
	if (estaInicializado())
	{
		close(descriptor_socket_escucha_);
		descriptor_socket_escucha_ = -1;
	}
	
	//Se rellenan los datos de conexión necesarios para crear un socket
	struct addrinfo indicaciones;
	struct addrinfo *info_servidor;
	string cad_puerto = to_string(puerto);
	
	memset(&indicaciones, 0, sizeof(struct addrinfo));
	indicaciones.ai_family = AF_UNSPEC;
	indicaciones.ai_socktype = SOCK_STREAM;
	indicaciones.ai_flags = AI_PASSIVE;
	
	//Si no se consigue obtener información de dirección válida para el servidor, se termina con error
	if (getaddrinfo(nullptr, &cad_puerto[0], &indicaciones, &info_servidor) != 0) return false;
		
	//Se recorren los resultados obtenidos creando y asociando el socket a la primera dirección posible
	for (struct addrinfo *p = info_servidor; p != nullptr; p = p->ai_next)
	{
		descriptor_socket_escucha_ = socket (p->ai_family, p->ai_socktype, p->ai_protocol);
		if (descriptor_socket_escucha_ >= 0)
		{	
			//Si se ha encontrado una dirección válida, se establecen las opciones del socket
			int si = 1;
			if (setsockopt(descriptor_socket_escucha_, SOL_SOCKET, SO_REUSEADDR, &si, sizeof(int)) < 0)
			{				
				//Si esta operación falla, se devuelve error
				freeaddrinfo(info_servidor);
				close(descriptor_socket_escucha_);
				descriptor_socket_escucha_ = -1;
				return false;
			}
			
			//Se asocia el socket a la dirección encontrada
			if (bind(descriptor_socket_escucha_, p->ai_addr, p->ai_addrlen) >= 0)
			{
				//En caso de que todo haya ido correctamente, se guarda la tabla de clientes y se termina
				freeaddrinfo(info_servidor);
				clientes_ = destino;
				return true;
			}
			
			//Si ha fallado la asociación del socket a la dirección, se cierra el socket y se sigue buscando
			close(descriptor_socket_escucha_);
		}
	}
	
	//Si se ha llegado a este punto, no se ha conseguido conectar a ninguna dirección válida
	freeaddrinfo(info_servidor);
	return false;
}

bool ServidorDeConexion::recibirClientes (void)
{
	//Se chequea si el servidor ha sido ya correctamente inicializado y dispone de un socket operativo
	if (!estaInicializado()) return false;
	
	//El servidor comienza a escuchar en el socket
	if (listen(descriptor_socket_escucha_, MAX_PENDIENTES_) < 0) return false;
		
	//Se crea un nuevo hilo para obtener nuevos clientes
	thread hilo_servidor ([]
		(const int descriptor_socket_escucha, weak_ptr<TablaDeClientes> clientes)
		{
			//En el hilo, se crea un bucle que obtiene nuevos clientes y los añade a los ya existentes
			shared_ptr<TablaDeClientes> sp_clientes;
			int socket_nuevo_cliente;
			struct sockaddr_storage direccion_nuevo_cliente;
			socklen_t tamano_direccion = sizeof(struct sockaddr_storage);
			bool terminar = false;
			while (!terminar)
			{
				socket_nuevo_cliente = accept (	descriptor_socket_escucha,
												(struct sockaddr *)& direccion_nuevo_cliente,
												&tamano_direccion	);
				if (socket_nuevo_cliente >= 0)
				{
					sp_clientes = clientes.lock();
					if (!sp_clientes) terminar = true;
					else sp_clientes->anadirCliente(socket_nuevo_cliente);
					sp_clientes.reset();
				}
				else terminar = true;
			}
			
		}, descriptor_socket_escucha_, clientes_);
		
	//Se desvincula del hilo servidor y termina con éxito
	hilo_servidor.detach();
	return true;
}
	
} //namespace lognotify