/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#ifndef _historial_h_
#define _historial_h_

#include <string>
#include "evento.h"

namespace lognotify
{

/**
* Una instancia de Historial permite crear un historial de todas las notificaciones recibidas a lo largo de
* la sesión en un fichero que pueda ser revisado posteriormente. El historial permite mantener un cierto
* número de sesiones pasadas almacenadas, a partir del cual las sesiones más antiguas se irán eliminando 
*/
class Historial
{
	public:
	
	//Constantes públicas
	constexpr static unsigned int NUM_SESIONES_POR_DEFECTO = 5;
									///< Número máximo de sesiones antiguas por defecto
	
	//Funciones
	
	/**
	* Constructor de la clase Historial
	*/
	Historial (void);
	
	/**
	* Inicializa la instancia de Historial, preparándola para registrar notificaciones. También rota los
	* ficheros de sesiones previos de acuerdo a los parámetros introducidos
	* @param ruta Ruta del fichero en el que almacenar el historial de notificaciones
	* @return true en caso de que el proceso de inicialización haya sido exitoso, false en caso contrario
	*/
	bool inicializar (const std::string& ruta);
	
	/**
	* Indica si la instancia de Historial ya ha sido correctamente inicializada
	* @return true si la instancia ya ha sido correctamente inicializada, false en caso contrario
	*/
	inline bool estaInicializado (void) { return inicializado_; }
	
	/**
	* Registra un evento de notificación en el historial
	* @param evento Evento para registrar en el historial
	* @return true si el evento se ha registrado correctamente, false en caso contrario
	*/
	bool registrarEvento (Evento& evento);
	
	/**
	* Establece cuantas sesiones antiguas se mantendrán en la rotación de ficheros de Historial. Si esta
	* función no es ANTES de la inicialización, se aplicará el valor por defecto NUM_SESIONES_POR_DEFECTO
	* @param numeroDeSesiones El número de sesiones antiguas que se desea mantener (este puede ser 0, en cuyo
	* caso NO se guardarán los ficheros correspondientes a sesiones antiguas y simplemente se reescribirán
	* en cada nueva sesión).
	*/
	void establecer_numero_sesiones_antiguas (const unsigned int numeroDeSesiones)
	{
		numero_sesiones_antiguas_ = numeroDeSesiones;
	}
	
	private:
	
	/**
	* Rota los ficheros de sesiones antigüas de forma que se preserven hasta el número indicado por parámetros
	* de las mismas, y cualquier historial de sesión adicional sea eliminado. Se preservan los N ficheros de
	* historial de sesiones antiguas más recientes, numerados con .1, .2, .3, etcétera hasta el número indicado
	* por orden de más reciente a más antigüo.
	* @param ruta Ruta del fichero donde se almacena el historial de la sesión actual
	*/
	void rotarFicheros (const std::string& ruta);
	
	//Variables miembro
	std::string ruta_historial_;			///< Ruta del fichero donde se escribe el historial de la sesión
	bool inicializado_;						///< Indica si ya ha sido correctamente inicializado
	unsigned int numero_sesiones_antiguas_;	///< Número de sesiones antiguas a mantener
};

} //namespace lognotify

#endif //_historial_h_