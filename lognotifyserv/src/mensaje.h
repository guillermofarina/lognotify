/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#ifndef _mensaje_h_
#define _mensaje_h_

#include <cstring>

namespace lognotify
{

/**
* La clase Mensaje encapsula un buffer con datos para enviar mediante una comunicación de red asegurando que
* su contenido permanezca inalterable hasta su destrucción
*/
class Mensaje
{
	public:
	
	/**
	* Constructor de la clase Mensaje
	* @param buffer Puntero al primer byte del buffer que contiene el mensaje. El contenido del buffer será
	* copiado por lo que puede ser liberado inmediatamente después de la llamada al constructor
	* @param longitud Longitud en bytes del buffer
	*/
	Mensaje (const char *buffer, const unsigned int longitud): longitud_(longitud)
	{
		inicio_ = new char [longitud];
		memcpy(inicio_, buffer, longitud);
	}
	
	/**
	* Constructor-copia de la clase Mensaje
	* @param origen Objeto Mensaje orígen de la copia
	*/
	Mensaje (const Mensaje& origen): longitud_(origen.longitud_)
	{
		inicio_ = new char [origen.longitud_];
		memcpy(inicio_, origen.inicio_, origen.longitud_);
	}
	
	/**
	* Constructor-mover de la clase Mensaje
	* @param origen Objeto Mensaje orígen del movimiento
	*/
	Mensaje (Mensaje&& origen): longitud_(origen.longitud_)
	{
		inicio_ = origen.inicio_;
		origen.inicio_ = nullptr;
	}
	
	/**
	* Destructor de la clase Mensaje
	*/
	~Mensaje (void)
	{
		delete[] inicio_;
	}
		
	/**
	* Devuelve la longitud en bytes del mensaje
	* @return Longitud en bytes del mensaje
	*/
	inline unsigned int obtener_longitud (void) { return longitud_; }
	
	/**
	* Devuelve un puntero al primer byte del mensaje
	* @return Puntero al primer byte del mensaje
	*/
	inline char* obtener_inicio (void) { return inicio_; }
	
	/**
	* Operador de asignación-copia
	* @param origen Objeto Mensaje orígen de la copia en la asignación
	* @return Objeto Mensaje que resultante de la copia en la asignación
	*/
	Mensaje& operator= (const Mensaje& origen)
	{
		if (this == &origen) return *this;
		longitud_ = origen.longitud_;
		inicio_ = new char [origen.longitud_];
		memcpy(inicio_, origen.inicio_, origen.longitud_);
		return *this;
	}
	
	/**
	* Operador de asignación-mover
	* @param origen Objeto Mensaje orígen del movimiento en la asignación
	* @return Objeto Mensaje resultante del movimiento en la asignación
	*/
	Mensaje& operator= (Mensaje&& origen)
	{
		if (this == &origen) return *this;
		longitud_ = origen.longitud_;
		inicio_ = origen.inicio_;
		origen.inicio_ = nullptr;
		return *this;
	}
	
	private:
	
	//Variables miembro
	unsigned int longitud_;		///< Longitud de buffer del mensaje (número de bytes)
	char* inicio_;				///< Puntero al primer byte del buffer de mensaje
};

} //namespace lognotify

#endif //_mensaje_h_