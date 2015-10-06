/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#ifndef _fichero_h_
#define _fichero_h_

#include <string>
#include <fstream>

namespace lognotify
{

/**
* Clase que representa los ficheros de registro con los que trabaja logNotify. Cada objeto
* inicializado de clase Fichero contiene toda la información local relativa al propio fichero (su nombre,
* ubicación dentro del directorio de ficheros de registro del sistema, los últimos cambios producidos en
* el fichero...). Cada objeto debe de ser inicializado manualmente una vez construido antes de ser
* funcional, pues dicha inicialización puede fallar.
*/
class Fichero
{
	public:
	
	/**
	* Constructor de la clase Fichero
	*/
	Fichero (void);

	/**
	* Carga los datos asociados al fichero especificado
	* @param dirRegistro Ruta absoluta del directorio de ficheros de registro del sistema
	* @param rutaFichero Ruta del fichero relativa al directorio de ficheros de registro del sistema 
	* @return True en caso de éxito, False en caso de producirse un error
	*/		
	bool inicializar (const std::string& dirRegistro, const std::string& rutaFichero);
	
	/**
	* Devuelve el nombre del fichero
	* @return Cadena de caracteres con el nombre del fichero
	*/
	inline std::string obtener_nombre (void) { return nombre_; }
	
	/**
	* Devuelve la ruta local del directorio en que se ubica el fichero, en relación al directorio de
	* registros del sistema
	* @return Cadena de caracteres con la ubicación del fichero
	*/
	inline std::string obtener_ubicacion (void) { return ubicacion_; }
	
	/**
	* Devuelve la ruta completa del fichero relativa al directorio de registros del sistema
	* @return Cadena de caracteres con la ruta del fichero
	*/
	inline std::string obtener_ruta (void) { return ubicacion_ + nombre_; }
	
	/**
	* Devuelve el contenido añadido al fichero desde la última vez que se empleó esta función.
	* Si es la primera vez que se utiliza, devuelve el contenido añadido desde su inicialización
	* NOTA: es posible que el fichero haya sido rotado/eliminado/truncado/vaciado desde su último
	* acceso, en cuyo caso esta función retornará una cadena vacía ("").
	* @param dirRegistro Ruta absoluta del directorio de ficheros de registro del sistema
	* @return Contenido añadido desde el último acceso. Puede ser cadena vacía ("") si el fichero
	* ha sido rotado/eliminado/truncado/vaciado desde entonces
	*/
	std::string ultimaModificacion (const std::string& dirRegistro);
	
	private:
	
	//Variables miembro
	std::string nombre_;			///< Nombre del fichero
	std::string ubicacion_;			///< Ruta del fichero relativa al directorio de registros
	std::streamoff ultimo_tamano_;	///< Tamaño del fichero la última vez que fue accedido
};

} //namespace lognotify

#endif //_fichero_h_