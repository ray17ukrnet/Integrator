#include "integrator_pch.h"

// Парсинг запросов
#include "Requests/BaseWebserverRequests.h"

// Обработка запросов универсального киоска
#include "UniKiosk/UniKioskRequests.h"

// Парсинг универсальных запросов
//#include "UniKioskRequests.h"

#include "IntegratorSignedListener.h"

GivsSignedListener::GivsSignedListener (ACE_Thread_Manager* t) : BaseMarkerSaListener (t)
{
	SetEndMarker (String_To_DataBlock(ETX_SIGNED));

  UniKioskAcceptor.reset(new MillExtAcceptor());
}

GivsSignedListener::GivsSignedListener (TAO_ORB_Core *orb_core) : BaseMarkerSaListener (orb_core)
{
	SetEndMarker (String_To_DataBlock(ETX_SIGNED));

  UniKioskAcceptor.reset(new MillExtAcceptor());
}

//-------------------------------------------------------------------------
// Возвращает название запроса
// Вход
//	Request - Текст запроса
// Выход 
//	результат функции - описание типа запроса
//-------------------------------------------------------------------------
std::string GivsSignedListener::GetRequestName (std::string RequestText)
{
	std::string					    Result;
  
  eUniKioskClientRequest  TelType;
//  int									    ExecResult;

  TWebRequestList 				RequestList;

  RequestList = ParseWebServerRequest (
    RequestText,
    BASE_PACKET_DIVIDER,
    BASE_ETX
    );

  TelType = String_To_eUniKioskClientRequest(RequestList[0]);

//   if (ExecResult != OK) {
//     Result = "!!!INVALIDE!!!";
// 
//     return Result;
//   }

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
int GivsSignedListener::Processing (std::string RequestText, std::string &ResponseText)
{
	TID                         ClassID = tidSA_LISTENER;
	std::string                 ProcedureName = "GivsSignedListener::Processing : ";
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
		
		ResponseText += ETX_SIGNED;

//		Result = OK;
	}

	return Result;
}

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)

template class ACE_Thread_Strategy<GivsSignedListener>;
template class ACE_Strategy_Acceptor<GivsSignedListener, ACE_SOCK_ACCEPTOR>;
template class ACE_Creation_Strategy<GivsSignedListener>;
template class ACE_Acceptor<GivsSignedListener, ACE_SOCK_ACCEPTOR>;
template class ACE_Accept_Strategy<GivsSignedListener, ACE_SOCK_ACCEPTOR>;
template class ACE_Concurrency_Strategy<GivsSignedListener>;
template class ACE_Scheduling_Strategy<GivsSignedListener>;

#elif defined (ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)

#pragma instantiate ACE_Thread_Strategy<GivsSignedListener>
#pragma instantiate ACE_Strategy_Acceptor<GivsSignedListener, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Creation_Strategy<GivsSignedListener>
#pragma instantiate ACE_Acceptor<GivsSignedListener, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Accept_Strategy<GivsSignedListener, ACE_SOCK_ACCEPTOR>
#pragma instantiate ACE_Concurrency_Strategy<GivsSignedListener>
#pragma instantiate ACE_Scheduling_Strategy<GivsSignedListener>

#endif // ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION 
