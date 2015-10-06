/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#ifndef _servidor_de_conexion_h_
#define _servidor_de_conexion_h_

#include <memory>

#include "tabla_de_clientes.h"

namespace lognotify
{

/**
* Un objeto ServidorDeConexion permite aceptar conexiones entrantes y poblar una TablaDeClientes conforme vayan
* llegando estas. El ServidorDeConexion trabaja en un hilo aparte de forma que la aplicación no quede bloqueada
* esperando nuevas conexiones entrantes. Un objeto ServidorDeConexion debe de ser inicializado después de su
* construcción (ServidorDeConexion::inicializar()) antes de comenzar a recibir conexiones.
*/
class ServidorDeConexion
{
	public:
	
	/**
	* Constructor de la clase ServidorDeConexion
	*/
	ServidorDeConexion (void);
	
	/**
	* Inicializa el ServidorDeConexion con los valores especificados, quedando preparado para empezar a
	* recibir nuevos clientes. NOTA: Es necesario ejecutar esta función antes de empezar a recibir clientes.
	* @param puerto Puerto TCP en el que escuchará la entrada de solicitudes de conexión
	* @param destino Puntero a la TablaDeClientes donde se almacenarán los clientes que se conecten mediante
	* este servicio
	*/
	bool inicializar (const unsigned short puerto, std::shared_ptr<TablaDeClientes> destino);
	
	/**
	* Indica si la instancia del ServidorDeConexion ya ha sido correctamente inicializada
	* @return true si ya ha sido correctamente inicializada, false en caso contrario
	*/
	inline bool estaInicializado (void)
	{
		if (descriptor_socket_escucha_ < 0) return false;
		return true;
	}
	
	/**
	* Crea un nuevo hilo de ejecución en el que el servidor escucha nuevas conexiones entrantes para aceptar
	* nuevos clientes y añadirlos a la TablaDeClientes. El ServidorDeConexion debe haber sido previamente
	* inicializado correctamente; en caso contrario, recibirClientes retornará un error
	* @return true si el proceso de escucha ha sido lanzado correctamente, false en caso contrario
	*/
	bool recibirClientes (void);
	
	private:
	
	//Constantes
	constexpr static int MAX_PENDIENTES_ = 20;	///< Máximo de conexiones pendientes a la escucha
	
	//Variables miembro
	int descriptor_socket_escucha_;				///< Descriptor del socket asignado para escuchar conexiones
	std::shared_ptr<TablaDeClientes> clientes_;	///< Todos los clientes que se han conectado a este servidor
};

} //namespace lognotify

#endif //_servidor_de_conexion_h_