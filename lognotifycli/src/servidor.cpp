/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include "servidor.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <cstring>
#include <string>
#include <thread>

#include "centro_de_notificaciones.h"

using namespace std;
namespace lognotify
{

Servidor::Servidor (const std::string& direccion, const std::string& puerto):
	direccion_(direccion),
	puerto_(puerto),
	descriptor_socket_(-1),
	estado_(ESTADO_DESCONECTADO) {}

bool Servidor::conectar (CentroDeNotificaciones& destino)
{
	//Si ya está conectado, se termina la conexión pre-existente en primer lugar
	if (estado_ == ESTADO_CONECTADO) desconectar();
	
	//Se obtiene la información de direcciones del servidor
	struct addrinfo indicaciones, *info_servidor;
	
	memset(&indicaciones, 0, sizeof indicaciones);
	indicaciones.ai_family = AF_UNSPEC;
	indicaciones.ai_socktype = SOCK_STREAM;
	
	//Si el proceso para obtener las direcciones del servidor falla, se actualiza el estado y se termina
	if (getaddrinfo(&direccion_[0], &puerto_[0], &indicaciones, &info_servidor) != 0)
	{
		estado_ = ESTADO_ERROR;
		return false;
    }
	
	//Se recorren las posibles direcciones obtenidas, conectando a la primera que lo permita
	descriptor_socket_ = -1;
	for (struct addrinfo *p = info_servidor; p != nullptr; p = p->ai_next)
	{
		descriptor_socket_ = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (descriptor_socket_ >= 0)
		{
			//Se ha obtenido un socket válido al que intentar conectar
			//Si consigue establecer conexión, termina la búsqueda
			if (connect(descriptor_socket_, p->ai_addr, p->ai_addrlen) == 0) break;
			else
			{
				//En caso contrario, se cierra el socket y se continúa
				close(descriptor_socket_);
				descriptor_socket_ = -1;
			}
		}
	}
	
	//Se libera la información de direcciones del servidor
	freeaddrinfo(info_servidor);
	
	//Al finalizar la búsqueda de una dirección válida, si no se ha conectado se termina con error
	if (descriptor_socket_ < 0)
	{
		estado_ = ESTADO_ERROR;
		return false;
	}
	
	//Una vez conseguida la conexión, se actualiza el estado y se lanza el hilo para recibir datos
	estado_ = ESTADO_CONECTADO;
	
	thread hilo_recepcion ([]
		(	int descriptor_socket,
			CentroDeNotificaciones *notificador,
			const string& direccion,
			const string& puerto	)
		{
			//En el hilo de recepción, se recuperan eventos de monitorización y se pasan al notificador
			//Estos vienen en formato "nombre"\0"ubicacion"\0"descripcion"\0
			char buffer [Servidor::TAMANO_BUFFER_];
			int recibidos = 1;
			int procesados = 1;
			int longitud;
			bool terminado;
			string nombre, ubicacion, descripcion;
			Evento *nuevo_evento;
			
			//Se deserializan notificaciones hasta que la conexión falle
			while (recibidos > 0)
			{
				
			//En primer lugar hay que obtener el nombre de la notificación
				
				terminado = false;
				nombre = "";
				
				//Si el buffer actual ya ha sido completamente procesado, se lee uno nuevo antes de continuar
				if (procesados == recibidos)
				{
					//Se asegura que el contenido del buffer más allá de lo leído sea 0's
					memset(&buffer[0], 0, sizeof(buffer));
					recibidos = recv (descriptor_socket, buffer, Servidor::TAMANO_BUFFER_ - 1, MSG_NOSIGNAL);
					procesados = 0;
				}
				
				//Se concatenan bufferes al nombre hasta que se lea un \0 entre los recibidos (o falle)
				while ((recibidos > 0) && !terminado)
				{
					//Al concatenar, sólo lo hace hasta el primer \0 que encuentre (puede ser entre los leídos,
					//o el 1º más allá de estos, en cuyo caso no habrá terminado aún y habrá que continuar)
					longitud = nombre.length();
					nombre = nombre + &buffer[procesados];
					procesados = procesados + nombre.length() - longitud;
					
					//Como string::length() no cuenta el byte \0, si los bytes procesados calculados de la forma
					//anterior son < recibidos, significa que ha encontrado un \0 entre los recibidos y termina
					if (procesados < recibidos) terminado = true;
					else
					{
						//Si no, se llena otro buffer para continuar añadiendo al nombre
						memset(&buffer[0], 0, sizeof(buffer));
						recibidos = recv (descriptor_socket, buffer, Servidor::TAMANO_BUFFER_ - 1, MSG_NOSIGNAL);
						procesados = 0;
					}
				}
				//Una vez terminado con el nombre, se incrementa procesados para contabilizar el \0 finalmente
				//leído de entre los recibidos...
				++procesados;
								
			//Ahora hay que obtener la ubicación repitiendo el procedimiento anterior por donde se dejó
			
				terminado = false;
				ubicacion = "";
				
				//Si el buffer actual ya ha sido completamente procesado, se lee uno nuevo antes de continuar
				if ((recibidos > 0) && (procesados == recibidos))
				{
					//Se asegura que el contenido del buffer más allá de lo leído sea 0's
					memset(&buffer[0], 0, sizeof(buffer));
					recibidos = recv (descriptor_socket, buffer, Servidor::TAMANO_BUFFER_ - 1, MSG_NOSIGNAL);
					procesados = 0;
				}
				
				//Se concatenan bufferes a la ubicación hasta que se lea un \0 entre los recibidos (o falle)
				while ((recibidos > 0) && !terminado)
				{
					//Al concatenar, sólo lo hace hasta el primer \0 que encuentre (puede ser entre los leídos,
					//o el 1º más allá de estos, en cuyo caso no habrá terminado aún y habrá que continuar)
					longitud = ubicacion.length();
					ubicacion = ubicacion + &buffer[procesados];
					procesados = procesados + ubicacion.length() - longitud;
					
					//Como string::length() no cuenta el byte \0, si los bytes procesados calculados de la forma
					//anterior son < recibidos, significa que ha encontrado un \0 entre los recibidos y termina
					if (procesados < recibidos) terminado = true;
					else
					{
						//Si no, se llena otro buffer para continuar añadiendo a la ubicacion
						memset(&buffer[0], 0, sizeof(buffer));
						recibidos = recv (descriptor_socket, buffer, Servidor::TAMANO_BUFFER_ - 1, MSG_NOSIGNAL);
						procesados = 0;
					}
				}
				//Una vez terminado con la ubicacion, se incrementa procesados para contabilizar el \0 finalmente
				//leído de entre los recibidos...
				++procesados;
				
			//Por último, se repite el mismo procedimiento para la descripción
			
				terminado = false;
				descripcion = "";
				
				//Si el buffer actual ya ha sido completamente procesado, se lee uno nuevo antes de continuar
				if ((recibidos > 0) && (procesados == recibidos))
				{
					//Se asegura que el contenido del buffer más allá de lo leído sea 0's
					memset(&buffer[0], 0, sizeof(buffer));
					recibidos = recv (descriptor_socket, buffer, Servidor::TAMANO_BUFFER_ - 1, MSG_NOSIGNAL);
					procesados = 0;
				}
				
				//Se concatenan bufferes a la descripción hasta que se lea un \0 entre los recibidos (o falle)
				while ((recibidos > 0) && !terminado)
				{
					//Al concatenar, sólo lo hace hasta el primer \0 que encuentre (puede ser entre los leídos,
					//o el 1º más allá de estos, en cuyo caso no habrá terminado aún y habrá que continuar)
					longitud = descripcion.length();
					descripcion = descripcion + &buffer[procesados];
					procesados = procesados + descripcion.length() - longitud;
					
					//Como string::length() no cuenta el byte \0, si los bytes procesados calculados de la forma
					//anterior son < recibidos, significa que ha encontrado un \0 entre los recibidos y termina
					if (procesados < recibidos) terminado = true;
					else
					{
						//Si no, se llena otro buffer para continuar añadiendo a la descripción
						memset(&buffer[0], 0, sizeof(buffer));
						recibidos = recv (descriptor_socket, buffer, Servidor::TAMANO_BUFFER_ - 1, MSG_NOSIGNAL);
						procesados = 0;
					}
				}
				//Una vez terminado con la descripción, se incrementa procesados para contabilizar el \0 finalmente
				//leído de entre los recibidos...
				++procesados;
				
				//Si la conexión no ha fallado, en este punto hay un evento completo deserializado
				//Se crea y se envía
				if (recibidos > 0)
				{
					nuevo_evento = new Evento (nombre, ubicacion, descripcion, direccion, puerto);
					notificador->notificar(*nuevo_evento);
					delete nuevo_evento;
				}
			}
					
		}, descriptor_socket_, &destino, direccion_, puerto_);
	
	//se guarda el estado del hilo creado y se retorna con éxito
	hilo_recepcion_ = move(hilo_recepcion);
	return true;
}

void Servidor::desconectar (void)
{
	//Si el estado actual es conectado, se cierra el socket de conexión con el servidor
	//El hilo en el que se reciben los eventos del servidor terminará por sí sólo al fallar la recepción
	if (estado_ == ESTADO_CONECTADO)
	{
		close (descriptor_socket_);
		descriptor_socket_ = -1;
	}
	
	//Fuera cual fuera el estado previo, se establece el estado actual como desconectado
	estado_ = ESTADO_DESCONECTADO;
}

void Servidor::esperar (void)
{
	//Si el hilo es joinable (es decir, está escuchando mensajes) espera a que termine
	if (hilo_recepcion_.joinable()) hilo_recepcion_.join();
}

} //namespace lognotify