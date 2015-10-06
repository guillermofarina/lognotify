/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include "servidor_de_notificaciones.h"

#include <string>
#include <memory>

#include "monitor_de_ficheros.h"
#include "tabla_de_clientes.h"
#include "servidor_de_conexion.h"
#include "evento.h"
#include "mensaje.h"

using namespace std;
namespace lognotify
{

bool ServidorDeNotificaciones::inicializar (const unsigned short puerto,
											const std::string& dirRegistro,
											const std::vector<std::string> ficheros	)
{
	//Si ha sido previamente inicializado, termina con error
	if (esta_inicializado()) return false;
	
	//Se inicializa la tabla de clientes
	destinatarios_ = make_shared<TablaDeClientes>();
	
	//Se inicializa el servidor de conexión
	if (!proveedor_de_clientes_.inicializar(puerto, destinatarios_)) return false;
	
	//Se inicializa el monitor de ficheros
	if (!proveedor_de_eventos_.inicializar(dirRegistro)) return false;
	
	//Se añaden los ficheros pasados por parámetro al monitor de ficheros
	vector<string> no_abiertos;
	for (unsigned int i = 0; i < ficheros.size(); ++i)
		if (!proveedor_de_eventos_.anadirFichero(ficheros[i])) no_abiertos.push_back(ficheros[i]);
		
	//Se hace una segunda intentona con los ficheros no abiertos por si estaban temporalmente indisponibles
	for (unsigned int i = 0; i < no_abiertos.size(); ++i)
		proveedor_de_eventos_.anadirFichero(no_abiertos[i]);
		
	//Si no ha logrado abrir ningún fichero, termina con error
	if (proveedor_de_eventos_.obtenerNumeroDeFicheros() == 0) return false;
	
	return true;
}


void ServidorDeNotificaciones::darServicio (void)
{
	//Se empieza a aceptar la conexión de nuevos clientes
	if (!proveedor_de_clientes_.recibirClientes()) return;
	
	//Da comienzo la secuencia de obtención de nueva notificación -> envío a los clientes subscritos
	bool error = false;
	unique_ptr<Evento> evento;
	while (!error)
	{
		//Se toma el siguiente evento
		evento = proveedor_de_eventos_.obtenerSiguienteEvento();
		if (!evento) error = true;
		else
		{
			//Si no ha existido error, se envía el evento serializado a todos los destinatarios
			destinatarios_->enviar(make_shared<Mensaje> (serializarEvento(move(evento))));
		}
	}
}

Mensaje ServidorDeNotificaciones::serializarEvento (std::unique_ptr<Evento> evento)
{
	//Se calcula la longitud total del evento sumando la de cada campo, y +1 por cada uno para el caracter
	//separador (fin de cadena o '\0')
	unsigned int longitud_total 	= evento->obtener_nombre().length()
									+ evento->obtener_ubicacion().length()
									+ evento->obtener_descripcion().length() + 3;
	
	//Se copia el contenido de cada campo en un buffer de caracteres, incluído el caracter separador/fin de
	//cadena ('\0') tras cada uno								
	char *buffer = new char [longitud_total];
	strcpy(&buffer[0], &(evento->obtener_nombre())[0]);
	strcpy(&buffer[evento->obtener_nombre().length() + 1], &(evento->obtener_ubicacion())[0]);
	strcpy(	&buffer[evento->obtener_nombre().length() + evento->obtener_ubicacion().length() + 2],
			&(evento->obtener_descripcion())[0]	);

	//Una vez se dispone del contenido del evento serializado en el buffer, se crea y devuelve el nuevo mensaje
	Mensaje nuevo_mensaje (buffer, longitud_total);
	delete[] buffer;
	return nuevo_mensaje;
}

} //namespace lognotify