/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include "cliente_de_notificaciones.h"

#include <string>
#include <vector>
#include <fstream>
#include <regex>

#include "centro_de_notificaciones.h"
#include "servidor.h"

using namespace std;
namespace lognotify
{

ClienteDeNotificaciones::ClienteDeNotificaciones (const std::string& nombreDeAplicacion):
	nombre_de_aplicacion_ (nombreDeAplicacion),
	inicializada_ (false) {}

bool ClienteDeNotificaciones::inicializar	(	const std::string& rutaFicheroServidores,
												const std::string& rutaFicheroFiltro,
												const std::string& rutaFicheroHistorial,
												int tiempoExpiracionNotificaciones,
												bool mostrarRutaCompletaNotificaciones,
												bool mostrarRemitenteNotificaciones,
												int numeroSesionesAntiguas )
{
	//Se inicializa el notificador; en caso de problemas, se retorna false
	if (!notificador_.inicializar(nombre_de_aplicacion_, rutaFicheroFiltro)) return false;
	
	//Se configura el notificador con en base a los parámetros recibidos
	notificador_.establecer_tiempo_visible(tiempoExpiracionNotificaciones);
	notificador_.establecer_mostrar_ruta_completa(mostrarRutaCompletaNotificaciones);
	notificador_.establecer_anadir_remitente(mostrarRemitenteNotificaciones);
	notificador_.establecer_numero_de_historiales_antiguos(numeroSesionesAntiguas);
	notificador_.establecer_historial_de_sesion(rutaFicheroHistorial);
	
	//Se parsea el fichero de servidores, conectando a cada uno de los especificados
	ifstream fichero_servidores;
	fichero_servidores.open(rutaFicheroServidores);
	if (!fichero_servidores.is_open()) return false;
	
	string linea = "";
	string direccion = "";
	string puerto = "";
	while (getline(fichero_servidores, linea))
	{
		//if (regex_match(linea, regex("[.]+/[\\d]+")))
		if (regex_match(linea, regex("[^/]+/[\\d]+")))
		{
			direccion = linea.substr(0, linea.find_first_of('/'));
			puerto = linea.substr(linea.find_first_of('/') + 1, string::npos);
			conectarServidor(anadirServidor(direccion, puerto));
			
		}
		
	}
	fichero_servidores.close();
	
	//Si la lista de proveedores está vacía, se retorna error para informar de que ha habido algún problema
	if (proveedores_.empty()) return false;
	
	//Se espera a que no quede ningún Servidor activo para finalizar
	for (unsigned int i = 0; i < proveedores_.size(); ++i) proveedores_[i].esperar();
	return true;
}
							
int ClienteDeNotificaciones::anadirServidor (const std::string& direccion, const std::string& puerto)
{
	//Se crea un nuevo servidor, y se añade a la lista de proveedores de notificaciones
	proveedores_.push_back(move(*(new Servidor(direccion, puerto))));
	return proveedores_.size() - 1;
}

bool ClienteDeNotificaciones::conectarServidor (const int identificador)
{
	return proveedores_[identificador].conectar(notificador_);
}

void ClienteDeNotificaciones::desconectarServidor (const int identificador)
{
	proveedores_[identificador].desconectar();
}

} //namespace lognotify