/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include "cliente.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <memory>
#include <vector>
#include <future>
#include <chrono>

#include "mensaje.h"
#include "evento.h"

using namespace std;
namespace lognotify
{

Cliente::Cliente (const int descriptorSocket): descriptor_socket_(descriptorSocket) {}

bool Cliente::enviar (std::shared_ptr<Mensaje> mensaje)
{
	//Se chequea si el socket es válido
	if (descriptor_socket_ < 0) return false;
		
	//Se comprueba el estado de las confirmaciones de envío pendientes eliminando las ya recibidas antes
	//de introducir una nueva
	int fallo = false;
	for (int i = pendientes_.size() - 1; i >= 0; --i)
	{	
		//Si el estado de la confirmación pendiente no es válido, se elimina el retorno pendiente
		if (!pendientes_[i].valid())
		{
			fallo = true;
			pendientes_[i] = move(pendientes_.back());
			pendientes_.pop_back();
		}
		else
		{
			//Se comprueba si la confirmación ya ha sido retornada
			if (pendientes_[i].wait_for(chrono::steady_clock::duration::zero()) == future_status::ready)
			{
				//En caso afirmativo, se comprueba si el envío correspondiente fue exitoso o falló
				if (pendientes_[i].get() == false) fallo = true;
				
				//En cualquier caso, el retorno ya no está pendiente así que se elimina
				pendientes_[i] = move(pendientes_.back());
				pendientes_.pop_back();
			}
		}
	}
	
	//Si se ha detectado que alguno de los envíos pendientes ha fallado, se intrepreta que la conexión ha
	//fallado y se devuelve false sin más intentos
	if (fallo) return false;
	
	//Se dispara el envío asíncrono registrando el retorno futuro en pendientes_
	pendientes_.push_back(async(launch::async, []
		(const int descriptorSocket, shared_ptr<Mensaje> msj)
		{
			//La tarea asíncrona envía el mensaje a través del socket especificado
			int enviados = 0;
			char *siguiente = msj->obtener_inicio();
			while (enviados < msj->obtener_longitud())
			{
				//Hasta que se haya enviado el contenido completo, se hacen llamadas send
				enviados = send(descriptorSocket, siguiente, msj->obtener_longitud() - enviados, MSG_NOSIGNAL);
				
				//En caso de que la última llamada haya fallado, se asume un fallo de conexión y se termina
				if (enviados < 0) return false;
			}
			
			//Se termina la tarea correctamente
			return true;
			
		}, descriptor_socket_, move(mensaje)));
		
	//Se termina la funcion en true
	return true;
}

void Cliente::terminarConexion (void)
{
	//Se chequea si el descriptor de socket es válido (y por tanto corresponde a una conexión abierta)
	if (descriptor_socket_ >= 0)
	{
		//En caso afirmativo, se cierra el socket terminando la conexión
		close(descriptor_socket_);
		descriptor_socket_ = -1;
	}
}

} //namespace lognotify