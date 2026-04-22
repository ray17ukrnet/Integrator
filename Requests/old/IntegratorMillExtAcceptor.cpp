#include "integrator_pch.h"

// Работа с парсингом запросов
#include "Requests/BaseWebserverRequests.h"

// Выполнения запросов по сети
#include "Http/ExecuteHttpRequest.h"

#include "IntegratorMillExtAcceptor.h"


//---------------------------------------------------------------------------
// Конструктор
//---------------------------------------------------------------------------
MillExtAcceptor::MillExtAcceptor (
              ) : IntegratorParserBase (
              )
{

}

//---------------------------------------------------------------------------
// Деструктор
//---------------------------------------------------------------------------
MillExtAcceptor::~MillExtAcceptor ()
{

}

//---------------------------------------------------------------------------
// Возвращает название оплаты для базы данных
//---------------------------------------------------------------------------
std::string MillExtAcceptor::GetBillName (void)
{
  std::string						Result;

  Result = PayBillReq.BODY.idService;
  Result += " Sum-" + PayBillReq.BODY.amountPayed;
  Result += " Client-" + PayBillReq.BODY.idClient;

  // Обрезаем сообщение до длины записи в бд
  if (Result.length() > 47) {
    Result = Result.substr(0, 47) + "...";
  }

  return Result;
}

//----------------------------------------------------------------------
// Выполнить запрос на внутреннем сервере
// Вход
//    inIdService - номер сервиса
//    inIdSubService - номер услуги
//		requestList - запрос
// Выход
//    responseList - ответ
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::execInternalServRequest (
  std::string                 inIdService,
  std::string                 inIdSubService,
  TWebRequestList             requestList,

  TWebRequestList	            &responseList
  )
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::execInternalServRequest : ";
  int											    Result = OK;

  std::string                 request;
  std::string                 response;
  std::string                 address;
  int                         port;
  ConnectHostGuard            HostCon;

  responseList.clear();

  // создаем запрос
  request = CreateWebServerResponse(
    requestList, 
    TelRecordDivider,
    Etx
    );

  // Берем адрес куда подключаться
  Result = DATABASE->GetInternalServAddress (
    inIdService,
    inIdSubService,

    address,
    port
    );

  CHECK_ERROR_RETURN(Result);

  Result = OpenTcpConnection(
		HostCon,
    address,
    port
		);

  CHECK_ERROR_RETURN(Result);

  Result = ExecTcpRequest(
    HostCon,
    Etx,
    Etx,
    request, 
    response
    );

  CHECK_ERROR_RETURN(Result);

  // Разбиваем телеграмму на отдельные поля
  responseList = ParseWebServerRequest(
    response, 
    TelRecordDivider,
    Etx
    );

  return Result;
}

//----------------------------------------------------------------------
// Создать значение запроса по умолчанию
//----------------------------------------------------------------------
std::string MillExtAcceptor::CreateDefaultValue (
  std::string				  value
  )
{
  std::string       result;

  if (value == "") {
    result = NullRecordValue;
  } else {
    result = value;
  }

  return result;
}

//----------------------------------------------------------------------
// Запросить счет
// Вход
//		
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::GetBillProvider (
  std::string				  inIdService,
  std::string				  inIdSubService,
  std::string				  inIdClient,
  std::string				  inIdTerminal,
  std::string				  inIdBuyer,

  eUniKioskStatus			&uniKioskStatus,
  std::string					&outProviderMessage,
  eBillType           &outBillType,
  std::string					&outAmountToPay,
  std::string					&outPrevCounter,
  std::string					&outKioskMessage,
  std::string					&outDopInfo
                                      )
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::GetBillProvider : ";
  int											    Result = OK;

  TWebRequestList             requestList;
  TWebRequestList	            responseList;
  eInterReqType               respType;

  Inform(MS
    << "\n....................."
    << "\nЗапрос счета - запрос"
    << "\n....................."
    << "\ninIdService = " << inIdService
    << "\ninIdSubService = " << inIdSubService
    << "\ninIdClient = " << inIdClient
    << "\ninIdTerminal = " << inIdTerminal
    << "\ninIdBuyer = " << inIdBuyer
    ,ClassID, mtMessage);

#ifdef EMULATE_INTEG_INTERNAL_REQUEST
  uniKioskStatus = uksOK;
  outProviderMessage = "*";
  outBillType = ebtCounterPeriod;
  outAmountToPay = GetRandomNumber(3);
  outPrevCounter = GetRandomNumber(4);
  outKioskMessage = "Test payment";
  outDopInfo = "";

#else // EMULATE_INTEG_INTERNAL_REQUEST

// 1. Тип запроса N 211
// 2. IdService A Номер провайдера услуг в системе банка Фамильный. 
// 3. IdSubService A Номер услуги. Если сервис не имеет внутреннего деления на услуги, то поле =0
// 4. IdClient А20 Идентификатор клиента
// 5. IdTerminal А Идентификатор терминала (по внутренней нумерации или внешней — это решает ИС. ПС не проверяет и не меняет его, только использует при необходимости)
// 6. idbuyer NN  Идентификатор байера

  int N = 0;

  requestList[N++] = IntToString(irGetBillReq);
  requestList[N++] = inIdService;
  requestList[N++] = inIdSubService;
  requestList[N++] = inIdClient;
  requestList[N++] = inIdTerminal;
  requestList[N++] = inIdBuyer;

  Result = execInternalServRequest (
    inIdService,
    inIdSubService,
    requestList,

    responseList
    );

  CHECK_ERROR_RETURN(Result);

  // Если получили сообщение об ошибке
//   if (responseList.size() == REQ_INTEG_ERROR_RESP_COUNT) {
// 
//     respType = (eInterReqType)StringToInt(responseList[N++]);
//     uniKioskStatus = (eUniKioskStatus)StringToInt(responseList[N++]);
// 
//     Result = ERROR_INVALID_RESPONSE_PARAMS_COUNT;
// 
//     Inform(MSML 
//       << "Ошибка выполнения запроса на сервере"
//       << "\nОжидалось = " << REQ_INTEG_GET_BILL_RESP_COUNT
//       << "\nПолучено = " << responseList.size()
//       ,ClassID, mtError, Result);
// 
//     return Result;
//   }

  // проверяем количество параметров в ответе
  if (responseList.size() != REQ_INTEG_GET_BILL_RESP_COUNT) {

    Result = ERROR_INVALID_RESPONSE_PARAMS_COUNT;

    Inform(MSML 
      << "Неожиданное количество параметров в ответе"
      << "\nОжидалось = " << REQ_INTEG_GET_BILL_RESP_COUNT
      << "\nПолучено = " << responseList.size()
      ,ClassID, mtError, Result);

    return Result;
  }

// 1.Тип запроса N 212
// 2.AnswerStatus N Статус ответа
// 3 AnswerMessage A Если никакого сообщения (ошибки) от провайдера нет, то заполнять *.  
// 4 BillType N тип счета
// 5 AmountToPay N12 Сумма к оплате в копейках (выставленная),  если статус =0 или 2
// 6 PrevCounter N Предыд. показания счетчика ( или *)
// 7 KioskMessage A80 Сообщение для отображения на терминале (кассе) (cp-1251)
// 8 DopInfo А Доп.информация. Поле заполняется если это требует конкр.  сервис. В общем случае поле пустое ( заполняется * ) 

  N = 0;

  respType = (eInterReqType)StringToInt(responseList[N++]);
  uniKioskStatus = (eUniKioskStatus)StringToInt(responseList[N++]);
  outProviderMessage = responseList[N++];
  outBillType = (eBillType)StringToInt(responseList[N++]);
  outAmountToPay = AmountCentToAmount(responseList[N++]);
  outPrevCounter = responseList[N++];
  outKioskMessage = responseList[N++];
  outDopInfo = responseList[N++];

  if (respType != irGetBillResp) {
    Result = ERROR_UNEXPECTED_INTER_RESP_TYPE;

    Inform(MSML
      << "Неожиданный идентификатор ответа"
      << "\nAccepted = " << respType
      << "\nEstimated = " << irGetBillResp
      ,ClassID, mtError, Result);

    return Result;
  }

#endif // TEST_SERVER

  Inform(MS
    << "\n....................."
    << "\nЗапрос счета - ответ"
    << "\n....................."
    << "\nuniKioskStatus = " << uniKioskStatus
    << "\noutProviderMessage = " << outProviderMessage
    << "\noutBillType = " << outBillType
    << "\noutAmountToPay = " << outAmountToPay
    << "\noutPrevCounter = " << outPrevCounter
    << "\noutKioskMessage = " << outKioskMessage
    << "\noutDopInfo = " << outDopInfo
    ,ClassID, mtMessage);

  return Result;
}

//----------------------------------------------------------------------
// Оплатить счет
// Вход
//		
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::PayBillProvider (
  eRepeatReq          inRepeatReq,
  std::string					inIdService,
  std::string					inIdSubService,
  std::string					inIdClient,
  std::string					inIdComBill,
  std::string					inAmountPayed,
  eBillType           inBillType,
  std::string					inPrevCounter,
  std::string					inCurrCounter,
  std::string					inUsedCounter,
  TPosDateTime			  inPeriodBegin,
  TPosDateTime			  inPeriodEnd,
  std::string					inDopInfo,
  std::string					inIdTerminal,
  std::string					inIdBuyer,

  eUniKioskStatus			&uniKioskStatus,
  std::string					&outProviderMessage,
  TPosDateTime        &outPayDateTime,
  std::string					&outProviderExtNo,
  std::string					&outIdCompany
                                      )
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::GetBillProvider : ";
  int											    Result = OK;

  TWebRequestList             requestList;
  TWebRequestList	            responseList;
  std::string							    stValue;
  eInterReqType               respType;

  Inform(MS
    << "\n....................."
    << "\nОплата счета - запрос"
    << "\n....................."
    << "\ninRepeatReq = " << inRepeatReq
    << "\ninIdService = " << inIdService
    << "\ninIdSubService = " << inIdSubService
    << "\ninIdClient = " << inIdClient
    << "\ninIdComBill = " << inIdComBill
    << "\ninAmountPayed = " << inAmountPayed
    << "\ninBillType = " << inBillType
    << "\ninPrevCounter = " << inPrevCounter
    << "\ninCurrCounter = " << inCurrCounter
    << "\ninUsedCounter = " << inUsedCounter
    << "\ninPeriodBegin = " << TPosDateTime_To_DateTimeString(inPeriodBegin)
    << "\ninPeriodEnd = " << TPosDateTime_To_DateTimeString(inPeriodEnd)
    << "\ninDopInfo = " << inDopInfo
    << "\ninIdTerminal = " << inIdTerminal
    << "\ninIdBuyer = " << inIdBuyer
    ,ClassID, mtMessage);

#ifdef EMULATE_INTEG_INTERNAL_REQUEST

  uniKioskStatus = uksBillPayed;
  outProviderMessage = "*";
  outPayDateTime = GetCurrentDateTime();
  outProviderExtNo = GetRandomNumber(7);
  outIdCompany = "*";

#else // EMULATE_INTEG_INTERNAL_REQUEST

// 1. Тип запроса N 213
// 2. repeat N 
// 3. IdService N Номер провайдера услуг
// 4. IdSubService N услуга
// 5. IdClient А20 Идентификатор клиента Номер счета ,  договора, № телефона и т.д.
// 6. Idcombill  A Idcombill операции.  ПС может его использовать в своих вспомогательных таблицах. 
// 7. AmountPayed N12 Сумма оплаты для перечисления Поставщику (в копейках)
// 8 BillType  N тип счета
// 9. PrevCounter N Предыд. показание счетчика
// 10. CurrCounter Текущее показание счетчика
// 11. UsedCounter Использовано (потребление)
// 12. PeriodBegin yyyy.mm.dd Период оплаты (начало)  Если параметры счета не требуют заполнения полей или клиент не указал, то заполняются символом * 
// 13. PeriodEnd yyyy.mm.dd Период оплаты (окончание) Если параметры счета не требуют заполнения полей или клиент не указал, то заполняются символом * 
// 14. KioskMessage А80 Сообщение Которое пришло со счетом (в сообщении 212) 
// 15. DopInfo А Доп.информация для сервиса. Если поле пустое то заполняется *
// 17. IdTerminal NN (N<=12) Идентификатор устройства
// 18. idbuyer NN Идентификатор байера

  int N = 0;

  requestList[N++] = IntToString(irPayBillReq);
  requestList[N++] = inIdService;
  requestList[N++] = inIdSubService;
  requestList[N++] = inIdClient;
  requestList[N++] = IntToString(inRepeatReq);
  requestList[N++] = inIdComBill;
  requestList[N++] = AmountToAmountCent(inAmountPayed);
  requestList[N++] = IntToString(inBillType);
  requestList[N++] = CreateDefaultValue(inPrevCounter);
  requestList[N++] = CreateDefaultValue(inCurrCounter);
  requestList[N++] = CreateDefaultValue(inUsedCounter);
  requestList[N++] = TPosDateTime_To_DateYYYY_MM_DD_dot(inPeriodBegin);
  requestList[N++] = TPosDateTime_To_DateYYYY_MM_DD_dot(inPeriodEnd);
  requestList[N++] = CreateDefaultValue(inDopInfo);
  requestList[N++] = CreateDefaultValue(inIdTerminal);
  requestList[N++] = inIdBuyer;

  Result = execInternalServRequest (
    inIdService,
    inIdSubService,
    requestList,

    responseList
    );

  CHECK_ERROR_RETURN(Result);

  // проверяем количество параметров в ответе
  if (responseList.size() != REQ_INTEG_PAY_BILL_RESP_COUNT) {

    Result = ERROR_INVALID_RESPONSE_PARAMS_COUNT;

    Inform(MSML 
      << "Неожиданное количество параметров в ответе"
      << "\nОжидалось = " << REQ_INTEG_PAY_BILL_RESP_COUNT
      << "\nПолучено = " << responseList.size()
      ,ClassID, mtError, Result);

    return Result;
  }

// 1.Тип запроса N 214
// 2. UniKioskStatus  N Статус ответа
// 3 ProviderMessage A Если никакого сообщения (ошибки) от провайдера нет, то заполнять *.  
// 4 PayDateTime yyyy.mm.dd hh:mm:ss Дата время оплаты переданное провайдером (если передается), иначе время оплаты по часам ПС
// 5. ProviderExtNo N Номер транзакции оплаты в системе провайдера (например, номер по Киевстару). Если нет заполняется «*»
// 6. IdCompany N Номер получателя для расширенного онлайна. Если нет - заполняется «*»

  N = 0;

  respType = (eInterReqType)StringToInt(responseList[N++]);
  uniKioskStatus = (eUniKioskStatus)StringToInt(responseList[N++]);
  outProviderMessage = responseList[N++];
  stValue = responseList[N++];
  Result = DateTimeString_To_TPosDateTime(stValue, outPayDateTime);
  CHECK_ERROR_RETURN(Result);

  outProviderExtNo = responseList[N++];
  outIdCompany = responseList[N++];

  if (respType != irPayBillResp) {
    Result = ERROR_UNEXPECTED_INTER_RESP_TYPE;

    Inform(MSML
      << "Неожиданный идентификатор ответа"
      << "\nAccepted = " << respType
      << "\nEstimated = " << irPayBillResp
      ,ClassID, mtError, Result);

    return Result;
  }

#endif // TEST_SERVER

  Inform(MS
    << "\n....................."
    << "\nОплата счета - ответ"
    << "\n....................."
    << "\nuniKioskStatus = " << uniKioskStatus
    << "\noutProviderMessage = " << outProviderMessage
    << "\noutPayDateTime = " << TPosDateTime_To_DateTimeString(outPayDateTime)
    << "\noutProviderExtNo = " << outProviderExtNo
    << "\noutIdCompany = " << outIdCompany
    ,ClassID, mtMessage);

  return Result;
}
/*
//----------------------------------------------------------------------
// Сделать сторно по счету
// Вход
//		
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::StornoBillProvider (
  std::string					inIdService,
  std::string					inIdSubService,
  std::string					inIdClient,
  std::string					inIdComBill,
  std::string					inAmountPayed,
  std::string					inIdTerminal,
  std::string					inIdBuyer,

  eUniKioskStatus			&uniKioskStatus,
  std::string					&outProviderMessage
                                      )
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::StornoBillProvider : ";
  int											    Result = OK;

  TWebRequestList             requestList;
  TWebRequestList	            responseList;
  std::string							    stValue;
  eInterReqType               respType;

  Inform(MS
    << "\n....................."
    << "\nСторно счета - запрос"
    << "\n....................."
    << "\ninIdService = " << inIdService
    << "\ninIdSubService = " << inIdSubService
    << "\ninIdClient = " << inIdClient
    << "\ninIdComBill = " << inIdComBill
    << "\ninAmountPayed = " << inAmountPayed
    << "\ninIdTerminal = " << inIdTerminal
    << "\ninIdBuyer = " << inIdBuyer
    ,ClassID, mtMessage);

#ifdef EMULATE_INTEG_INTERNAL_REQUEST

  uniKioskStatus = uksStornoSuccess;
  outProviderMessag = "*";

#else // EMULATE_INTEG_INTERNAL_REQUEST

// 1. Тип запроса N 215
// 2. IdService N Номер провайдера услуг
// 3. IdSubService N услуга
// 4. IdClient А20 Идентификатор клиента Номер счета ,  договора, № телефона и т.д.
// 5. Idcombill A Idcombill операции, которую отменяют. 
// 6. AmountPayed N12 Сумма операции которую отменяют  (в копейках)
// 7. IdTerminal NN (N<=12) Идентификатор устройства
// 8. idbuyer NN Идентификатор байера

  int N = 0;

  requestList[N++] = IntToString(irStornoBillReq);
  requestList[N++] = inIdService;
  requestList[N++] = inIdSubService;
  requestList[N++] = inIdClient;
  requestList[N++] = inIdComBill;
  requestList[N++] = inAmountPayed;
  requestList[N++] = inIdTerminal;
  requestList[N++] = inIdBuyer;

  Result = execInternalServRequest (
    inIdService,
    inIdSubService,
    requestList,

    responseList
    );

  CHECK_ERROR_RETURN(Result);

  // проверяем количество параметров в ответе
  if (responseList.size() != REQ_INTEG_STORNO_BILL_RESP_COUNT) {

    Result = ERROR_INVALID_RESPONSE_PARAMS_COUNT;

    Inform(MSML 
      << "Неожиданное количество параметров в ответе"
      << "\nОжидалось = " << REQ_INTEG_STORNO_BILL_RESP_COUNT
      << "\nПолучено = " << responseList.size()
      ,ClassID, mtError, Result);

    return Result;
  }

// 1. Тип запроса N 216
// 2. AnswerStatus N Статус ответа
//   0 – Ок, отмена выполнена успешно
//   1 – Ок, повторный ответ — отмена выполнена успешно.
//   2 — провайдер не поддерживает отмены в автоматическом режиме
//   3 — ошибка , сообщение пришло не по адресу
//   иначе код ошибки от провайдера и заполняется только поле AnswerMessage, остальные поля не передаются
// 3 AnswerMessage A Если никакого сообщения (ошибки) от провайдера нет, то заполнять *.  

  N = 0;

  respType = (eInterReqType)StringToInt(responseList[N++]);
  uniKioskStatus = (eUniKioskStatus)StringToInt(responseList[N++]);
  outProviderMessage = responseList[N++];
  stValue = responseList[N++];
  Result = DateTimeString_To_TPosDateTime(stValue, outPayDateTime);
  CHECK_ERROR_RETURN(Result);

  outProviderExtNo = responseList[N++];
  outIdCompany = responseList[N++];

  if (StringToInt(respType) != irStornoBillResp) {
    Result = ERROR_UNEXPECTED_INTER_RESP_TYPE;

    Inform(MSML
      << "Неожиданный идентификатор ответа"
      << "\nAccepted = " << respType
      << "\nEstimated = " << irStornoBillResp
      ,ClassID, mtError, Result);

    return Result;
  }

#endif // TEST_SERVER

  Inform(MS
    << "\n....................."
    << "\nСторно счета - ответ"
    << "\n....................."
    << "\nuniKioskStatus = " << uniKioskStatus
    << "\noutProviderMessage = " << outProviderMessage
    ,ClassID, mtMessage);

  return Result;
}
*/
//----------------------------------------------------------------------
// Обработать полученный запрос счета
// Вход
//		
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::AcceptGetBill (void)
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::AcceptGetBill : ";
  int											    Result = OK;

  std::string					        providerMessage;

  // Ставим статус по умолчанию ошибка сервера
  GetBillResp.BODY.uniKioskStatus = uksServerError;

  GetBillResp.BODY.idService = GetBillReq.BODY.idService;
  GetBillResp.BODY.idSubService = GetBillReq.BODY.idSubService;
  GetBillResp.BODY.idClient = GetBillReq.BODY.idClient;

  // Проверяем параметры платежа в бд
  Result = DATABASE->IntegProcess111Stage1 (	
    GetBillReq.BODY.idService,
    GetBillReq.BODY.idSubService,
    GetBillReq.BODY.idClient,
    GetBillReq.END.idTerminal,
    GetBillReq.END.keyValue,

    GetBillResp.BODY.uniKioskStatus,
    IdBuyer
//     GetBillResp.BODY.chequeText
  );

  // Если ошибка выходим со статусом общей ошибки
  if (Result != OK) {
    GetBillResp.BODY.uniKioskStatus = uksServerError;

    return Result;
  }

  // Если статус универсального киоска не успешен сразу отвечаем ошибкой
  if (!IsOkUniKioskStatus(GetBillResp.BODY.uniKioskStatus)) {
    return Result;
  }

  // Ставим ошибку по умолчанию
  GetBillResp.BODY.uniKioskStatus = uksRepeatWithTheSameCertificate;

  // Обращаемся к серверу и вычитываем данные счета
  Result = GetBillProvider (
    GetBillReq.BODY.idService,
    GetBillReq.BODY.idSubService,
    GetBillReq.BODY.idClient,
    GetBillReq.END.idTerminal,
    IdBuyer,

    GetBillResp.BODY.uniKioskStatus,
    providerMessage,
    GetBillResp.BODY.billType,
    GetBillResp.BODY.amountToPay,
    GetBillResp.BODY.prevCounter,
    GetBillResp.BODY.msg2Client,
    GetBillResp.BODY.dopInfo
  );

  CHECK_ERROR_RETURN(Result);

  if (!IsOkUniKioskStatus(GetBillResp.BODY.uniKioskStatus)) {
    return Result;
  }

  // Делаем обработку запроса после оплаты
  Result = DATABASE->IntegProcess111Stage2 (	
    GetBillReq.BODY.idService,
    GetBillReq.BODY.idSubService,
    GetBillReq.BODY.idClient,
    GetBillReq.END.idTerminal,
    GetBillReq.END.keyValue,

    GetBillResp.BODY.uniKioskStatus,
    GetBillResp.BODY.chequeText
  );

  return Result;
}

//----------------------------------------------------------------------
// Обработать полученный запрос оплаты счета по молотильному интерфейсу
// Вход
//		
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::AcceptPayBillMill (void)
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::AcceptPayBillMill : ";
  int											    Result = OK;

  std::string                 MillOperationId;
  TPosDateTime						    MillAcceptTime;
  eMillAcceptStatus           MillAcceptStatus;
  eProviderAcceptStatus       providerAcceptStatus = pasError;
//   std::string                 newIdComBill;
  eRepeatReq                  repeatReq = errFirstReq;
  std::string					        providerMessage;
  std::string					        providerExtNo;
  std::string					        idCompany;


	// Ставим статус по умолчанию ошибка сервера
	PayBillResp.BODY.uniKioskStatus = uksRepeatWithTheSameCertificate;

  MillAcceptTime = GetCurrentDateTime();
  MillAcceptStatus = masError;
  MillOperationId = "-1";

  PayBillResp.BODY.extReqNo = PayBillReq.BODY.extReqNo;
  PayBillResp.BODY.idService = PayBillReq.BODY.idService;
  PayBillResp.BODY.idSubService = PayBillReq.BODY.idSubService;
  PayBillResp.BODY.idClient = PayBillReq.BODY.idClient;

  // Обновляем мониторинг
  Result = SetMonitoring_IdService(PayBillReq.BODY.idService);
  CHECK_ERROR_RETURN(Result);

  // Выделяем новый номер тразакции
  Result = BASE_DATABASE->GetIdComBill(idComBill);

  CHECK_ERROR_RETURN(Result);

  // Проверяем статус операциив БД
  Result = DATABASE->IntegProcess113Stage1 (
    idComBill,
    PayBillReq.BODY.extReqNo,
    PayBillReq.BODY.idService,
    PayBillReq.BODY.idSubService,
    PayBillReq.BODY.idClient,
    PayBillReq.BODY.billType,
    PayBillReq.BODY.amountPayed,
    PayBillReq.BODY.amountCommis,
    PayBillReq.BODY.prevCounter,
    PayBillReq.BODY.currCounter,
    PayBillReq.BODY.usedCounter,
    PayBillReq.BODY.periodBegin,
    PayBillReq.BODY.periodEnd,
//     PayBillReq.BODY.kioskMessage,
    PayBillReq.BODY.dopInfo,
    PayBillReq.BODY.addInfoAgent,
    PayBillReq.BODY.msgDateTime,
    PayBillReq.END.idTerminal,
    PayBillReq.END.keyValue,

    PayBillResp.BODY.uniKioskStatus,
    uniBillStatus,
    PayBillResp.BODY.idComBill,
    IdBuyer,
    PayBillResp.BODY.chequeText,
    isMillOvertaking,
    isProviderOvertaking,
    isSuccessLocked
    );

  idComBill = PayBillResp.BODY.idComBill;

  CHECK_ERROR_RETURN(Result);

  // Если счет уже оплачен то ничего не делаем и просто выходим
  if (uniBillStatus == ubsBillPayed) {
    Result = OK;

    Inform(MS <<
      "Счет был уже оплачен ранее"
      ,ClassID, mtWarning);

    return Result;
  }

  // Проверяет статус счета
  Result = checkBillStatus ();

  CHECK_ERROR_RETURN(Result);

  // Если транзакция заблокирована в другом потоке
  if (!isSuccessLocked) {
    Result = ERROR_TRANSACTION_LOCKED_BY_OTHER_THREAD;

//     PayBillResp.BODY.UniKioskStatus = uksTransactionLockedByOtherThread;

    Inform(MSML
      << "Транзакция заблокирована другим потоком"
      << "\nIdBuyer = " << IdBuyer
      << "\nextReqNo = " << PayBillReq.BODY.extReqNo
      ,ClassID, mtError, Result);

    return Result;
  }

	// Ставим статус по умолчанию повторить с тем же ваучером
	PayBillResp.BODY.uniKioskStatus = uksRepeatWithTheSameCertificate;

	// Если ваучер не акцептирован то акцептируем ваучер
  if ( (uniBillStatus == ubsPayStarted) || (uniBillStatus == ubsDangCertificateAccepted) ) {

    Result = AcceptMoneyMill (
      PayBillResp.BODY.idComBill,
      IdBuyer,
      PayBillReq.BODY.idService,
      PayBillReq.BODY.extReqNo,
      PayBillReq.BODY.amountPayed,
      millReturnGuard,

      PayBillResp.BODY.uniKioskStatus,
      MillOperationId,
      MillAcceptTime,
      MillAcceptStatus
      );

    CHECK_ERROR_RETURN(Result);

    uniBillStatus = ubsDangCertificateAccepted;
  }

	// Ставим статус по умолчанию повторить с тем же ваучером
	PayBillResp.BODY.uniKioskStatus = uksServerBusy;

	// Если ваучер не акцептирован то акцептируем ваучер
  if (uniBillStatus == ubsDangCertificateAccepted) {

    // Отправляем запрос на сервер на оплату счета
    Result = PayBillProvider (
      repeatReq,
      PayBillReq.BODY.idService,
      PayBillReq.BODY.idSubService,
      PayBillReq.BODY.idClient,
      PayBillResp.BODY.idComBill,
      PayBillReq.BODY.amountPayed,
      PayBillReq.BODY.billType,
      PayBillReq.BODY.prevCounter,
      PayBillReq.BODY.currCounter,
      PayBillReq.BODY.usedCounter,
      PayBillReq.BODY.periodBegin,
      PayBillReq.BODY.periodEnd,
      PayBillReq.BODY.dopInfo,
      PayBillReq.END.idTerminal,
      IdBuyer,
      
      PayBillResp.BODY.uniKioskStatus,
      providerMessage,
      PayBillResp.BODY.msgDateTime,
      providerExtNo,
      idCompany
    );

    if (Result == OK) {
      providerAcceptStatus = pasOK;
    } else {
      providerAcceptStatus = pasError;
    }

    // Если оплата по провайдеру неуспешна
    if (!IsOkUniKioskStatus(PayBillResp.BODY.uniKioskStatus)) {
      providerAcceptStatus = pasError;
    }

  }

	// Если счет уже был успешно оплачен
  if (uniBillStatus == ubsBillPayed) {
    providerAcceptStatus = pasOK;
  }

  // Подтверждаем в базе данных что счет оплачен
  Result = DATABASE->IntegProcess113Stage2 (	
    PayBillResp.BODY.idComBill,
    PayBillReq.BODY.extReqNo,
    PayBillReq.BODY.idService,
    PayBillReq.BODY.idSubService,
    PayBillReq.BODY.idClient,
    PayBillReq.BODY.billType,
    PayBillReq.BODY.amountPayed,
    PayBillReq.BODY.amountCommis,
    PayBillReq.BODY.prevCounter,
    PayBillReq.BODY.currCounter,
    PayBillReq.BODY.usedCounter,
    PayBillReq.BODY.periodBegin,
    PayBillReq.BODY.periodEnd,
//     PayBillReq.BODY.kioskMessage,
    PayBillReq.BODY.dopInfo,
    PayBillReq.BODY.addInfoAgent,
    PayBillReq.BODY.msgDateTime,
    PayBillReq.END.idTerminal,
    PayBillReq.END.keyValue,
    PayBillResp.BODY.msgDateTime,
    MillOperationId,
    MillAcceptTime,
    MillAcceptStatus,
    providerAcceptStatus,
    PayBillResp.BODY.uniKioskStatus,

    PayBillResp.BODY.uniKioskStatus,
    PayBillResp.BODY.msgDateTime,
    isReturnVoucher,
    isSendToOvertaking
  );

  return Result;
}


//----------------------------------------------------------------------
// Отменить указанную операцию в бд
// Вход
// 		inIdBuyer - Номер покупателя
// 		inIdService - номер сервиса
//    inExtReqNo - номер сертификата
//		
// Выход
//    outIdComBill - уникальный номер транзакции в БД
//    outStornoStatus - результат сторнирования
//     30 – успешно возвращен
//     31 – успешно возвращен, повторный запрос
//     32 – не проведен, возврат по данному сервису невозможен
//     33 – не проведен, возврат операции за прошлые сутки невозможен
//     34 – не проведен, превышен интервал времени разрешенного возврата
//    outStornoAmount - сторнированная сумма если успешно
//    isReturnMoney - вовзвращать деньги или нет
//    outIdService - номер сервиса реальный
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::Storno (
  std::string					inIdBuyer,
  std::string					inIdService,
  std::string					inExtReqNo,

  std::string					&outIdComBill, 
  eUniKioskStatus			&outStornoStatus,
  std::string					&outStornoAmount,
  bool                &isReturnMoney
  )
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::Storno : ";
  int											    Result = OK;

  Result = ERROR_NOT_DEFINED_PROGRAM_CODE;
  /*
  std::string							    StornoIdService;
  bool                        exists;

  // Определяем номер сервиса
  Result = DATABASE->getIdServiceByMill (	
    inIdBuyer,
    inExtReqNo,

    StornoIdService
    );

  CHECK_ERROR_RETURN(Result);

    std::string					inIdClient,
    std::string					inIdComBill,
    std::string					inAmountPayed,
    std::string					inIdTerminal,
    std::string					inIdBuyer,


    std::string					inIdService,
    std::string					inIdSubService,
    std::string					inIdClient,
    std::string					inIdComBill,
    std::string					inAmountPayed,
    std::string					inIdTerminal,
    std::string					inIdBuyer,


  // Отменяем операцию у провайдера
  Result = StornoBillProvider (
    std::string					inIdService,
    std::string					inIdSubService,
    std::string					inIdClient,
    std::string					inIdComBill,
    std::string					inAmountPayed,
    std::string					inIdTerminal,
    std::string					inIdBuyer,

    eUniKioskStatus			&uniKioskStatus,
    std::string					&outProviderMessage
                                      )



  // отменяем операцию в БД
  Result = DATABASE->StornoIntegratorBill (	
    inIdBuyer,
    inCertificateNumber,

    outIdComBill, 
    outStornoStatus, 
    outStornoAmount
    );

  CHECK_ERROR_RETURN(Result);
  */
  return Result;
}

