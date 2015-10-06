/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#ifndef _condicion_h_
#define _condicion_h_

#include <string>
#include <regex>

#include "evento.h"

namespace lognotify
{

/**
* Clase abstracta que define la estructura e interfaz de una condición de una Regla para el Filtro. Una
* Condicion se compone de una expresión regular en formato ECMAScript y una fución (virtual pura, implementada
* en las clases derivadas) evaluar() que decide si la condición se cumple o no para una notificación
*/
class Condicion
{
	public:
	
	/**
	* Constructor de la clase Condicion
	* @param regex Expresión regular en formato ECMAScript utilizada para evaluar la condición
	* @param signo Si se desea que la condición evalúe a true cuando la expresión regular NO sea igual y a false
	* cuando sí lo sea (es decir, negación lógica del resultado) introducir signo = false.
	*/
	Condicion (const std::string& regex, bool signo);
	
	/**
	* Función virtual pura que decide si la Condicion se cumple o no para el evento especificado
	* @param evento Evento para el que la condición es evaluada
	* @return true si la condición se cumple, false en caso contrario
	*/
	virtual bool evaluar (Evento& evento) = 0;
	
	protected:
	
	//Variables miembro
	std::regex regex_;		///< Expresión regular en formato ECMAScript utilizada para evaluar la condición
	bool signo_;			///< Indica si la condición evalúa true cuando es igual (true) o cuando NO (false)
};

/**
* Condición basada en el nombre del fichero que origina el evento. Si el nombre del fichero se iguala con la
* expresión regular que define la instancia de CondicionDeFichero, la condición se cumple
*/
class CondicionDeFichero: public Condicion
{
	public:
	
	using Condicion::Condicion;
	
	/**
	* Decide si la Condicion se cumple o no para el evento especificado. La condición se cumple cuando el
	* nombre del fichero que origina el evento iguala la expresión regular que define la condición
	* @param evento Evento para el que la condición es evaluada
	* @return true si la condición se cumple, false en caso contrario
	*/
	bool evaluar (Evento& evento);
};

/**
* Condición basada en la dirección IP del servidor que envía el evento. Si la dirección se iguala con la
* expresión regular que define la instancia de CondicionDeFichero, la condición se cumple
*/
class CondicionDeRemitente: public Condicion
{
	public:
	
	using Condicion::Condicion;
	
	/**
	* Decide si la Condicion se cumple o no para el evento especificado. La condición se cumple cuando la
	* dirección IP del servidor que envía el evento iguala la expresión regular que define la condición
	* @param evento Evento para el que la condición es evaluada
	* @return true si la condición se cumple, false en caso contrario
	*/
	bool evaluar (Evento& evento);
};

/**
* Condición basada en el contenido (descripción) del evento. Si la descripción del evento se iguala con la
* expresión regular que define la instancia de CondicionDeFichero, la condición se cumple
*/
class CondicionDeContenido: public Condicion
{
	public:
	
	using Condicion::Condicion;
	
	/**
	* Decide si la Condicion se cumple o no para el evento especificado. La condición se cumple cuando el
	* contenido (descripción) del evento iguala la expresión regular que define la condición
	* @param evento Evento para el que la condición es evaluada
	* @return true si la condición se cumple, false en caso contrario
	*/
	bool evaluar (Evento& evento);
};

}

#endif //_condicion_h_