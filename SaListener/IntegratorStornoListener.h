#ifndef __UNI_STORNO_LISTENER_H__
#define __UNI_STORNO_LISTENER_H__

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */
/*
// Обработчик запросов по универсальному интерфейсу
#include "Requests/IntegratorStornoAcceptor.h"

#include "SaListener/BaseMarkerSaListener.h"


//*************************************************************************
//* Класс для приема входящих телеграмм по сокету                         *
//*************************************************************************
class GivsStornoListener : public BaseMarkerSaListener//IP_Server_Connection_Handler
{
public:

	GivsStornoListener (ACE_Thread_Manager* t = 0);
	GivsStornoListener (TAO_ORB_Core *orb_core);

protected:

  // Обработчик запросов
  autoStornoAcceptor           UniKioskAcceptor;

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
typedef ACE_Strategy_Acceptor<GivsStornoListener,ACE_SOCK_ACCEPTOR> Givs_Storno_Listener_ACCEPTOR;
*/
#endif // __UNI_STORNO_LISTENER_H__
