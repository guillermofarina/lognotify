/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include "historial.h"

#include <string>
#include <fstream>
//#include <iomanip>
#include <chrono>
#include <ctime>
#include <cstdio>

#include "evento.h"

using namespace std;
namespace lognotify
{

Historial::Historial (void):
	ruta_historial_(""),
	inicializado_(false),
	numero_sesiones_antiguas_(NUM_SESIONES_POR_DEFECTO) {}

bool Historial::inicializar (const std::string& ruta)
{
	//Si ya está inicializado, devuelve false
	if (estaInicializado()) return false;
	
	//Si se permite guardar sesiones antiguas se hace rotación de ficheros
	if (numero_sesiones_antiguas_ > 0) rotarFicheros (ruta);
	
	//Se crea un nuevo fichero vacío correspondiente al historial en el que se guardará la sesión,
	//vaciando cualquier fichero ya existente con el mismo nombre, y luego se cierra
	ofstream fichero (ruta, ios::out | ios::trunc);
	if (!fichero.is_open()) return false;
	fichero.close();
	
	//Si el proceso ha ido correctamente, se guarda la ruta e inicializado y se termina en éxito
	ruta_historial_ = ruta;
	inicializado_ = true;
	return true;
}

bool Historial::registrarEvento (Evento& evento)
{
	//Se abre el fichero como stream para escritura añadiendo al final de lo que ya tenga escrito
	ofstream fichero (ruta_historial_, ios::out | ios::app);
	if (!fichero.is_open()) return false;
	
	//Se escribe un nueva entrada al historial, con el formato:
	//YYYY-MM-DD HH:MM:SS	XXX.XXX.XXX.XXX/ppppp	ubicacion/fichero
	//Descripción del evento...
	char buffer_tiempo [20];
	time_t ahora = chrono::system_clock::to_time_t(chrono::system_clock::now());
	//Solución original al formateo de fecha/hora con std::put_time de C++11 (iomanip) no soportado en g++ 4.9.2
	//fichero << put_time(localtime(&ahora), "%Y-%m-%d %T");	
	strftime(buffer_tiempo, 20, "%Y-%m-%d %H:%M:%S", localtime(&ahora));
	fichero << &buffer_tiempo[0];
	fichero << "\t" << evento.obtener_direccion() << "/" << evento.obtener_puerto();
	fichero << "\t" << evento.obtener_ubicacion() << evento.obtener_nombre() << endl;
	fichero << evento.obtener_descripcion() << endl << endl;
	
	//Se cierra el fichero una vez finalizada la operación y se termina en éxito
	fichero.close();
	return true;
}

void Historial::rotarFicheros (const std::string& ruta)
{
	FILE *fichero;
	
	//Se elimina el historial más antiguo dentro del máximo a mantener si existe
	fichero = fopen(&(ruta + "." + to_string(numero_sesiones_antiguas_))[0], "r+");
	if (fichero != nullptr)
	{
		//Si el fichero existe, se elimina
		fclose(fichero);
		remove(&(ruta + "." + to_string(numero_sesiones_antiguas_))[0]);
	}
	
	//Se recorren todos los ficheros antiguos desde el anteriormente eliminado hasta el actual
	//renombrándolos un número más del que tenían
	for (unsigned int i = numero_sesiones_antiguas_ - 1; i > 0; --i)
	{
		fichero = fopen(&(ruta + "." + to_string(i))[0], "r+");
		if (fichero != nullptr)
		{
			//si el fichero existe, se renombra a un número más
			fclose(fichero);
			rename(&(ruta + "." + to_string(i))[0], &(ruta + "." + to_string(i+1))[0]);
		}
	}
	
	//Se renombra por último el fichero donde se almacenaba el historial actual como la primera copia,
	//liberando la ruta para que el historial pueda empezar esta sesión con un fichero vacío
	fichero = fopen(&ruta[0], "r+");
	if (fichero != nullptr)
	{
		//Si el fichero existe, se renombra con la terminación ".1"
		fclose(fichero);
		rename(&ruta[0], &(ruta + ".1")[0]);
	}
}

} //namespace lognotify