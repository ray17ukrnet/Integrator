#include "integrator_pch.h"

// Парсинг запросов
#include "Requests/BaseWebserverRequests.h"

#include "IntegratorConsoleListener.h"

IntegConsoleListener::IntegConsoleListener(ACE_Thread_Manager* t) : BaseConsoleListener(t)
{
	SetEndMarker(String_To_DataBlock(BASE_ETX));
	isNonRestrictedAcceptCount = true;
	serverCommand.reset(new IntegratorServerCommand());
}

IntegConsoleListener::IntegConsoleListener(TAO_ORB_Core *orb_core) : BaseConsoleListener(orb_core)
{
	SetEndMarker(String_To_DataBlock(BASE_ETX));
	isNonRestrictedAcceptCount = true;
	serverCommand.reset(new IntegratorServerCommand());
}

//-------------------------------------------------------------------------
// Обрабатываем полученную телеграмму и формируем ответ
// Вход
//	RequestText - запрос для обработки
// Выход 
//	ResponseText - ответная телеграмма 
//	результат функции - результат выполнения
//-------------------------------------------------------------------------
int IntegConsoleListener::Processing(std::string RequestText, std::string &ResponseText)
{
	return serverCommand->ProcessServerCommand(RequestText, ResponseText);
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class ACE_Thread_Strategy<UniConsoleListener>;
template class ACE_Strategy_Acceptor<UniConsoleListener, ACE_SOCK_ACCEPTOR>;
template class ACE_Creation_Strategy<UniConsoleListener>;
template class ACE_Acceptor<UniConsoleListener, ACE_SOCK_ACCEPTOR>;
template class ACE_Accept_Strategy<UniConsoleListener, ACE_SOCK_ACCEPTOR>;
template class ACE_Concurrency_Strategy<UniConsoleListener>;
template class ACE_Scheduling_Strategy<UniConsoleListener>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate ACE_Thread_Strategy<UniConsoleListener>
#pragma instantiate ACE_Strategy_Acceptor<UniConsoleListener, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Creation_Strategy<UniConsoleListener>
#pragma instantiate ACE_Acceptor<UniConsoleListener, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Accept_Strategy<UniConsoleListener, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Concurrency_Strategy<UniConsoleListener>
#pragma instantiate ACE_Scheduling_Strategy<UniConsoleListener>

#endif // ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION 
