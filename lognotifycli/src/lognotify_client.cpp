/*
* logNotify - Monitorización de ficheros de registro en GNU Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include <iostream>
#include <string>
#include <vector>

#include "centro_de_notificaciones.h"
#include "servidor.h"

using namespace std;
using namespace lognotify;

/**
* Punto de entrada de la aplicación cliente de logNotify
* @param argc Número de parámetros pasados por línea de comandos al ejecutar la aplicación
* @param argv Lista de parámetros pasados por línea de comandos al ejecutar la aplicación
*/
int main (int argc, char** argv)
{
	CentroDeNotificaciones notificador;
	if (!notificador.inicializar("logNotify"))
	{
		cout << "¡Error al inicializar el centro de notificaciones!" << endl;
		return 1;
	}
	if (!notificador.establecer_historial_de_sesion("./files/historial"))
	{
		cout << "¡Error al inicializar el historial!" << endl;
		return 2;
	}
	
	vector<Servidor> servidores;
	
	string direccion, puerto;
	while (true)
	{
		cout << "Introduce una dirección IP para conectar a un nuevo servidor: ";
		cin >> direccion;
		cout << endl << "Introduce el puerto en el que espera dicho servidor: ";
		cin >> puerto;
		cout << endl;
		
		servidores.push_back(move(*(new Servidor(direccion, puerto))));
		if (!servidores.back().conectar(notificador))
		{
			cout << "No se ha podido conectar con el servidor" << endl;
			servidores.pop_back();
		}
		else cout << "¡Conexión establecida correctamente!" << endl;
	}
		
	return 0;
}