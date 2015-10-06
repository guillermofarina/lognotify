/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#ifndef _servidor_h_
#define _servidor_h_

#include <string>
#include <thread>
#include "centro_de_notificaciones.h"

namespace lognotify
{

/**
* Cada instancia de la clase Servidor es una abstracción de un servidor de logNotify en la aplicación cliente.
* Permite conectar con dicho servidor, recibiendo datos del mismo de forma concurrente al hilo principal de
* la aplicación, así como gestionar cualquier otro tipo de interacción con el mismo, incluyendo la obtención
* de los datos del servidor o su estado.
*/
class Servidor
{

	public:
	
	//Constantes públicas
	constexpr static unsigned int ESTADO_DESCONECTADO = 0;	///< Estado de conexión: desconectado
	constexpr static unsigned int ESTADO_CONECTADO = 1;		///< Estado de conexión: conectado
	constexpr static unsigned int ESTADO_PERDIDO = 2;		///< Estado de conexión: perdida
	constexpr static unsigned int ESTADO_ERROR = 3;			///< Estado de conexión: error al conectar
	
	/**
	* Constructor de la clase Servidor
	* @param direccion Dirección IP del servidor
	* @param puerto Puerto TCP correspondiente a la conexión con el servidor
	*/
	Servidor (const std::string& direccion, const std::string& puerto);
	
	/**
	* Solicita una conexión al servidor. En caso de tener éxito, el Servidor comenzará a transmitir eventos
	* al CentroDeNotificaciones especificado. Este proceso se lleva a cabo de forma concurrente al hilo
	* principal de la aplicación, por lo que ésta puede proseguir normalmente una vez realizada esta llamada
	* @param destino CentroDeNotificaciones al que el Servidor debe dirigir cada Evento enviado
	* @return true si la conexión se ha establecido correctamente, false si ha ocurrido algún error 
	*/
	bool conectar (CentroDeNotificaciones& destino);
	
	/**
	* Termina la conexión con el servidor en caso de que exista una
	*/
	void desconectar (void);
	
	/**
	* Espera a que el Servidor termine el hilo de recepción de mensajes de notificación, si hay uno en marcha.
	* NOTA: Esto bloqueará efectivamente el hilo desde el que se llame la función hasta que el hilo de recepción
	* termine 
	*/
	void esperar (void);
	
	/**
	* Devuelve el estado de conexión actual del servidor. Dicho estado se guarda en la forma de un número
	* entero, cada valor correspondiendo a un estado concreto. Las constantes públicas de clase ESTADO_XXXX
	* se corresponden a los distintos estados de conexión posibles
	* @return Estado de conexión actual del servidor
	*/
	inline unsigned int obtener_estado (void) { return estado_; }
	
	/**
	* Devuelve la dirección IP del servidor
	* @return Dirección IP del servidor
	*/
	inline std::string obtener_direccion (void) { return direccion_; }
	
	/**
	* Devuelve el puerto TCP a través del que se establece la conexión con el servidor
	* @return Puerto TCP de la conexión con el servidor
	*/
	inline std::string obtener_puerto (void) { return puerto_; }
	
	private:
	
	//Constantes de clase privadas
	constexpr static int TAMANO_BUFFER_ = 1024;	///< Tamaño del buffer utilizado para recibir datos
	
	//Variables miembro
	std::string direccion_;			///< Dirección IP del servidor
	std::string puerto_;			///< Puerto TCP de la conexión con el servidor
	int descriptor_socket_;			///< Descriptor de fichero del socket correspondiente a la conexión
	std::thread hilo_recepcion_;	///< Hilo en el que recibe notificaciones
	unsigned int estado_;			///< Estado actual de la conexión con el servidor
	
};

}//namespace lognotify

#endif //_servidor_h_