/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include "fichero.h"

#include <string>
#include <fstream>
#include <iostream>

using namespace std;
namespace lognotify
{
	
Fichero::Fichero (void):
		nombre_(""),
		ubicacion_(""),
		ultimo_tamano_(0) {}

bool Fichero::inicializar (const std::string& dirRegistro, const std::string& rutaFichero) {

	//En primer lugar, se abre el fichero para comprobar que la ruta proporcionada es válida
	ifstream fichero;
	fichero.open (dirRegistro + rutaFichero);
	if (!fichero.is_open())
	{
		//Si el fichero ha fallado al abrirse, la función termina con error
		return false;
	}
	
	//Si el fichero se ha abierto sin problemas, se guardan las posiciones inicial y final del mismo
	streampos inicio, fin;
	inicio = fichero.tellg();
	fichero.seekg(0, ios::end);
	fin = fichero.tellg();
	
	//Se cierra el fichero
	fichero.close();

	//Se guarda el tamaño del fichero como diferencia entre las posiciones final e inicial obtenidas
	ultimo_tamano_ = fin - inicio;
		
	//Se guarda la información de nombre y ubicación a partir de las rutas introducidas
	ubicacion_ = "";
	nombre_ = "";
	unsigned int i = 0;
	unsigned int j = 0;
	while (i < rutaFichero.length())
	{
		if (rutaFichero[i] == '/')
		{
			ubicacion_ = ubicacion_ + rutaFichero.substr(j, i - j + 1);
			j = i + 1;
		}
		++i;
	}
	nombre_ = rutaFichero.substr(j, i - j + 1);

	//La función termina correctamente
	return true;
}

std::string Fichero::ultimaModificacion (const std::string& dirRegistro) {
	
	//En primer lugar, se abre el fichero
	ifstream fichero;
	fichero.open (dirRegistro + ubicacion_ + nombre_);
	if (!fichero.is_open())
	{
		//Si el fichero ha fallado al abrirse, la función devuelve ""
		return "";
	}
	
	//Si el fichero se ha abierto sin problemas, se guardan las posiciones inicial y final del mismo
	streampos inicio, fin;
	inicio = fichero.tellg();
	fichero.seekg(0, ios::end);
	fin = fichero.tellg();
	
	//Se comprueba que el tamaño actual del fichero sea mayor que el último tamaño registrado (ultimo_tamano_)
	//De no ser así, es probable que el fichero haya sido truncado/vaciado/rotado, en cuyo caso la función
	//simplemente termina devolviendo una cadena vacía ("")
	if ((fin - inicio) <= ultimo_tamano_)
	{
		fichero.close();
		ultimo_tamano_ = fin - inicio;
		return "";
	}
	
	//Se desplaza el puntero a la última posición reportada del fichero
	fichero.seekg(ultimo_tamano_, ios::beg);
	
	//Se copia en destino todo el contenido hasta el final del fichero
	string linea;
	string destino = "";
	if (getline(fichero, linea)) destino = linea;
	while (getline(fichero, linea))
	{
		destino = destino + '\n' + linea;
	}
	
	//Se cierra el fichero una vez terminado con él
	fichero.close();
	
	//Se modifica el valor de ultimo_tamano_ para reflejar el proceso ya realizado
	ultimo_tamano_ = fin - inicio;
		
	//La función termina correctamente
	return destino;
}
	
} //namespace lognotify