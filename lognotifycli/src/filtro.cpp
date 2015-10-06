/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include "filtro.h"

#include <vector>
#include <memory>
#include <string>
#include <fstream>
#include <regex>

#include "regla.h"
#include "condicion.h"

using namespace std;
namespace lognotify
{

bool Filtro::inicializar (const std::string& rutaDeReglas)
{
	//Se comprueba que no haya sido inicializado con anterioridad
	if (estaInicializado()) return false;
	
	//Se cargan las reglas, si el proceso es correcto la instancia quedará inicializada, de no ser así,
	//termina en error
	inicializado_ = cargarReglas(rutaDeReglas);
	return inicializado_;
}

bool Filtro::evaluar (Evento& evento)
{
	//Se evalúan todas las reglas del filtro, si alguna de ellas se cumple, se devuelve false
	for (unsigned int i = 0; i < reglas_.size(); ++i)
		if (reglas_[i]->evaluar(evento)) return false;
		
	//Si ninguna regla se ha cumplido, el evento no debe ser omitido y pasa el filtro (se devuelve true)
	return true; 
}

bool Filtro::cargarReglas (const std::string& rutaDeReglas)
{
	//Se abre el fichero de reglas
	ifstream fichero_reglas;
	fichero_reglas.open(rutaDeReglas);
	if (!fichero_reglas.is_open()) return false;
	
	//Se parsea el fichero completo, línea por línea
	string linea = "";
	string expresion = "";
	while (getline(fichero_reglas, linea))
	{
		//Si la línea comienza con la palabra clave regla, se añade una nueva regla
		if (regex_match(linea, regex("[\\s\\t]*regla.*")))
		{
			unique_ptr<Regla> regla (new Regla ());
			reglas_.push_back(move(regla));
		}
		//Si la línea comienza con la palabra clave origen=, es una nueva condición de remitente igual a ...
		else if (regex_match(linea, regex("[\\s\\t]*origen=.*")))
		{
			//Si no hay una regla creada, se crea una nueva
			if (reglas_.empty())
			{
				unique_ptr<Regla> regla (new Regla ());
				reglas_.push_back(move(regla));
			}
			
			//Se extrae la expresión regular de la linea y se construye y añade la nueva condición
			expresion = linea.substr(linea.find_first_of('=') + 1, string::npos);
			unique_ptr<CondicionDeRemitente> condicion_rem (new CondicionDeRemitente(expresion, true));
			reglas_.back()->anadirCondicion(move(condicion_rem));
		}
		//Si la línea comienza con la palabra clave origen!=, es una nueva condición de remitente distinto de ...
		else if (regex_match(linea, regex("[\\s\\t]*origen!=.*")))
		{
			//Si no hay una regla creada, se crea una nueva
			if (reglas_.empty())
			{
				unique_ptr<Regla> regla (new Regla ());
				reglas_.push_back(move(regla));
			}
			
			//Se extrae la expresión regular de la linea y se construye y añade la nueva condición
			expresion = linea.substr(linea.find_first_of('=') + 1, string::npos);
			unique_ptr<CondicionDeRemitente> condicion_rem (new CondicionDeRemitente(expresion, false));
			reglas_.back()->anadirCondicion(move(condicion_rem));
		}
		//Si la línea comienza con la palabra clave fichero=, es una nueva condición de fichero igual a ...
		else if (regex_match(linea, regex("[\\s\\t]*fichero=.*")))
		{
			//Si no hay una regla creada, se crea una nueva
			if (reglas_.empty())
			{
				unique_ptr<Regla> regla (new Regla ());
				reglas_.push_back(move(regla));
			}
			
			//Se extrae la expresión regular de la linea y se construye y añade la nueva condición
			expresion = linea.substr(linea.find_first_of('=') + 1, string::npos);
			unique_ptr<CondicionDeFichero> condicion_fic (new CondicionDeFichero(expresion, true));
			reglas_.back()->anadirCondicion(move(condicion_fic));
		}
		//Si la línea comienza con la palabra clave fichero!=, es una nueva condición de fichero distinto de ...
		else if (regex_match(linea, regex("[\\s\\t]*fichero!=.*")))
		{
			//Si no hay una regla creada, se crea una nueva
			if (reglas_.empty())
			{
				unique_ptr<Regla> regla (new Regla ());
				reglas_.push_back(move(regla));
			}
			
			//Se extrae la expresión regular de la linea y se construye y añade la nueva condición
			expresion = linea.substr(linea.find_first_of('=') + 1, string::npos);
			unique_ptr<CondicionDeFichero> condicion_fic (new CondicionDeFichero(expresion, false));
			reglas_.back()->anadirCondicion(move(condicion_fic));
		}
		//Si la línea comienza con la palabra clave contenido, es una nueva condición de contenido igual a ...
		else if (regex_match(linea, regex("[\\s\\t]*contenido=.*")))
		{
			//Si no hay una regla creada, se crea una nueva
			if (reglas_.empty())
			{
				unique_ptr<Regla> regla (new Regla ());
				reglas_.push_back(move(regla));
			}
			
			//Se extrae la expresión regular de la linea y se construye y añade la nueva condición
			expresion = linea.substr(linea.find_first_of('=') + 1, string::npos);
			unique_ptr<CondicionDeContenido> condicion_con (new CondicionDeContenido(expresion, true));
			reglas_.back()->anadirCondicion(move(condicion_con));
		}
		//Si la línea comienza con la palabra clave contenido, es una nueva condición de contenido distinto de ...
		else if (regex_match(linea, regex("[\\s\\t]*contenido!=.*")))
		{
			//Si no hay una regla creada, se crea una nueva
			if (reglas_.empty())
			{
				unique_ptr<Regla> regla (new Regla ());
				reglas_.push_back(move(regla));
			}
			
			//Se extrae la expresión regular de la linea y se construye y añade la nueva condición
			expresion = linea.substr(linea.find_first_of('=') + 1, string::npos);
			unique_ptr<CondicionDeContenido> condicion_con (new CondicionDeContenido(expresion, false));
			reglas_.back()->anadirCondicion(move(condicion_con));
		}
		//En caso contrario, la expresión no puede ser reconocida y es ignorada
	}
	
	//Se cierra el fichero de reglas y se retorna con éxito
	fichero_reglas.close();
	return true;
}

} //namespace lognotify