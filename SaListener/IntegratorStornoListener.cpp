#include "integrator_pch.h"
/*
// Парсинг запросов
#include "Requests/BaseWebserverRequests.h"

// Обработка запросов универсального киоска
#include "UniKiosk/UniKioskRequests.h"

#include "IntegratorStornoListener.h"

GivsStornoListener::GivsStornoListener (ACE_Thread_Manager* t) : BaseMarkerSaListener (t)
{
	SetEndMarker (String_To_DataBlock(BASE_ETX));

  UniKioskAcceptor.reset(new StornoAcceptor());
}

GivsStornoListener::GivsStornoListener (TAO_ORB_Core *orb_core) : BaseMarkerSaListener (orb_core)
{
	SetEndMarker (String_To_DataBlock(BASE_ETX));

  UniKioskAcceptor.reset(new StornoAcceptor());
}

//-------------------------------------------------------------------------
// Возвращает название запроса
// Вход
//	Request - Текст запроса
// Выход 
//	результат функции - описание типа запроса
//-------------------------------------------------------------------------
std::string GivsStornoListener::GetRequestName (std::string RequestText)
{
	std::string					    Result;
  
  eUniKioskClientRequest  TelType;

  TWebRequestList 				RequestList;

  RequestList = ParseWebServerRequest (
    RequestText,
    BASE_PACKET_DIVIDER,
    BASE_ETX
    );

  TelType = String_To_eUniKioskClientRequest(RequestList[0]);

  Result = " - ";
  Result += Int64ToHex(TelType, 2, 10);
  Result += " ";
  Result += eUniKioskClientRequest_To_StringMessage (TelType);

	return Result;
}

//-------------------------------------------------------------------------
// Обрабатываем полученную телеграмму и формируем ответ
// Вход
//	RequestText - запрос для обработки
// Выход 
//	ResponseText - ответная телеграмма 
//	результат функции - результат выполнения
//-------------------------------------------------------------------------
int GivsStornoListener::Processing (std::string RequestText, std::string &ResponseText)
{
	TID                         ClassID = tidSA_LISTENER;
	std::string                 ProcedureName = "GivsStornoListener::Processing : ";
	int													Result = OK;

//	int													ExecResult;
	S_SERVER_ERROR							ServerError;

  ProcessResult = UniKioskAcceptor->AcceptRequest (RequestText, ResponseText);

	ProcessResult = Result;

	// Если ответная телеграмма не сформирована и есть ошибка выполнения запроса
	// то формируем телеграмму с ошибкой
	if ( (ResponseText == "") && (Result != OK) )  {
		ServerError.ErrorCode = IntToString(Result);
		ServerError.ErrorDescription = GetErrorMessageText(Result);

		ResponseText = ClientCreateServerError (
			ServerError
			);
		
		ResponseText += BASE_ETX;

//		Result = OK;
	}

	return Result;
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class ACE_Thread_Strategy<GivsStornoListener>;
template class ACE_Strategy_Acceptor<GivsStornoListener, ACE_SOCK_ACCEPTOR>;
template class ACE_Creation_Strategy<GivsStornoListener>;
template class ACE_Acceptor<GivsStornoListener, ACE_SOCK_ACCEPTOR>;
template class ACE_Accept_Strategy<GivsStornoListener, ACE_SOCK_ACCEPTOR>;
template class ACE_Concurrency_Strategy<GivsStornoListener>;
template class ACE_Scheduling_Strategy<GivsStornoListener>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate ACE_Thread_Strategy<GivsStornoListener>
#pragma instantiate ACE_Strategy_Acceptor<GivsStornoListener, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Creation_Strategy<GivsStornoListener>
#pragma instantiate ACE_Acceptor<GivsStornoListener, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Accept_Strategy<GivsStornoListener, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Concurrency_Strategy<GivsStornoListener>
#pragma instantiate ACE_Scheduling_Strategy<GivsStornoListener>

#endif // ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION 
*/
