/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include "condicion.h"

#include <string>
#include <regex>

#include "evento.h"

using namespace std;
namespace lognotify
{

Condicion::Condicion (const std::string& regex, bool signo):
	regex_(regex),
	signo_(signo) { }

bool CondicionDeFichero::evaluar (Evento& evento)
{
	return signo_ == regex_match(evento.obtener_nombre(), regex_);
}

bool CondicionDeRemitente::evaluar (Evento& evento)
{
	return signo_ == regex_match(evento.obtener_direccion(), regex_);
}

bool CondicionDeContenido::evaluar (Evento& evento)
{
	return signo_ == regex_match(evento.obtener_descripcion(), regex_);
}

} //namespace lognotify