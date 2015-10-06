/*
* Lognotify - Monitorización de ficheros de registro en GNU/Linux con notificaciones de escritorio
* Autor: Guillermo Fariña Arroyo
* C++11 (ISO/IEC 14882:2011)
*/

#include "tabla_de_clientes.h"

#include <string>
#include <vector>
#include <memory>
#include <mutex>

#include "cliente.h"

using namespace std;
namespace lognotify
{

int TablaDeClientes::anadirCliente (const int descriptor_socket)
{
	//Se adquiere el mutex
	mutex_.lock();
	
	//Se llama a anadirClienteNoSeguro
	int identificador = anadirClienteNoSeguro(descriptor_socket);
	
	//Se libera el mutex
	mutex_.unlock();
	
	//Se devuelve el identificador obtenido
	return identificador;
}

void TablaDeClientes::eliminarCliente (const int identificador_cliente)
{
	//Se adquiere el mutex
	mutex_.lock();
	
	//Se llama a eliminarClienteNoSeguro
	eliminarClienteNoSeguro(identificador_cliente);
	
	//Se libera el mutex
	mutex_.unlock();
}

void TablaDeClientes::eliminarTodo (void)
{
	//Se adquiere el mutex
	mutex_.lock();
	
	//Se eliminan clientes de la lista hasta que no quede ninguno
	while (clientes_.size() > 0)
	{
		clientes_.back().terminarConexion();
		clientes_.pop_back();
	}
	
	//Se libera el mutex
	mutex_.unlock();
}

bool TablaDeClientes::enviar (std::shared_ptr<Mensaje> mensaje, const int identificador_cliente)
{
	bool enviado = false;
	
	//Se adquiere el mutex
	mutex_.lock();
	
	//Se comprueba que el identificador se corresponde a una entrada válida de la TablaDeClientes
	if (identificador_cliente < clientes_.size())
	{
		//Se solicita al Cliente especificado que envíe el Evento
		enviado = clientes_[identificador_cliente].enviar(move(mensaje));
		
		//Si el envío ha fallado, se asume que la conexión se ha perdido y se elimina el cliente
		if (!enviado) eliminarClienteNoSeguro(identificador_cliente);
	}
	
	//Se libera el mutex
	mutex_.unlock();
	
	//Se devuelve el resultado del envío
	return enviado;
}

bool TablaDeClientes::enviar (std::shared_ptr<Mensaje> mensaje)
{
	bool enviado = false;
	
	//Se adquiere el mutex
	mutex_.lock();
	
	//Se recorre la lista completa de clientes enviando el evento a cada uno de ellos
	for (int i = clientes_.size() - 1; i >= 0; --i)
	{
		//Si el envío tiene éxito se marca que se ha conseguido al menos un envío exitoso
		if (clientes_[i].enviar(mensaje)) enviado = true;
		
		//En caso contrario, se asume que la conexión se ha perdido y se elimina el cliente
		else eliminarClienteNoSeguro(i);
	}
	
	//Se libera el mutex
	mutex_.unlock();
	
	//Se devuelve el resultado del envío
	return enviado;
}

int TablaDeClientes::anadirClienteNoSeguro (const int descriptor_socket)
{
	//El cliente se inserta al final de la lista y se devuelve su posición
	Cliente nuevo_cliente (descriptor_socket);
	clientes_.push_back(move(nuevo_cliente));
	return clientes_.size() - 1;
}

void TablaDeClientes::eliminarClienteNoSeguro (const int identificador_cliente)
{
	//Se comprueba que el identificador de cliente dado sea válido
	if (identificador_cliente < clientes_.size())
	{
		//Si es así, se pide al cliente especificado que termine su conexión
		clientes_[identificador_cliente].terminarConexion();
		
		//A continuación, se copia el último de la lista a la posición del que se quiere eliminar, y se
		//elimina el último
		clientes_[identificador_cliente] = move(clientes_.back());
		clientes_.pop_back();
	}
}

} //namespace lognotify