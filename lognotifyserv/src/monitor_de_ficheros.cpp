/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include "monitor_de_ficheros.h"

#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <stdlib.h>
#include <vector>
#include <list>
#include <memory>

#include "fichero.h"
#include "evento.h"

using namespace std;
namespace lognotify
{
	
MonitorDeFicheros::MonitorDeFicheros (void):
		descriptor_inotify_(-1),
		ocupado_buffer_inotify_(0),
		puntero_buffer_inotify_(0),
		directorio_registro_("") {}

MonitorDeFicheros::~MonitorDeFicheros (void)
{
	close(descriptor_inotify_);
	delete[] buffer_inotify_;
}

bool MonitorDeFicheros::inicializar (const std::string& dirRegistro)
{
	//Se inicializa la instancia de inotify, obteniendo el descriptor de la misma
	descriptor_inotify_ = inotify_init();
	
	//Si el proceso ha fallado, se termina con error
	if (descriptor_inotify_ < 0) return false;
			
	//En caso contrario, se guarda la ruta normalizada del directorio de registro
	directorio_registro_ = normalizarRuta(dirRegistro);
	
	//Si la ruta especificada no es válida, se cierra la instancia de inotify y se termina
	if (directorio_registro_ == "")
	{
		close(descriptor_inotify_);
		return false;
	}
	
	//Se comprueba que la ruta introducida sea un directorio
	struct stat buffer_stat;
	if (stat(&directorio_registro_[0], &buffer_stat) < 0)
	{
		directorio_registro_ = "";
		close(descriptor_inotify_);
		return false;
	}
	if (!S_ISDIR(buffer_stat.st_mode))
	{
		directorio_registro_ = "";
		close(descriptor_inotify_);
		return false;
	}
	
	//Se inicializa el buffer de lectura de inotify
	buffer_inotify_ = new char [LON_BUF_INOT_ * (sizeof(struct inotify_event) + NAME_MAX + 1)];
	
	//Se añade "/" al directorio_registro_ y se termina correctamente
	directorio_registro_ = directorio_registro_ + "/";
	return true;
}

bool MonitorDeFicheros::anadirFichero (const std::string& ruta)
{	
	//Se comprueba que la instancia de MonitorDeFicheros está inicializada
	if (!estaInicializado()) return false;
	
	//Se normaliza la ruta introducida por parámetro
	string ruta_canonica = normalizarRuta(ruta, directorio_registro_);
	
	//Si la normalización ha fallado (la ruta especificada no es válida) se termina con error
	if (ruta_canonica == "") return false;
	
	//Se comprueba que la ruta introducida sea un fichero regular
	struct stat buffer_stat;
	if (stat(&(directorio_registro_ + ruta_canonica)[0], &buffer_stat) < 0) return false;
	if (!S_ISREG(buffer_stat.st_mode)) return false;
		
	//Se crea e inicializa un nuevo Fichero
	unique_ptr<Fichero> nuevo_fichero (new Fichero());
	if (!nuevo_fichero->inicializar(directorio_registro_, ruta_canonica)) return false;
	
	//Se añade un watch a la instancia de inotify con objeto de iniciar la monitorización del fichero
	int descriptor_watch = inotify_add_watch(	descriptor_inotify_,
												&(directorio_registro_ + ruta_canonica)[0],
												IN_DELETE_SELF | IN_MODIFY | IN_MOVE_SELF	);
	
	//Se comprueba que el watch ha sido añadido correctamente
	if (descriptor_watch < 0) return false;
	
	//Se añade el nuevo Fichero a la lista en la posición correspondiente al descriptor del watch
	if (ficheros_vigilados_.size() == descriptor_watch) ficheros_vigilados_.push_back(move(nuevo_fichero));
	else if (ficheros_vigilados_.size() < descriptor_watch)
	{
		ficheros_vigilados_.resize(descriptor_watch);
		ficheros_vigilados_.push_back(move(nuevo_fichero));
	}
	else ficheros_vigilados_[descriptor_watch] = move(nuevo_fichero);
	
	//La función termina correctamente
	return true;
}

void MonitorDeFicheros::eliminarFichero (const std::string& ruta)
{
	//Se normaliza la ruta introducida por parámetro
	string ruta_canonica = normalizarRuta(ruta, directorio_registro_);
	
	//Se recorre la lista de ficheros hasta encontrar el especificado
	unsigned int i = 0;
	unsigned int indice_fichero;
	bool encontrado = false;
	while ((i < ficheros_vigilados_.size()) && !encontrado)
	{
		if (ficheros_vigilados_[i])
			if ((ficheros_vigilados_[i]->obtener_ruta() == ruta_canonica))
			{
				indice_fichero = i;
				encontrado = true;
			}
		++i;
	}
	
	//Si se ha encontrado el fichero, se elimina el fichero con el índice en cuestión
	if (encontrado) eliminarFichero(indice_fichero);
}

void MonitorDeFicheros::eliminarTodo (void)
{
	//Se recorre la lista de ficheros llamando a eliminarFichero para cada uno
	for (unsigned int i = 0; i < ficheros_vigilados_.size(); ++i) eliminarFichero(i);
}

int MonitorDeFicheros::obtenerNumeroDeFicheros (void)
{
	//Se recorre la lista de ficheros contando todos aquellos que no sean nulos
	int numero_de_ficheros = 0;
	for (unsigned int i = 0; i < ficheros_vigilados_.size(); ++i)
		if (ficheros_vigilados_[i]) ++numero_de_ficheros;
		
	//Se devuelve el número obtenido
	return numero_de_ficheros;
}

std::unique_ptr<Evento> MonitorDeFicheros::obtenerSiguienteEvento (void)
{
	//No todos los eventos generados por inotify son devueltos, algunos son sólo procesados internamente
	//obtenerSiguienteEvento sigue su ejecución hasta que se obtenga un evento válido para retorno
	struct inotify_event* aviso;
	string temporal;
	while (true)
	{
		//En primer lugar se lee un nuevo evento del buffer de inotify
		//Si el puntero de lectura del buffer de inotify ha alcanzado los bytes válidos contenidos en este,
		//hay que leer más eventos de la instancia de inotify
		if (puntero_buffer_inotify_ >= ocupado_buffer_inotify_)
		{
			puntero_buffer_inotify_ = 0;
			ocupado_buffer_inotify_ = read(	descriptor_inotify_,
											buffer_inotify_,
											LON_BUF_INOT_ * (sizeof(struct inotify_event) + NAME_MAX + 1)	);
			
			//Si la lectura ha fallado, termina devolviendo un valor nulo
			if (ocupado_buffer_inotify_ < 0) return nullptr;
		}
		
		//Se lee el siguiente aviso del buffer
		aviso = (struct inotify_event*) &buffer_inotify_[puntero_buffer_inotify_];
		puntero_buffer_inotify_ = puntero_buffer_inotify_ + sizeof(struct inotify_event) + aviso->len;
		
		//Se procesa el aviso obtenido
		//Normalmente los ficheros esperan eventos de tipo IN_MODIFY, IN_DELETE_SELF e IN_MOVE_SELF
		if (aviso->mask == IN_MODIFY)
		{
			//Si es IN_MODIFY, se obtiene la última entrada de datos al fichero, y se devuelve un nuevo
			//Evento con los datos del mismo (excepto si los datos son "", en cuyo caso se ignora; ya se
			//tratarán las posibles situaciones que puedan dar lugar a ello con otros avisos de inotify)
			temporal = ficheros_vigilados_[aviso->wd]->ultimaModificacion(directorio_registro_);
			if (temporal != "")
				return unique_ptr<Evento>(new Evento (	ficheros_vigilados_[aviso->wd]->obtener_nombre(),
														directorio_registro_ +
															ficheros_vigilados_[aviso->wd]->obtener_ubicacion(),
														temporal	));
		}
		else if ((aviso->mask == IN_DELETE_SELF) || (aviso->mask == IN_MOVE_SELF))
		{
			//Si el fichero ha sido borrado o renombrado, hay que rotar el fichero
			//Se retira el watch de inotify para dejar de escuchar eventos temporalmente
			inotify_rm_watch(descriptor_inotify_, aviso->wd);
			
			//Se mueve el fichero de la lista de ficheros vigilados a rotación
			iniciarRotacionFichero(move(ficheros_vigilados_[aviso->wd]));
		}
		else if ((aviso->mask == IN_CREATE) || (aviso->mask == IN_MOVED_TO))
		{
			//Si el aviso es IN_CREATE o IN_MOVED_TO, significa que proviene de un directorio en el que hay
			//ficheros en rotación. Una llamada a finalizarRotacionFichero terminará el proceso de rotación
			//para el fichero en cuestión si el aviso recibido corresponde efectivamente a uno de estos
			finalizarRotacionFichero(aviso->wd, aviso->name);
		}
	}
}

void MonitorDeFicheros::iniciarRotacionFichero (std::unique_ptr<Fichero> fichero)
{
	//Se buscan otros ficheros con la misma ubicación que ya estén en rotación
	unsigned int i = 0;
	while (i < ficheros_en_rotacion_.size())
	{
		if (!ficheros_en_rotacion_[i].empty() &&
			(ficheros_en_rotacion_[i].front()->obtener_ubicacion() == fichero->obtener_ubicacion()))
		{
			//Si los encuentra, simplemente mueve el fichero con ellos y termina la funcion
			ficheros_en_rotacion_[i].push_back(move(fichero));
			return;
		}
		else ++i;
	}
	
	//Si no ha encontrado ninguno, crea un nuevo observador de inotify para su ubicación, y mueve
	//el fichero a los ficheros_en_rotacion_ a un nuevo indice correspondiente al descriptor obtenido
	int descriptor_watch = inotify_add_watch(	descriptor_inotify_,
												&(directorio_registro_ + fichero->obtener_ubicacion())[0],
												IN_CREATE | IN_MOVED_TO	);
	
	//Se comprueba que la creación del watch haya sido posible
	if (descriptor_watch >= 0)
	{
		//Si el tamaño del primer nivel de la lista de ficheros (ubicaciones) es igual al descriptor, se
		//añade en una nueva lista de 2º nivel en la última posición
		if (ficheros_en_rotacion_.size() == descriptor_watch)
		{
			ficheros_en_rotacion_.emplace_back();
			ficheros_en_rotacion_.back().push_back(move(fichero));
		}
		//Si el descriptor es mayor, se redimensiona a ese tamaño antes de hacer lo mismo
		else if (ficheros_en_rotacion_.size() < descriptor_watch)
		{
			ficheros_en_rotacion_.resize(descriptor_watch);
			ficheros_en_rotacion_.emplace_back();
			ficheros_en_rotacion_.back().push_back(move(fichero));
		}
		//Finalmente si es menor, simplemente se añade en la posición adecuada
		else ficheros_en_rotacion_[descriptor_watch].push_back(move(fichero));
	}
}

void MonitorDeFicheros::finalizarRotacionFichero (unsigned int indice, const std::string& nombre)
{
	//Si no hay ficheros asociados a ese indice, se retira el watch y se termina la función
	if (ficheros_en_rotacion_[indice].empty())
	{
		inotify_rm_watch(descriptor_inotify_, indice);
		return;
	}
	//Se localiza el fichero en cuestión si existe
	for (	auto iterador = ficheros_en_rotacion_[indice].begin();
			iterador != ficheros_en_rotacion_[indice].end();
			++iterador	)
	{
		if ((*iterador)->obtener_nombre() == nombre)
		{
			//Si el fichero es localizado, se vuelve a intentar añadir el fichero y si se consigue se extrae
			//la copia previa (eliminando el watch a la ubicación si era el útimo) antes de terminar la función
			if (anadirFichero((*iterador)->obtener_ruta()))
			{
				ficheros_en_rotacion_[indice].erase(iterador);
				if (ficheros_en_rotacion_[indice].empty()) inotify_rm_watch(descriptor_inotify_, indice);
			}
			return;
		}
	}
}

void MonitorDeFicheros::eliminarFichero (unsigned int indice)
{
	//Se comprueba si la entrada especificada es válida
	if (indice < ficheros_vigilados_.size())
		if (ficheros_vigilados_[indice])
		{
			//Si la entrada es válida, se retira el watch de inotify
			inotify_rm_watch(descriptor_inotify_, indice);
			
			//Y se anula el fichero de la lista de la lista de vigilancia
			ficheros_vigilados_[indice] = nullptr;
		}
}

std::string MonitorDeFicheros::normalizarRuta (const std::string& ruta)
{
	//Se utiliza la función realpath() para obtener la ruta canónica equivalente
	char* ruta_canonica = realpath(&ruta[0], nullptr);
	
	//Si el intento falla, la función termina en ""
	if (ruta_canonica == nullptr) return "";
	
	//En caso contrario, se obtiene el string correspondiente y se libera la memoria reservada por realpath
	string ruta_devolver (ruta_canonica);
	free(ruta_canonica);
	return ruta_devolver;
}

std::string MonitorDeFicheros::normalizarRuta (const std::string& ruta, const std::string& rutaRaiz)
{
	//Se normaliza la rutaRaiz para garantizar que es canónica
	string ruta_raiz_canonica = normalizarRuta (rutaRaiz);
	
	//Si la ruta raíz no es válida, la función termina en ""
	if (ruta_raiz_canonica == "") return "";
	
	//A continuación, se normaliza la concatenación de la ruta raíz normalizada y la ruta proporcionada
	string ruta_canonica = normalizarRuta (ruta_raiz_canonica + "/" + ruta);
	
	//Si la ruta obtenida no es válida, termina en ""
	if (ruta_canonica == "") return "";
	
	//Una vez se dispone de ambas rutas absolutas canónicas, se substrae la parte correspondiente a la raíz
	//y se devuelve el resultado
	return ruta_canonica.substr(ruta_raiz_canonica.length() + 1, string::npos);
}

} //namespace lognotify