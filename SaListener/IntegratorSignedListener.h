#ifndef __INTEGRATOR_SIGNED_LISTENER_H__
#define __INTEGRATOR_SIGNED_LISTENER_H__

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

// Обработчик запросов по универсальному интерфейсу
#include "Requests/IntegratorMillExtAcceptor.h"

#include "SaListener/BaseMarkerSaListener.h"


//*************************************************************************
//* Класс для приема входящих телеграмм по сокету                         *
//*************************************************************************
class GivsSignedListener : public BaseMarkerSaListener//IP_Server_Connection_Handler
{
public:

	GivsSignedListener (ACE_Thread_Manager* t = 0);
	GivsSignedListener (TAO_ORB_Core *orb_core);

protected:

  // Обработчик запросов по универсальному милу и дангу
  autoMillExtAcceptor           UniKioskAcceptor;

  //-------------------------------------------------------------------------
	// Возвращает название запроса
	// Вход
	//	Request - Текст запроса
	// Выход 
	//	результат функции - описание типа запроса
	//-------------------------------------------------------------------------
	virtual std::string GetRequestName (std::string RequestText);

	//-------------------------------------------------------------------------
	// Обрабатываем полученную телеграмму и формируем ответ
	// Вход
	//	RequestText - запрос для обработки
	// Выход 
	//	ResponseText - ответная телеграмма 
	//	результат функции - результат выполнения
	//-------------------------------------------------------------------------
	virtual int Processing (std::string RequestText, std::string &ResponseText);

};

// Инстантиирование фабрики акцептора входящих соединений
typedef ACE_Strategy_Acceptor<GivsSignedListener,ACE_SOCK_ACCEPTOR> Givs_Signed_Listener_ACCEPTOR;

#endif // __INTEGRATOR_SIGNED_LISTENER_H__
