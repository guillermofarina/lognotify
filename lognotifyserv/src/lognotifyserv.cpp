/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <regex>
#include <cstdlib>
#include <cstring>
#include <climits>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include "servidor_de_notificaciones.h"

using namespace std;
using namespace lognotify;

/**
* Punto de entrada de la aplicación servidora de Lognotify
* @param argc Número de parámetros pasados por línea de comandos al ejecutar la aplicación
* @param argv Lista de parámetros pasados por línea de comandos al ejecutar la aplicación
*/
int main (int argc, char** argv)
{
	//Se inicializan los valores por defecto de las opciones de ejecución
	bool demonio = false;
	unsigned short puerto = 0;
	string ruta_ficheros = "$HOME/.lognotify";
	string ruta_registro = "/var/log";
	bool mostrar_ayuda = false;
	bool error_parametros = false;
	
	//Se procesan los parámetros de línea de comandos
	opterr = 0;
	int opcion = 0;
	while ((opcion = getopt(argc, argv, "dp:f:w:h")) != -1)
	{
		switch (opcion)
		{
			case 'd':
				demonio = true;
				break;
			case 'p':
				if ((atoi(optarg) > 0) && (atoi(optarg) < USHRT_MAX)) puerto = (unsigned short) atoi(optarg);
				else error_parametros = true;
				break;
			case 'f':
				ruta_ficheros = optarg;
				break;
			case 'w':
				ruta_registro = optarg;
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
	if (puerto == 0) error_parametros = true;
		
	//Si ha habido errores o se ha pedido la ayuda, se muestra en pantalla y se termina
	if (error_parametros) cout << "El formato de los parámetros es incorrecto" << endl;
	if (error_parametros || mostrar_ayuda)
	{
		cout << "La aplicación lognotifyserv soporta los siguientes parámetros:" << endl;
		cout << "-p Especificar puerto TCP (este parámetro es necesario para ejecutar)" << endl;
		cout << "-d Ejecutar lognotifyserv como demonio" << endl;
		cout << "-f Especificar ruta alternativa a $HOME/.lognotify (ej. -f /mis/ficheros)" << endl;
		cout << "-w Especificar ruta alternativa a /var/log (ej. -w /mis/logs)" << endl;
		cout << "-h Mostrar ayuda de ejecución de lognotifyserv (no se ejecutará el programa)" << endl;
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
	
	//Se carga la lista de ficheros a monitorizar del fichero indicado
	vector<string> ficheros;
	ifstream fichero_ficheros_registro;
	string linea;
	fichero_ficheros_registro.open(ruta_ficheros + "/ficheros");
	if(!fichero_ficheros_registro.is_open())
	{
		if (!demonio) cout << "No se ha podido inicializar Lognotify. No se ha encontrado \'ficheros\' en la ubicación indicada, o ha sido imposible abrirlo";
		return -1;
	}
	while (getline(fichero_ficheros_registro, linea))
	{
		//Si la linea contiene un fragmento de texto, se interpreta como el nombre de un fichero y se guarda
		if (regex_match(linea, regex("[\\s\\t]*[^\\s\\t]+.*")))
		{
			ficheros.push_back(linea);
		}
	}
	fichero_ficheros_registro.close();
		
	//Se crea e inicializa una instancia de ServidorDeNotificaciones, poniéndola a hacer su función
	ServidorDeNotificaciones servidor;
	if (!servidor.inicializar(puerto, ruta_registro, ficheros))
	{
		if (!demonio) cout << "No se ha podido inicializar Lognotify. Es posible que no se haya proporcionado una lista de 1+ ficheros de registro que monitorizar en el fichero \"ficheros\" o que ninguno sea válido" << endl;
		return -1;
	}
	servidor.darServicio();
			
	return 0;
}