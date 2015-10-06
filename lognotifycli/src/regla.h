/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#ifndef _regla_h_
#define _regla_h_

#include <vector>
#include <memory>

#include "condicion.h"
#include "evento.h"

namespace lognotify
{

/**
* Cada instancia de Regla define una regla del Filtro de notificaciones. Una Regla está compuesta por un
* conjunto de condiciones lógicas cada una de las cuales puede evaluar a true o false. Una Regla evalúa a
* true cuando TODAS las condiciones que la componen evalúan a true, false en caso contrario. Es decir, la
* evaluación de la Regla es equivalente al AND (Y) lógico de todas las condiciones que la componen
*/
class Regla
{
	public:
	
	/**
	* Añade una nueva condición a la Regla
	* @param condicion Puntero único a la Condicion que pasa a formar parte de la regla
	*/
	void anadirCondicion (std::unique_ptr<Condicion> condicion);
	
	/**
	* Decide si la Regla se cumple o no para el Evento especificado. Para que una regla se cumpla, todas y cada
	* una de las condiciones que la forman deben cumplirse también (es decir, el resultado de la función
	* evaluar() es el AND lógico de todas las condiciones que componen la regla)
	* @param evento Evento para el que la regla es evaluada
	* @return true si la regla se cumple, false en caso contrario
	*/
	bool evaluar (Evento& evento);
	
	private:
	
	//Variables miembro
	std::vector<std::unique_ptr<Condicion>> condiciones_;	///< Conjunto de condiciones que componen la regla
};

} //namespace lognotify

#endif //_regla_h_