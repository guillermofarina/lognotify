/*
* logNotify - Monitorización de ficheros de registro en GNU Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <climits>

#include "servidor_de_notificaciones.h"

using namespace std;
using namespace lognotify;

/**
* Punto de entrada de la aplicación servidora de logNotify
* @param argc Número de parámetros pasados por línea de comandos al ejecutar la aplicación
* @param argv Lista de parámetros pasados por línea de comandos al ejecutar la aplicación. Puede recibir: \n
*	-p número > Puerto TCP (rango recomendado 49152-65535*) en el que el servidor escucha conexiones entrantes \n
*	-d ruta > Ruta absoluta al directorio donde el sistema mantiene los ficheros de registro (ej. "/var/log/") \n
*	-f ruta1 ruta2 ruta3 ... > Cualquier número de rutas relativas al directorio de registros con ficheros a monitorizar \n
*	-h > Muestra ayuda en pantalla sobre cómo ejecutar logNotify desde línea de comandos\n
* *Se recomienda el rango 49152 a 65535 para puerto al ser el rango destinado para puertos Privados y/o Dinámicos
* por IANA (http://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml), siendo
* por tanto la opción más segura. No obstante, logNotify permitirá especificar cualquier puerto entre 0-65535
*/
int main (int argc, char** argv)
{
	unsigned short puerto;
	bool puerto_inicializado = false;
	string dir_registro;
	vector<string> ficheros;
	bool mostrar_ayuda = false;
	
	//Se recorre la lista de parámetros de línea de comandos
	for (int i = 1; i < argc; ++i)
	{
		if (strcmp(argv[i], "-p") == 0)
		{
			//El siguiente es el puerto TCP
			++i;
			if (i >= argc) mostrar_ayuda = true;
			else
			{
				if (stoul(argv[i]) < USHRT_MAX)
				{
					puerto = (unsigned short) stoul(argv[i]);
					puerto_inicializado = true;
				}
				else
				{
					mostrar_ayuda = true;
					i = argc;
				}
			}
		}
		else if (strcmp(argv[i], "-d") == 0)
		{
			//El siguiente es la ruta del directorio de ficheros de registro
			++i;
			if (i >= argc) mostrar_ayuda = true;
			else dir_registro = argv[i];
		}
		else if (strcmp(argv[i], "-f") == 0)
		{
			//Los siguientes son los ficheros que desea monitorizarse
			++i;
			if (i >= argc) mostrar_ayuda = true;
			else
			{
				while ((i < argc) && (argv[i][0] != '-'))
				{
					ficheros.push_back(argv[i]);
					++i;
				}	
			}
		}
		else
		{
			//El parámetro introducido es -h o un comando no válido
			mostrar_ayuda = true;
			i = argc;
		}
	}
	
	if (mostrar_ayuda || !puerto_inicializado || (dir_registro == "") || (ficheros.size() == 0))
	{
		cout << endl << "Es necesario especificar los parámetros de configuración de logNotify:" << endl;
		cout << "\t-p número > Puerto TCP (rango recomendado 49152-65535*) en el que el servidor escucha conexiones entrantes" << endl;
		cout << "\t-d ruta > Ruta absoluta al directorio donde el sistema mantiene los ficheros de registro (ej. \"/var/log/\")" << endl;
		cout << "\t-f ruta1 ruta2 ruta3 ... > Cualquier número de rutas relativas al directorio de registros con ficheros a monitorizar" << endl;
		cout << "\t-h > Muestra ayuda en pantalla sobre cómo ejecutar logNotify desde línea de comandos" << endl;
		cout << "*Se recomienda el rango 49152 a 65535 para puerto al ser el rango destinado para puertos Privados y/o Dinámicos por IANA (http://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml), siendo por tanto la opción más segura. No obstante, logNotify permitirá especificar cualquier puerto entre 0-65535" << endl << endl;
		cout << "Ejemplo de ejecución: ~/logNotify_server -p 50000 -d /var/log/ -f auth.log messages kern.log" << endl << endl;
		
		return 0;
	}
	
	/*cout << "[Puerto]: " << puerto << endl << "[Directorio]: " << dir_registro << endl;
	for (unsigned int j = 0; j < ficheros.size(); j++)
		cout << "[Fichero " << j << "]: " << ficheros[j] << endl;*/

	ServidorDeNotificaciones servidor_lognotify;
	if (!servidor_lognotify.inicializar(puerto, dir_registro, ficheros))
	{
		cout << endl << "Fallo en la inicialización del servidor. Recuerda:" << endl;
		cout << "\tComprobar que se ha ejecutado el programa con suficientes privilegios para acceder a los ficheros de registro" << endl;
		cout << "\tComprobar que todos los parámetros se han introducido correctamente:" << endl << endl;
		cout << "\t-p número > Puerto TCP (rango recomendado 49152-65535*) en el que el servidor escucha conexiones entrantes" << endl;
		cout << "\t-d ruta > Ruta absoluta al directorio donde el sistema mantiene los ficheros de registro (ej. \"/var/log/\")" << endl;
		cout << "\t-f ruta1 ruta2 ruta3 ... > Cualquier número de rutas relativas al directorio de registros con ficheros a monitorizar" << endl;
		cout << "\t-h > Muestra ayuda en pantalla sobre cómo ejecutar logNotify desde línea de comandos" << endl;
		cout << "*Se recomienda el rango 49152 a 65535 para puerto al ser el rango destinado para puertos Privados y/o Dinámicos por IANA (http://www.iana.org/assignments/service-names-port-numbers/service-names-port-numbers.xhtml), siendo por tanto la opción más segura. No obstante, logNotify permitirá especificar cualquier puerto entre 0-65535" << endl << endl;
		cout << "Ejemplo de ejecución: ~/logNotify_server -p 50000 -d /var/log/ -f auth.log messages kern.log" << endl << endl;
		
		return 0;
	}
	servidor_lognotify.darServicio();
		
	return 0;
}