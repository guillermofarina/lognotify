/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#ifndef _monitor_de_ficheros_h_
#define _monitor_de_ficheros_h_

#include <vector>
#include <list>
#include <memory>

#include "fichero.h"
#include "evento.h"

namespace lognotify
{

/**
* Un objeto de tipo MonitorDeFicheros vigila un conjunto de ficheros, generando eventos cada vez que
* a uno de los ficheros monitorizados se le añada un contenido adicional, y pudiendo capturar estos
* eventos mediante una llamada bloqueante: obtenerSiguienteEvento(). Un objeto de esta clase debe ser
* inicializado antes de que se le puedan añadir ficheros o capturar eventos. 
*/
class MonitorDeFicheros
{
	public:
	
	/**
	* Constructor de la clase MonitorDeFicheros
	*/
	MonitorDeFicheros (void);
		
	/**
	* Destructor de la clase MonitorDeFicheros
	*/
	~MonitorDeFicheros (void);
	
	/**
	* Inicializa el monitor de ficheros y pone en marcha el proceso de vigilancia.
	* Hasta que no se haya inicializado, no pueden añadirse ni eliminarse ficheros
	* @param dirRegistro Ruta absoluta del directorio de ficheros de registro del sistema
	* @return true si el proceso ha sido exitoso, false si ha ocurrido algún error durante el mismo
	*/
	bool inicializar (const std::string& dirRegistro);
	
	/**
	* Comprueba si la instancia de MonitorDeFicheros ya ha sido inicializada
	* @return true si la instancia ha sido correctamente inicializada y tiene un descriptor válido,
	* false en caso contrario.
	*/
	inline bool estaInicializado (void)
	{
		if (descriptor_inotify_ < 0) return false;
		return true;
	}
	
	/**
	* Añade el fichero especificado al conjunto de ficheros monitorizados.
	* Es importante tener en cuenta que es necesario inicializar la instancia antes de
	* añadir ficheros
	* @param ruta Ruta del fichero relativa al directorio de ficheros de registro del sistema
	* @return true en caso de éxito, false en caso de que se produzca algún error
	*/
	bool anadirFichero (const std::string& ruta);
	
	/**
	* Elimina el fichero especificado del conjunto de ficheros monitorizados
	* @param ruta Ruta del fichero relativa al directorio de ficheros de registro del sistema
	*/
	void eliminarFichero (const std::string& ruta);
		
	/**
	* Elimina todos los ficheros del conjunto de ficheros monitorizados
	*/
	void eliminarTodo (void);
	
	/**
	* Obtiene el número de ficheros que están siendo activamente monitorizados en el momento de la llamada
	* @return Número de ficheros activamente monitorizados en el momento de la llamada
	*/
	int obtenerNumeroDeFicheros (void);
	
	/**
	* Obtiene el siguiente evento que se produzca en cualquiera de los ficheros contenidos.
	* obtenerSiguienteEvento es una función bloqueante: la ejecución continuará cuando se
	* produzca el siguiente evento que pueda devolverse desde el momento en que sea llamada
	* @return Puntero al objeto Evento que contiene los datos del evento producido.
	* Este puntero será nulo (nullptr) en el caso de que se haya producido algún error
	*/
	std::unique_ptr<Evento> obtenerSiguienteEvento (void);
	
	private:

	/**
	* Inicia el proceso de rotación de ficheros para el fichero proporcionado, dejando el fichero inactivo
	* a la espera de que un evento posibilite empezar a monitorizarlo
	* @param fichero Puntero único al fichero a rotar
	*/
	void iniciarRotacionFichero (std::unique_ptr<Fichero> fichero);
		
	/**
	* Finaliza el proceso de rotación de ficheros para el fichero especificado si este se encontraba en
	* rotación, re-inicializando el proceso de monitorización del fichero nuevamente
	* @param indice Índice de la ubicación del fichero en rotación
	* @param nombre Nombre del fichero en rotación
	*/
	void finalizarRotacionFichero (unsigned int indice, const std::string& nombre);

	/**
	* Elimina el fichero especificado del conjunto de ficheros monitorizados
	* @param indice Índice del fichero a eliminar
	*/
	void eliminarFichero (unsigned int indice);
	
	/**
	* Obtiene la ruta absoluta canónica correspondiente a la ruta introducida. Dicha ruta debe ser una ruta
	* válida, a un directorio o fichero existente en el sistema de ficheros.
	* @param ruta Ruta a un directorio o fichero existente que se desea normalizar
	* @return Ruta absoluta canónica correspondiente a la ruta introducida. En caso de producirse algún error,
	* la función devuelve cadena vacía ("").
	*/
	std::string normalizarRuta (const std::string& ruta);
	
	/**
	* Obtiene la ruta relativa canónica correspondiente a la ruta introducida. Dicha ruta debe ser una ruta
	* válida, a un directorio o fichero existente en el sistema de ficheros.
	* @param ruta Ruta a un directorio o fichero existente que se desea normalizar, relativa a rutaRaizCanonica
	* @param rutaRaiz Ruta a un directorio existente en relación a la que desea obtenerse la ruta relativa
	* @return Ruta relativa canónica correspondiente a la ruta introducida. En caso de producirse algún error,
	* la función devuelve cadena vacía ("").
	*/
	std::string normalizarRuta (const std::string& ruta, const std::string& rutaRaiz);
	
	//Constantes
	static constexpr unsigned int LON_BUF_INOT_ = 10;	///< Longitud del buffer de inotify en número de eventos
	
	//Variables miembro
	int descriptor_inotify_;				///< Descriptor de la instancia de inotify
	char* buffer_inotify_;					///< Buffer de lectura de eventos de inotify
	ssize_t ocupado_buffer_inotify_;		///< Número de bytes de datos válidos contenidos en buffer_inotify_
	unsigned int puntero_buffer_inotify_;	///< Referencia al siguiente byte del buffer_inotify_ por procesar
	std::string directorio_registro_;		///< Ruta absoluta del directorio de ficheros de registro del sistema
	std::vector<std::unique_ptr<Fichero>> ficheros_vigilados_;		///< Lista de ficheros vigilados con inotify
	std::vector<std::list<std::unique_ptr<Fichero>>> ficheros_en_rotacion_;	///< Ficheros sin vigilancia
		///< organizados en 2 niveles (una lista de ficheros por cada ubicación diferente)
};

} //namespace lognotify

#endif //_monitor_de_ficheros_h_