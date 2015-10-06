/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#ifndef _cliente_h_
#define _cliente_h_

#include <memory>
#include <vector>
#include <future>

#include "mensaje.h"
#include "evento.h"

namespace lognotify
{

/**
* Cada Cliente es una abstracción de una conexión con un cliente del sistema, que permite enviar objetos de
* tipo Mensaje a dicho cliente a través de la conexión TCP/IP creada. Crear y aceptar dicha conexión no es
* responsabilidad de la clase Cliente; una vez creada la conexión, el descriptor de fichero del socket será
* pasado al constructor de esta clase para crear la abstracción del cliente en torno al mismo
*/
class Cliente
{
	public:
	
	/**
	* Constructor de la clase Cliente
	* @param descriptorSocket Descriptor de fichero del socket correspondiente a la conexión al cliente
	*/
	Cliente (const int descriptorSocket);
	
	/**
	* Envía el Evento especificado al cliente. El mensaje es enviado de forma asíncrona, de forma que una
	* vez que se ha llamado a esta función el mensaje queda en proceso de ser enviado a su destino, pero esto
	* puede ocurrir de forma no inmediata. La ejecución puede continuar normalmente como si el mensaje hubiera
	* sido enviado (lo que ocurrirá tan pronto como sea posible)
	* @param mensaje Mensaje que desea enviarse
	* @return false si se detecta que la conexión ha fallado, true en caso contrario
	*/
	bool enviar (std::shared_ptr<Mensaje> mensaje);
		
	/**
	* Termina la conexión con el cliente si esta aún está vigente y cierra el socket de conexión.
	* Es importante terminar la conexión con el Cliente mediante una llamada a esta función, pues destruir el
	* objeto no lo hace automáticamente. Esto es así para permitir la duplicación o movimiento de objetos
	* Cliente sin terminar la conexión para todos ellos cuando uno de ellos es destruido.
	*/
	void terminarConexion (void);
	
	private:
	
	//Variables miembro
	int descriptor_socket_;		///< Descriptor de fichero del socket correspondiente a la conexión al cliente
	std::vector<std::future<bool>> pendientes_;	///< Confirmaciones de envío pendientes de recibir
};

} //namespace lognotify

#endif //_cliente_h_