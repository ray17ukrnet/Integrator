#ifndef __INTEGRATOR_CONSOLE_LISTENER_H__
#define __INTEGRATOR_CONSOLE_LISTENER_H__

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "Requests/IntegratorServerCommand.h"

#include "SaListener/BaseConsoleListener.h"

//*************************************************************************
//* Класс для приема входящих телеграмм по сокету                         *
//*************************************************************************
class IntegConsoleListener : public BaseConsoleListener
{
public:

	IntegConsoleListener(ACE_Thread_Manager* t = 0);
	IntegConsoleListener(TAO_ORB_Core *orb_core);

protected:

	// Обработка запросов комманд управления сервером
	autoIntegratorServerCommand     serverCommand;

	//-------------------------------------------------------------------------
	// Обрабатываем полученную телеграмму и формируем ответ
	// Вход
	//	RequestText - запрос для обработки
	// Выход 
	//	ResponseText - ответная телеграмма 
	//	результат функции - результат выполнения
	//-------------------------------------------------------------------------
	virtual int Processing(std::string RequestText, std::string &ResponseText);

};

// Инстантиирование фабрики акцептора входящих соединений
typedef ACE_Strategy_Acceptor<IntegConsoleListener, ACE_SOCK_ACCEPTOR> IntegConsole_Listener_ACCEPTOR;

#endif // __INTEGRATOR_CONSOLE_LISTENER_H__
