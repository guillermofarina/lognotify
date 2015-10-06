/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#ifndef _evento_h_
#define _evento_h_

#include <string>

namespace lognotify
{

/**
* Un objeto de tipo Evento encapsula los datos enviados por un servidor en relación a un evento de
* monitorización para ser notificado al usuario, incluyendo la ubicación completa y nombre del fichero
* donde se ha registrado, una descripcion textual del evento, y la dirección del servidor que lo ha enviado. 
*/
class Evento
{
	public:
	
	/**
	* Constructor de la clase Evento
	* @param nombre El nombre del fichero que ha provocado el evento
	* @param ubicacion La ruta completa del directorio en que se encuentra el fichero que ha
	* provocado el evento
	* @param descripcion La descripción textual del evento provocado
	* @param dirección La dirección IP del servidor que ha enviado el evento
	* @param puerto El puerto TCP de la conexión con el servidor que ha enviado el evento
	*/
	Evento (	const std::string& nombre,
				const std::string& ubicacion,
				const std::string& descripcion,
				const std::string& direccion,
				const std::string& puerto	):
		nombre_(nombre),
		ubicacion_(ubicacion),
		descripcion_(descripcion),
		direccion_(direccion),
		puerto_(puerto) {}
		
	/**
	* Obtiene el nombre del fichero que ha provocado el evento
	* @return El nombre del fichero que ha provocado el evento
	*/	
	inline std::string obtener_nombre (void)
	{
		return nombre_;
	}
	
	/**
	* Obtiene la ruta completa del directorio en que se encuentra el fichero que ha provocado
	* el evento
	* @return La ruta completa del directorio en que se encuentra el fichero que ha provocado
	* el evento
	*/
	inline std::string obtener_ubicacion (void)
	{
		return ubicacion_;
	}
	
	/**
	* Obtiene la descripción textual del evento provocado
	* @return Descripción textual del evento provocado
	*/
	inline std::string obtener_descripcion (void)
	{
		return descripcion_;
	}
	
	/**
	* Obtiene la dirección IP del servidor que ha enviado el evento
	* @return Dirección IP del servidor que ha enviado el evento
	*/
	inline std::string obtener_direccion (void)
	{
		return direccion_;
	}
	
	/**
	* Obtiene el puerto TCP correspondiente a la conexión con el servidor que ha enviado el evento
	* @return Puerto TCP de la conexión con el servidor que ha enviado el evento
	*/
	inline std::string obtener_puerto (void)
	{
		return puerto_;
	}
	
	private:
	
	//Variables miembro
	std::string nombre_;		///< Nombre del fichero que provoca el evento
	std::string ubicacion_;		///< Ubicación del fichero que provoca el evento
	std::string descripcion_;	///< Descripción del evento producido
	std::string direccion_;		///< Dirección IP del servidor que ha enviado el evento
	std::string puerto_;		///< Puerto TCP de la conexión con el servidor que ha enviado el evento
	
}; //class Evento

} //namespace lognotify

#endif //_evento_h_