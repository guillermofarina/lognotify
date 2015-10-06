/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#ifndef _centro_de_notificaciones_h_
#define _centro_de_notificaciones_h_

#include <libnotify/notify.h>
#include <libnotify/notification.h>
#include <glib.h>
#include <string>
#include <mutex>

#include "evento.h"
#include "historial.h"
#include "filtro.h"

namespace lognotify
{

/**
* Un CentroDeNotificaciones procesa las solicitudes de notificación de eventos, presentando en pantalla
* las notificaciones de escritorio correspondientes. También permite establecer las opciones de configuración
* para dicha tarea. Una instancia de CentroDeNotificaciones debe de ser correctamente inicializada como paso
* previo a ejercer su función.
*/
class CentroDeNotificaciones
{
	public:
	
	//Constantes públicas
	constexpr static int TIEMPO_VISIBLE_POR_DEFECTO = NOTIFY_EXPIRES_DEFAULT;
		///< Tiempo visible en pantalla para notificaciones: por defecto
	constexpr static int TIEMPO_VISIBLE_ILIMITADO = NOTIFY_EXPIRES_NEVER;
		///< Tiempo visible en pantalla para notificaciones: ilimitado (no expiran por sí mismas)
	//constexpr static char RUTA_ICONO_POR_DEFECTO []= "";
		///< Valor por defecto de la ruta para el icono de aplicación en los mensajes
	constexpr static bool MOSTRAR_RUTA_COMPLETA_POR_DEFECTO = false;
		///< Valor por defecto de la opción para mostrar la ruta completa en las notificaciones
	constexpr static bool ANADIR_REMITENTE_POR_DEFECTO = true;
	
	//Funciones
	
	/**
	* Constructor de la clase CentroDeNotificaciones
	*/
	CentroDeNotificaciones (void);
	
	/**
	* Inicializa la instancia de CentroDeNotificaciones con los parámetros iniciales, preparándola para presentar
	* notificaciones de escritorio. NOTA: No puede empezar a utilizarse hasta después de haber sido inicializado
	* y no puede inicializarse más de una vez (una segunda llamada a esta función devolverá error)
	* @param nombreDeAplicacion Nombre de la aplicación con el que se presentan las notificaciones
	* @param rutaFicheroReglas Ruta del fichero donde se definen las reglas de filtrado de notificaciones
	* @return true si el CentroDeNotificaciones se ha inicializado correctamente y puede proceder a ser usado,
	* false en caso contrario
	*/
	bool inicializar (const std::string& nombreDeAplicacion, const std::string& rutaFicheroReglas);
	
	/**
	* Chequea si la instancia de CentroDeNotificaciones ya ha sido inicializada correctamente
	* @return true si ya ha sido inicializado correctamente, false en caso contrario
	*/
	inline bool estaInicializado (void) { return inicializado_; }
	
	/**
	* Procesa un Evento para notificarlo. Esta función puede utilizarse de forma segura desde varios hilos
	* diferentes.
	* @param evento Evento que debe notificarse
	*/
	void notificar (Evento& evento);
	
	/**
	* Especifica el tiempo que las notificaciones de escritorio permanecen en pantalla una vez mostradas
	* @param milisegundos Tiempo en milisegundos que las notificaciones permancerán en pantalla
	*/
	void establecer_tiempo_visible (const int milisegundos);
	
	/**
	* Especifica el icono por defecto de la aplicación que aparecerá en las notificaciones de escritorio
	* @param ruta Ruta del icono
	*/
	void establecer_icono_aplicacion (const std::string& ruta);
	
	/**
	* Especifica si se desea que en la cabecera de las notificaciones se muestre la ruta completa del fichero o
	* solamente el nombre del mismo.
	* @param activado true si se desea que esta opción esté activada, false en caso contrario
	*/
	void establecer_mostrar_ruta_completa (const bool activado);
	
	/**
	* Especifica si se desea añadir el servidor que ha enviado la notificación (remitente) al final de la
	* notificación.
	* @param activado true si se desea que esta opción esté activada, false en caso contrario
	*/
	void establecer_anadir_remitente (const bool activado);

	/**
	* Especifica si se desea guardar un historial de eventos para la sesión (por defecto no). En caso de que
	* se utilice esta función, se activará el uso del historial de sesión que se guardará en el fichero con
	* la ruta especificada
	* @param ruta Ruta del fichero donde debe guardarse el historial de eventos de la sesión
	* @return true si el Historial de sesión se ha inicializado correctamente, false en caso contrario
	*/	
	bool establecer_historial_de_sesion (const std::string& ruta);
	
	/**
	* Especifica el número de historiales de sesiones anteriores que se preservarán por seguridad en caso de
	* que se utilice el Historial de sesión (establecer_historial_de_sesion). Si no se especifica nada, se
	* mantendrán Historial::NUM_SESIONES_POR_DEFECTO. Es importante observar que esta función debe de ser
	* llamada ANTES de activar el uso del Historial de sesión, pues es durante el proceso de inicialización
	* cuando realiza las rotaciones apropiadas de ficheros para salvaguardar las sesiones antiguas
	* @param numeroDeHistoriales Número de sesiones anteriores que se preservarán
	*/
	void establecer_numero_de_historiales_antiguos (unsigned int numeroDeHistoriales);
	
	private:
	
	/**
	* Presenta una notificación de escritorio en pantalla
	* @param cabecera Breve resumen o texto de cabecera para la notificación
	* @param cuerpo Contenido textual principal de la notificación
	* @param icono Ruta del icono que aparecerá en la notificación (cadena vacía "" para no utilizar ningún
	* icono)
	*/
	bool mostrarNotificacionDeEscritorio (	const std::string& cabecera,
											const std::string& cuerpo,
											const std::string& icono	);
	
	//Variables miembro
	bool inicializado_;					///< Indica si ya ha sido inicializado correctamente
	Historial historial_de_sesion_;		///< Historial de eventos de la sesión
	Filtro filtro_;						///< Filtro de notificaciones
	std::mutex mutex_notificacion_;		///< Mutex para sincronizar los accesos a libnotify
	std::mutex mutex_historial_;		///< Mutex para sincronizar los accesos al historial de sesión
	std::string ruta_icono_aplicacion_;	///< Ruta del icono por defecto para todas las notificaciones
	int tiempo_visible_;				///< Tiempo que las notificaciones permanecen en pantalla
	bool mostrar_ruta_completa_;		///< Mostrar ruta de fichero completa en las notificaciones (sí o no)
	bool anadir_remitente_;				///< Añadir servidor remitente de la notificación (sí o no)
};

} //namespace lognotify

#endif //_centro_de_notificaciones_h_