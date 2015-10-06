/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include "regla.h"

#include <vector>
#include <memory>

#include "condicion.h"
#include "evento.h"

using namespace std;
namespace lognotify
{

void Regla::anadirCondicion (std::unique_ptr<Condicion> condicion)
{
	condiciones_.push_back(move(condicion));
}

bool Regla::evaluar (Evento& evento)
{
	//Se evalúan una por una todas las condiciones que conforman la regla, en el momento en que una de ellas
	//evalúe negativamente (false), la evaluación de la regla será también false y se retornará tal resultado
	for (unsigned int i = 0; i < condiciones_.size(); ++i)
		if (!condiciones_[i]->evaluar(evento)) return false;
		
	//Si todas las condiciones han evaluado positivamente (true), la regla también lo hace
	return true;
}

} //namespace lognotify