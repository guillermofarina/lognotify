/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include <iostream>
#include <string>
#include <vector>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "cliente_de_notificaciones.h"

using namespace std;
using namespace lognotify;

/**
* Punto de entrada de la aplicación cliente de Lognotify
* @param argc Número de parámetros pasados por línea de comandos al ejecutar la aplicación
* @param argv Lista de parámetros pasados por línea de comandos al ejecutar la aplicación
*/
int main (int argc, char** argv)
{
	//Se inicializan los valores por defecto de las opciones de ejecución
	bool demonio = false;
	string ruta_ficheros = "$HOME/.lognotify";
	int tiempo_expiracion = -1;
	int num_sesiones_historial = 5;
	bool mostrar_ruta_completa = false;
	bool mostrar_remitente = false;
	bool mostrar_ayuda = false;
	bool error_parametros = false;
	
	//Se procesan los parámetros de línea de comandos
	opterr = 0;
	int opcion = 0;
	while ((opcion = getopt(argc, argv, "df:t:s:roh")) != -1)
	{
		switch (opcion)
		{
			case 'd':
				demonio = true;
				break;
			case 'f':
				ruta_ficheros = optarg;
				break;
			case 't':
				tiempo_expiracion = atoi(optarg);
				if (tiempo_expiracion < 0) error_parametros = true;
				break;
			case 's':
				num_sesiones_historial = atoi(optarg);
				if (num_sesiones_historial < 0) error_parametros = true;
				break;
			case 'r':
				mostrar_ruta_completa = true;
				break;
			case 'o':
				mostrar_remitente = true;
				break;
			case 'h':
				mostrar_ayuda = true;
				break;
			case '?':
				error_parametros = true;
				break;
			case ':':
				error_parametros = true;
				break;
			default:
				return -1;
		}
	}
	if (optind < argc) error_parametros = true;
		
	//Si ha habido errores o se ha pedido la ayuda, se muestra en pantalla y se termina
	if (error_parametros) cout << "El formato de los parámetros es incorrecto" << endl;
	if (error_parametros || mostrar_ayuda)
	{
		cout << "La aplicación lognotifycli soporta los siguientes parámetros:" << endl;
		cout << "-d Ejecutar lognotifycli como demonio" << endl;
		cout << "-f Especificar ruta alternativa a $HOME/.lognotify (ej. -f /mis/ficheros/)" << endl;
		cout << "-t Especificar un tiempo de expiración de notificaciones en ms (ej. -t 5000)" << endl;
		cout << "-s Especificar el número de historiales de sesión a conservar (ej. -s 5)" << endl;
		cout << "-r Mostrar ruta completa de los ficheros en notificaciones" << endl;
		cout << "-o Mostrar dirección del servidor origen en notificaciones" << endl;
		cout << "-h Mostrar ayuda de ejecución de lognotifycli (no se ejecutará el programa)" << endl;
		return 0;
	}
	
	//Si se ha pedido ejecutar el programa como demonio, se realiza el proceso correspondiente
	if (demonio)
	{
		pid_t pid, sid;
        
        //Fork del proceso padre
        pid = fork();
        if (pid < 0) return -1;
			
        //Se termina el proceso padre
        if (pid > 0) return 0;

        //Se cambia la máscara de ficheros
        umask(0);
                
        //Se crea un nuevo ID de sesión para el proceso
        sid = setsid();
        if (sid < 0) return -1;
        
        //Se cambia el directorio activo
        if ((chdir("/")) < 0) return -1;
        
        //Se cierran los descriptores de fichero estándar
        close(STDIN_FILENO);
        close(STDOUT_FILENO);
        close(STDERR_FILENO);
	}
		
	//Se crea e inicializa una instancia de ClienteDeNotificaciones, poniéndola a hacer su función
	ClienteDeNotificaciones cliente ("Lognotify");
	if (!cliente.inicializar	(	ruta_ficheros + "/servidores",
									ruta_ficheros + "/filtro",
									ruta_ficheros + "/historial",
									tiempo_expiracion,
									mostrar_ruta_completa,
									mostrar_remitente,
									num_sesiones_historial	))
	{
		if (!demonio) cout << "No se ha podido inicializar Lognotify. Es posible que no se haya proporcionado una lista de 1+ servidores válidos a la que conectar en el fichero \"servidores\" o que todos ellos estén fuera de servicio" << endl;
		return -1;
	}
			
	return 0;
}