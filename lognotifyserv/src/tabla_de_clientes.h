/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#ifndef _tabla_de_clientes_h_
#define _tabla_de_clientes_h_

#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include "cliente.h"

namespace lognotify
{

/**
* Una TablaDeClientes representa una colección de clientes conectados a los que se puede enviar mensajes
* de red. La TablaDeClientes es thread safe, permitiendo ser accedida desde distintos hilos (por ejemplo
* para ser poblada desde un ServidorDeConexion en un hilo y enviarse mensajes a los clientes ya registrados
* en otro) con seguridad.
*/
class TablaDeClientes
{
	public:
	
	/**
	* Añade un nuevo Cliente a la TablaDeClientes a partir de una conexión creada para dicho cliente.
	* @param descriptor_socket Descriptor de fichero del socket utilizado para la conexión con el cliente
	* @return Identificador numérico asignado al nuevo Cliente. NOTA: los identificadores de cliente son
	* volátiles y pueden cambiar al ser eliminados otros clientes. Utilizar un identificador de cliente
	* mediando una llamada a eliminarCliente entre su obtención y su uso producirá comportamiento impredecible. 
	*/
	int anadirCliente (const int descriptor_socket);
		
	/**
	* Elimina un Cliente de la TablaDeClientes, terminando la conexión si todavía está abierta.
	* NOTA: eliminarCliente utiliza el algoritmo de eliminación de intercambio de posición con el último de la
	* lista y eliminar el último, por lo que (salvo que el Cliente a eliminar fuese el último) su posición
	* quedará ocupada por el último Cliente de la lista, algo importante a tener en cuenta en ciertas
	* situaciones como al recorrer la lista eliminando condicionalmente algunos de sus elementos.
	* @param identificador_cliente Identificador asignado al Cliente durante su adición a la TablaDeClientes
	*/
	void eliminarCliente (const int identificador_cliente);
	
	/**
	* Elimina todos los Clientes de la TablaDeClientes, terminando la conexión de cada uno si todavía está
	* abierta
	*/
	void eliminarTodo (void);
	
	/**
	* Envía un Mensaje al cliente especificado.
	* Si la conexión con el cliente se ha perdido, el cliente será eliminado automáticamente
	* @param identificador_cliente Identificador de Cliente
	* @param mensaje Mensaje que se desea enviar en la comunicación
	* @return true si el proceso ha tenido éxito, false en caso contrario
	*/
	bool enviar (std::shared_ptr<Mensaje> mensaje, const int identificador_cliente);
	
	/**
	* Envía un Mensaje a TODOS los clientes registrados (broadcast).
	* Si alguna de las conexiones con estos clientes se ha perdido, estos serán eliminados automáticamente
	* @param mensaje Mensaje que se desea enviar en la comunicación
	* @return true si el mensaje ha sido enviado a por lo menos un cliente válido
	*/
	bool enviar (std::shared_ptr<Mensaje> mensaje);
	
	private:
	
	/**
	* Añade un nuevo Cliente a la TablaDeClientes a partir de una conexión creada para dicho cliente.
	* Esta versión de la función miembro anadirCliente NO es segura para acceso concurrente, por lo que sólo
	* debe de ser llamada desde el interior de una sección crítica ya asegurada. Su uso está pensado únicamente
	* para uso interno de objetos de la propia clase TablaDeClientes.
	* @param descriptor_socket Descriptor de fichero del socket utilizado para la conexión con el cliente
	* @return Identificador numérico asignado al nuevo Cliente
	*/
	int anadirClienteNoSeguro (const int descriptor_socket);
	
	/**
	* Elimina un Cliente de la TablaDeClientes, terminando la conexión si todavía está abierta.
	* Esta versión de la función miembro eliminarCliente NO es segura para acceso concurrente, por lo que sólo
	* debe de ser llamada desde el interior de una sección crítica ya asegurada. Su uso está pensado únicamente
	* para uso interno de objetos de la propia clase TablaDeClientes.
	* NOTA: eliminarCliente utiliza el algoritmo de eliminación de intercambio de posición con el último de la
	* lista y eliminar el último, por lo que (salvo que el Cliente a eliminar fuese el último) su posición
	* quedará ocupada por el último Cliente de la lista, algo importante a tener en cuenta en ciertas
	* situaciones como al recorrer la lista eliminando condicionalmente algunos de sus elementos.
	* @param identificador_cliente Identificador asignado al Cliente durante su adición a la TablaDeClientes
	*/
	void eliminarClienteNoSeguro (const int identificador_cliente);
	
	//Variables miembro
	std::vector<Cliente> clientes_;	///< Lista de clientes subscritos al servidor
	std::mutex mutex_;				///< Mutex utilizado para permitir acceso concurrente seguro a los clientes 
};

} //namespace lognotify

#endif //_tabla_de_clientes_h_