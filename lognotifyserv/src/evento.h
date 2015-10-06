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
* Un objeto de tipo Evento es generado cada vez que un fichero monitorizado es modificado.
* El evento contiene los datos relativos al mismo, incluyendo la ubicación completa y nombre del fichero,
* así como una descripcion textual del evento (que normalmente contendrá el texto añadido en la modificación). 
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
	*/
	Evento (const std::string& nombre, const std::string& ubicacion, const std::string& descripcion):
		nombre_(nombre),
		ubicacion_(ubicacion),
		descripcion_(descripcion) {}
		
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
	
	private:
	
	//Variables miembro
	std::string nombre_;		///< Nombre del fichero que provoca el evento
	std::string ubicacion_;		///< Ubicación del fichero que provoca el evento
	std::string descripcion_;	///< Descripción del evento producido
	
}; //class Evento

} //namespace lognotify

#endif //_evento_h_