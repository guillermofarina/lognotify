/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#ifndef _cliente_de_notificaciones_h_
#define _cliente_de_notificaciones_h_

#include <string>
#include <vector>

#include "centro_de_notificaciones.h"
#include "servidor.h"

namespace lognotify
{

/**
* Clase principal de la aplicación cliente de logNotify desde la que se controlan las funcionalidades de la 
* misma. Una instancia de ClienteDeNotificaciones debe de ser inicializada mediante la función
* ClienteDeNotificaciones::inicializar() para empezar a funcionar
*/	
class ClienteDeNotificaciones
{
	public:
	
	/**
	* Constructor de la clase ClienteDeNotificaciones
	* @param nombreDeAplicacion Nombre de la aplicación por el que será referida de forma externa
	*/
	ClienteDeNotificaciones (const std::string& nombreDeAplicacion);
	
	/**
	* Inicializa la instancia de ClienteDeNotificaciones. Es necesario llamar a esta instancia antes de
	* comenzar a usarla
	* @param rutaFicheroServidores Ruta del fichero que lista los servidores a los que conectar
	* @param rutaFicheroFiltro Ruta del fichero que contiene las reglas de filtrado de notificaciones
	* @param rutaFicheroHistorial Ruta del fichero en el que se escribe el historial de la sesión
	* @param tiempoExpiracionNotificaciones Tiempo que permanecen en pantalla las notificaciones
	* @param mostrarRutaCompletaNotificaciones Mostrar la ruta completa del fichero en las notificaciones
	* @param mostrarRemitenteNotificaciones Mostrar la dirección del servidor remitente en las notificaciones
	* @param numeroSesionesAntiguas Número de ficheros de historial de sesión antiguos a preservar
	* @return true si el proceso de inicialización transcurre con éxito, false en caso contrario
	*/
	bool inicializar	(	const std::string& rutaFicheroServidores,
							const std::string& rutaFicheroFiltro,
							const std::string& rutaFicheroHistorial,
							int tiempoExpiracionNotificaciones,
							bool mostrarRutaCompletaNotificaciones,
							bool mostrarRemitenteNotificaciones,
							int numeroSesionesAntiguas );
	
	/**
	* Registra un nuevo Servidor. Esta llamada no conecta automáticamente a dicho Servidor, para ello deberá
	* hacerse uso de otra llamada posterior a conectarServidor()
	* @param direccion Dirección IP del servidor
	* @param puerto Puerto TCP en el que el servidor espera la conexión
	* @return Identificador asignado al servidor correspondiente 
	*/
	int anadirServidor (const std::string& direccion, const std::string& puerto);
	
	/**
	* Conecta al Servidor especificado
	* @param identificador Identificador del servidor al que se desea conectar
	* @return true si el proceso de conexión ha sido exitoso, false en caso contrario
	*/
	bool conectarServidor (const int identificador);
	
	/**
	* Desconecta del Servidor especificado
	* @param identificador Identificador del servidor que se desea desconectar
	*/
	void desconectarServidor (const int identificador);
	
	private:
	
	//Variables miembro
	std::string nombre_de_aplicacion_;
	CentroDeNotificaciones notificador_;
	std::vector<Servidor> proveedores_;
	bool inicializada_;
};

} //namespace lognotify
#endif //_cliente_de_notificaciones_h_