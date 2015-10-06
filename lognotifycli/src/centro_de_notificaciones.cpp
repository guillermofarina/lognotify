/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include "centro_de_notificaciones.h"

#include <libnotify/notify.h>
#include <libnotify/notification.h>
#include <glib.h>
#include <string>
#include <mutex>

#include "evento.h"
#include "historial.h"
#include "filtro.h"

using namespace std;
namespace lognotify
{

CentroDeNotificaciones::CentroDeNotificaciones (void):
	inicializado_(false),
//	ruta_icono_aplicacion_(RUTA_ICONO_POR_DEFECTO),
	ruta_icono_aplicacion_(""),
	tiempo_visible_(TIEMPO_VISIBLE_POR_DEFECTO),
	mostrar_ruta_completa_(MOSTRAR_RUTA_COMPLETA_POR_DEFECTO),
	anadir_remitente_(ANADIR_REMITENTE_POR_DEFECTO) {}

bool CentroDeNotificaciones::inicializar (const std::string& nombreDeAplicacion, const std::string& rutaFicheroReglas)
{
	//Se inicializa la instancia de libnotify y el filtro y se devuelve el resultado de dichas inicializaciones
	if (!notify_init(&nombreDeAplicacion[0])) return false;
	if (!filtro_.inicializar(rutaFicheroReglas)) return false;
	return true;
}

void CentroDeNotificaciones::notificar (Evento& evento)
{
	//Se compone la notificacion de escritorio a mostrar a partir del evento
	std::string cabecera, cuerpo;
	if (!mostrar_ruta_completa_) cabecera = evento.obtener_nombre();
	else cabecera = evento.obtener_ubicacion() + evento.obtener_nombre();
	cuerpo = evento.obtener_descripcion();
	if (anadir_remitente_)
		cuerpo = cuerpo + "\nDesde: " + evento.obtener_direccion() + "/" + evento.obtener_puerto();
	
	//Se bloquea el mutex de notificación
	mutex_notificacion_.lock();
	
	//Si el filtro no la omite, se muestra la notificación de escritorio en pantalla
	if (filtro_.evaluar(evento))
		mostrarNotificacionDeEscritorio(cabecera, cuerpo, ruta_icono_aplicacion_);
	
	//Se bloquea el mutex de acceso al historial antes de liberar el de notificación para garantizar que se
	//mantiene el orden de llegada
	mutex_historial_.lock();
	
	//Se libera el mutex de notificación para permitir que otras notificaciones se muestren mientras se
	//realiza la IO de ficheros del historial
	mutex_notificacion_.unlock();
	
	//Si se ha activado el uso opcional de guardar un historial de eventos de la sesión, se registra el evento
	if (historial_de_sesion_.estaInicializado()) historial_de_sesion_.registrarEvento(evento);
	
	//Se libera el mutex de acceso al historial
	mutex_historial_.unlock();
}

void CentroDeNotificaciones::establecer_tiempo_visible (const int milisegundos)
{
	tiempo_visible_ = milisegundos;
}

void CentroDeNotificaciones::establecer_icono_aplicacion (const std::string& ruta)
{
	ruta_icono_aplicacion_ = ruta;
}

void CentroDeNotificaciones::establecer_mostrar_ruta_completa (const bool activado)
{
	mostrar_ruta_completa_ = activado;
}

void CentroDeNotificaciones::establecer_anadir_remitente (const bool activado)
{
	anadir_remitente_ = activado;
}

bool CentroDeNotificaciones::establecer_historial_de_sesion (const std::string& ruta)
{
	if (!historial_de_sesion_.estaInicializado()) return historial_de_sesion_.inicializar(ruta);
	else return false;
}

void CentroDeNotificaciones::establecer_numero_de_historiales_antiguos (unsigned int numeroDeHistoriales)
{
	historial_de_sesion_.establecer_numero_sesiones_antiguas(numeroDeHistoriales);
}

bool CentroDeNotificaciones::mostrarNotificacionDeEscritorio (	const std::string& cabecera,
																const std::string& cuerpo,
																const std::string& icono	)
{
	//Se crea una nueva notificación de libnotify
	NotifyNotification *notificacion;
	
	//Se comprueba si se desea añadir un icono o no a la notificación
	if (icono == "")
		notificacion = notify_notification_new (&cabecera[0], &cuerpo[0], nullptr);
	else
		notificacion = notify_notification_new (&cabecera[0], &cuerpo[0], &icono[0]);
		
	//Se establece el tiempo que permanecerá en pantalla la notificación
	notify_notification_set_timeout (notificacion, tiempo_visible_);
		
	//Se muestra la notificación
	bool mostrada = notify_notification_show (notificacion, nullptr);
	
	//Se libera la notificación
	g_object_unref(G_OBJECT(notificacion));
	
	//Se devuelve el resultado de mostrar en pantalla
	return mostrada;	
}

} //namespace lognotify