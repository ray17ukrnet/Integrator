#ifndef __INTEGRATOR_SERVER_COMMAND_H
#define __INTEGRATOR_SERVER_COMMAND_H

#include "Requests/BaseServerCommand.h"

//******************************************************************
// Класс для выполнения комманд управления сервером
//******************************************************************
class IntegratorServerCommand : public TBaseServerCommand
{
public:

	//------------------------------------------------------------------
	// Конструктор
	//------------------------------------------------------------------
	IntegratorServerCommand(void);

	//------------------------------------------------------------------
	// Деструктор
	//------------------------------------------------------------------
	virtual ~IntegratorServerCommand(void);


	//------------------------------------------------------------------
	// Выполнить комманду
	//------------------------------------------------------------------
	virtual int ProcessServerCommand(
		const std::string &command,

		std::string &message
	);

protected:

	// Обработчик запросов по универсальному милу и дангу
	autoMillExtAcceptor UniKioskAcceptor;


};

// Автоматический указатель на класс для выполнения команд сервера
typedef std::auto_ptr<IntegratorServerCommand> autoIntegratorServerCommand;

#endif // __INTEGRATOR_SERVER_COMMAND_H
