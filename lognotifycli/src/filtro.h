/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#ifndef _filtro_h_
#define _filtro_h_

#include <vector>
#include <memory>
#include <string>
#include "regla.h"

namespace lognotify
{

/**
* Un objeto de tipo Filtro se utiliza para filtrar eventos antes de notificarlos al usuario, de forma
* que no todos los eventos provoquen una notificación de escritorio. De esta forma, un usuario puede filtrar
* parte de los eventos producidos, de forma que no sea importunado por eventos que no quiera ver. Las reglas
* que componen el Filtro se definen en un fichero que el propio Filtro parsea durante su inicialización, y cada
* una se compone de una serie de condiciones consistentes en comparaciones de alguno de los campos del Evento
* con expresiones regulares. Un objeto de tipo Filtro debe de ser inicializado para estar activo
*/
class Filtro
{
	
	public:
	
	/**
	* Constructor de la clase Filtro
	*/
	Filtro (void): inicializado_ (false) {}
	
	/**
	* Inicializa y activa el Filtro, con las reglas definidas en el fichero cuya ruta se pasa por parámetro
	* @param rutaDeReglas Ruta del fichero donde se encuentran definidas las reglas para el Filtro
	* @return true si la inicialización ha tenido éxito, false en caso contrario
	*/
	bool inicializar (const std::string& rutaDeReglas);
	
	/**
	* Evalúa si un Evento debe ser notificado al usuario (es decir, si pasa el Filtro) o no. La evaluación
	* resulta positiva si el Evento no dispara ninguna de las reglas de omisión que conforman el Filtro (es
	* decir, si al menos una Regla se cumple para el Evento en cuestión, no pasará el Filtro)
	* @param evento Evento a filtrar
	* @return true si el Evento pasa el Filtro y debe ser notificado, false en caso contrario
	*/
	bool evaluar (Evento& evento);
	
	/**
	* Indica si la instancia de Filtro ya ha sido correctamente inicializada
	* @return true si el Filtro está inicializado, false en caso contrario
	*/
	inline bool estaInicializado (void) { return inicializado_; }
	
	private:
	
	/**
	* Extrae las reglas definidas en el fichero indicado, cargándolas en el Filtro
	* @param rutaDeReglas Ruta del fichero donde se encuentran definidas las reglas para el Filtro
	* @return true si el proceso ha finalizado sin errores, false en caso contrario
	*/
	bool cargarReglas (const std::string& rutaDeReglas);
	
	//Variables miembro
	std::vector<std::unique_ptr<Regla>> reglas_;	///< Conjunto de reglas usadas para filtrar notificaciones
	bool inicializado_;								///< Indica si ha sido correctamente inicializado o no
};

} //namespace lognotify

#endif //_filtro_h_