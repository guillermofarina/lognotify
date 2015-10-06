/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#ifndef _servidor_de_notificaciones_h_
#define _servidor_de_notificaciones_h_

#include <string>
#include <memory>

#include "monitor_de_ficheros.h"
#include "tabla_de_clientes.h"
#include "servidor_de_conexion.h"
#include "evento.h"
#include "mensaje.h"

namespace lognotify
{

/**
* Clase principal de la aplicación servidora de logNotify. Para utilizar el sistema, debe crearse una
* instancia de ServidorDeNotificaciones, que deberá ser inicializada (ServidorDeNotificaciones::inicializar)
* para especificar su configuración. Con una llamada posterior a ServidorDeNotificaciones::darServicio el
* servidor empezará a enviar eventos de modificación en los ficheros especificados a aquellos clientes que
* se conecten al sistema logNotify. 
*/
class ServidorDeNotificaciones
{
	public:
	
	/**
	* Constructor de la clase ServidorDeNotificaciones
	*/
	ServidorDeNotificaciones (void): esta_inicializado_ (false) {}
	
	/**
	* Inicializa el servidor de notificaciones con los parámetros introducidos
	* @param puerto Puerto TCP/IP en el que el servidor debe aceptar conexiones entrantes de nuevos clientes
	* @param dirRegistro Ruta del directorio donde el sistema mantiene los ficheros de registro
	* @param ficheros Lista de rutas relativas a dirRegistro de ficheros a monitorizar
	* @return true si el proceso de inicialización es correcto, false en caso contrario
	*/
	bool inicializar (	const unsigned short puerto,
						const std::string& dirRegistro,
						const std::vector<std::string> ficheros	);
	
	/**
	* Comprueba si el servidor ha sido ya inicializado con anterioridad
	* @return true si el servidor ya ha sido inicializado, false en caso contrario
	*/					
	inline bool esta_inicializado (void) { return esta_inicializado_; }
	
	/**
	* Comienza el servicio de notificaciones remotas de modificación de ficheros de registro
	*/
	void darServicio (void);
	
	private:
		
	/**
	* Convierte un Evento de monitorización en un Mensaje listo para ser enviado por red
	* @param evento Evento de monitorización que desea serializarse
	* @return Mensaje generado a partir del Evento proporcionado
	*/
	Mensaje serializarEvento (std::unique_ptr<Evento> evento);
	
	//Variables miembro
	bool esta_inicializado_;						///< Indica si el servidor ha sido ya inicializado
	MonitorDeFicheros proveedor_de_eventos_;		///< Monitor de ficheros que genera eventos de modificación
	ServidorDeConexion proveedor_de_clientes_;		///< Servidor de conexión que acepta nuevos clientes
	std::shared_ptr<TablaDeClientes> destinatarios_;///< Clientes a los que notificar los eventos generados
};

} //namespace lognotify

#endif //_servidor_de_notificaciones_h_