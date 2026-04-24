#include "integrator_pch.h"

// Работа с парсингом запросов
#include "Requests/BaseWebserverRequests.h"

// Выполнения запросов по сети
#include "Http/ExecuteHttpRequest.h"

#include "IntegratorMillExtAcceptor.h"

#include "Requests/SmsRequests.h"


/// <summary>
/// Конструктор
/// </summary>
MillExtAcceptor::MillExtAcceptor (
              ) : IntegratorBase (
              )
{

}

/// <summary>
/// Деструктор
/// </summary>
MillExtAcceptor::~MillExtAcceptor ()
{

}

//-------------------------------------------------------------------------
// Обрабатывает запрос
// Вход
//	
// Выход 
//	  результат функции - результат выполнения
//-------------------------------------------------------------------------
int MillExtAcceptor::ProcessRequest (void)
{
  TID                         ClassID = tidUniKiosk;
  std::string                 ProcedureName = "IntegratorBase::ProcessRequest : ";
  int													Result = OK;

  int                         execResult;

  ReqType = String_To_eUniKioskClientRequest(RequestList[0]);

  // Обновляем мониторинг
  Result = SetMonitoring_ReqType(ReqType);
  CHECK_ERROR_RETURN(Result);

  if (ReqType == ukcUnknown) {
    Result = ERROR_UNKNOWN_TELEGRAM_TYPE;

    Inform(MSML 
      << "Неизвестный тип телеграммы"
      << "\nReqType = " << RequestList[0]
    ,ClassID, mtError, Result);

    return Result;
  }
     
  switch (ReqType) {
    // Запрос состояния счета 
    case ukcServiceAccountReq:

    // Запрос статуса транзакции (133/134)
//     case ukcServiceOperStatusReq:

    // сверка транзакций списком (135/136)
    case ukcServiceOperListReq:

    // Сверка транзакций по суммам (137/138)
    case ukcServiceSummListReq:

    // Запрос списка поставщиков (ассортимент) (141/142)
    case ukcServiceProviderListReq:

    // Запрос комиссионных списка поставщиков  (143/144)
    case ukcServiceCommissionsReq:

    // Регистрация точек приема платежей (ТПП) (145/146)
    case ukcServiceRegistryTPPReq:

    // Запрос справочника регионов (151/152)
//     case ukcServiceRegionsListReq:

    // Запрос списка поставщиков (ассортимент-список) (147/148)
    case ukcServiceLiteProviderListReq:

    //   Запрос списка услуг для сервиса(ов) (153/154)
    case ukcServiceListSubSrvReq:

    //   Запрос общего перечня списков(справочников)  (155/156)
    case ukcServiceListCatalogReq:

    //   Запрос одного списка (справочника)  (157/158)
    case ukcServiceCatalogReq:

      execResult = ProcessServiceRequest ();

      CHECK_ERROR_RETURN(execResult);

      break;

    default: {
      
      return IntegratorBase::ProcessRequest ();
             }
  }

  return Result;
}

//-------------------------------------------------------------------------
// Получить и обработать сервисный запрос
// Вход
//  нет
// Выход 
//	результат функции - результат выполнения
//-------------------------------------------------------------------------
int MillExtAcceptor::ProcessServiceRequest (void)
{
  TID                         ClassID = tidUniKiosk;
  std::string                 ProcedureName = "MillExtAcceptor::ProcessServiceRequest : ";
  int													Result = OK;

  std::string                 idService = "0";
  std::string                 idSubService = "0";
  std::string                 endTransmissionString = UNI_KIOSK_ETX;
  ConnectHostGuard            HostCon;
  std::string                 address;
  int                         port;
  std::string                 requestSt;
  std::string                 responseSt;

  // Берем адресс куда отправлять сервисные запросы
  Result = ROUTE_DATABASE->GetInternalServAddress (
    idService,
    idSubService,

    address,
    port
    );

  CHECK_ERROR_RETURN(Result);

  requestSt = RequestText;

  // Отсылаем запрос 
  Result = OpenTcpConnection(
		HostCon,
    address,
    port
		);

  if (Result != OK) {
    Result = ERROR_SENDING_REQUEST;

		Inform(MSML
			<< "Ошибка отсылки сервисного запроса - ошибка открытия сокета"
      << "\naddress = " << address
      << "\nport = " << port
      ,ClassID, mtError, Result);

		return Result;
  }

  Result = ExecTcpRequest(
    HostCon,
    endTransmissionString,
    requestSt,
    responseSt
    );

  if (Result != OK) {
		Result = ERROR_SENDING_REQUEST;

		Inform(MSML
			<< "Ошибка отсылки сервисного запроса - ошибка открытия сокета"
      << "\naddress = " << address
      << "\nport = " << port
      ,ClassID, mtError, Result);

		return Result;
  }

  // Формируем ответ
  ResponseText = responseSt;

  return Result;
}

//---------------------------------------------------------------------------
// Возвращает название оплаты для базы данных
//---------------------------------------------------------------------------
std::string MillExtAcceptor::GetBillName (void)
{
  std::string						Result;

  Result = PayBillReq.HDR.idService;
  Result += " Sum-" + PayBillReq.BODY.tagAmountPay.amountToPay;
  Result += " Client-" + PayBillReq.HDR.idClient;

  // Обрезаем сообщение до длины записи в бд
  if (Result.length() > 47) {
    Result = Result.substr(0, 47) + "...";
  }

  return Result;
}

//----------------------------------------------------------------------
// Подготовить таг S_tagCheque1
//----------------------------------------------------------------------
int MillExtAcceptor::prepareTags (
  std::string                 idService,
  std::string                 idBuyer,

  S_tagCheque1                &tag,
  TStdStringList	            &billTags
  )
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::prepareTags : ";
  int											    Result = OK;

  // чек 1
// struct S_tagCheque1 {
//   std::string					chequeText;         // A360  Реквизиты получателя: Часть чека, описывающая банк плательщика, реквизиты получателя. Символ «|» обозначает перевод строки на чеке.
// };
  /*
  Result = DATABASE->GetServiceCheque (	
    idService,
    idBuyer,

    tag.chequeText
    );

  CHECK_ERROR_RETURN(Result);
  */

  Result = MillExtAcceptor::prepareTags (
    idService,
    "0",
    idBuyer,

    tag,
    billTags
    );

  CHECK_ERROR_RETURN(Result);

  return Result;
}

//----------------------------------------------------------------------
// Подготовить таг S_tagCheque1 для сервисов с IdCompany
//----------------------------------------------------------------------
int MillExtAcceptor::prepareTags (
  std::string                 idService,
  std::string                 idCompany,
  std::string                 idBuyer,

  S_tagCheque1                &tag,
  TStdStringList	            &billTags
  )
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::prepareTags : ";
  int											    Result = OK;

  std::string							    sender;
  std::string							    MFOSender;
  std::string							    receiver;
  std::string							    ZKPO;
  std::string							    bankReciever;
  std::string							    MFO;
  std::string							    account;
  std::string							    paymentText;
  std::string							    buyerType;

  /*
  // чек 1
// struct S_tagCheque1 {
//   std::string					chequeText;         // A360  Реквизиты получателя: Часть чека, описывающая банк плательщика, реквизиты получателя. Символ «|» обозначает перевод строки на чеке.
// };

  Result = DATABASE->GetServiceChequeIdCompany (	
    idService,
    idCompany,
    idBuyer,

    tag.chequeText
    );

  CHECK_ERROR_RETURN(Result);

  AddTag(billTags, TAG_CHECK1);
  */

  Result = DATABASE->GetServiceChequeExt (	
    idService,
    "0",
    idBuyer,
    "",
    idCompany,

    sender, 
    MFOSender,
    receiver,
    ZKPO,
    bankReciever,
    MFO,
    account,
    paymentText,
    buyerType
  );

  CHECK_ERROR_RETURN(Result);

  tag.chequeText = "";

  if (buyerType == "0") {
    tag.chequeText += "Банк платника: " + sender;
    tag.chequeText += "|Код банку Платника: " + MFOSender;
  }

  if (tag.chequeText != "") {
    tag.chequeText += "|";
  }
  tag.chequeText += "Отримувач: " + receiver;
  tag.chequeText += "|Код ЄДР: " + ZKPO;
  tag.chequeText += "|Рахунок: " + account;
  tag.chequeText += "|Банк Отримувача: " + bankReciever;
  /// ----------------------
  if ( (MFO != "0") && (MFO.length() > 5) )
  {
    tag.chequeText += "|Код банку Отримувача: " + MFO;
  }
  /// ----------------------
  tag.chequeText += "|Призначення платежу: " + paymentText;

  AddTag(billTags, TAG_CHECK1);

  return Result;
}

//----------------------------------------------------------------------
// Подготовить таг S_tagCheque2
//----------------------------------------------------------------------
int MillExtAcceptor::prepareTags (
  std::string                 idService,
  std::string                 idSubService,
  std::string                 idBuyer,
  std::string                 extReqNo,
  std::string                 idCompany,

  S_tagCheque2                &tag,
  TStdStringList	            &billTags
  )
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::prepareTags : ";
  int											    Result = OK;

  std::string				          chequeText;
  std::string				          sender;
  std::string				          MFOSender;
  std::string				          buyerType;

// чек 2
// struct S_tagCheque2 {
//   std::string					receiver;       // A Получатель 
//   std::string					ZKPO;           // A Код получателя
//   std::string					bankReciever;   // A Банк получателя
//   std::string					MFO;            // A МФО банка
//   std::string					account;        // A Счет получателя
//   std::string					paymentText;    // A Назначение платежа
// };

  Result = DATABASE->GetServiceChequeExt (	
    idService,
    idSubService,
    idBuyer,
    extReqNo,
    idCompany,

//     tag.paymentText,
    sender, 
    MFOSender,
    tag.receiver,
    tag.ZKPO,
    tag.bankReciever,
    tag.MFO,
    tag.account,
    tag.paymentText,
    buyerType
  );

  CHECK_ERROR_RETURN(Result);

  AddTag(billTags, TAG_CHECK2);

  return Result;
}

//----------------------------------------------------------------------
// Подготовить таг S_tagCheque3
//----------------------------------------------------------------------
int MillExtAcceptor::prepareTags (
  std::string                 idService,
  std::string                 idSubService,
  std::string                 idBuyer,
  std::string                 extReqNo,
  std::string                 idCompany,

  S_tagCheque3                &tag,
  TStdStringList	            &billTags
  )
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::prepareTags : ";
  int											    Result = OK;

  std::string				          chequeText;
  std::string				          buyerType;
//   std::string				          sender;
//   std::string				          MFOSender

// чек 3
// struct S_tagCheque3 {
//   std::string					sender;         // A Банк-плательщик 
//   std::string					MFOsender;      // МФО банка -плательщика
//   std::string					receiver;       // A Получатель 
//   std::string					ZKPO;           // A Код получателя
//   std::string					bankReciever;   // A Банк получателя
//   std::string					MFO;            // A МФО банка
//   std::string					account;        // A Счет получателя
//   std::string					paymentText;    // A Назначение платежа
// };

  Result = DATABASE->GetServiceChequeExt (	
    idService,
    idSubService,
    idBuyer,
    extReqNo,
    idCompany,

//     tag.paymentText,
    tag.sender,
    tag.MFOsender,
    tag.receiver,
    tag.ZKPO,
    tag.bankReciever,
    tag.MFO,
    tag.account,
    tag.paymentText,
    buyerType
  );

  CHECK_ERROR_RETURN(Result);

  AddTag(billTags, TAG_CHECK3);

  return Result;
}

//----------------------------------------------------------------------
// Вычитать и добавить необходимые таги в запрос
// Вход
//    billTagsToAdd - список тагов для добавления
//    HDR - заголовок запроса
//    tagBodyReq - данные тагов запроса
//    provHDR - заголовок ответа от предметного сервера
//    tagProvBodyResp - данные тагов ответа от провайдера
// Выход
//    tagBody - данные тагов
//    billTags - список тагов
//    uniKioskStatus - статус универсального киоска
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::addIntegratorRequestTags (
  TStdStringList	            billTagsToAdd,
  S_INTEG_HEADER_REQ	        HDR,
  S_TAG_BODY                  tagBodyReq,
  S_PROVIDER_HEADER_RESP      provHDR,
  S_TAG_BODY                  tagProvBodyResp,

  S_TAG_BODY                  &tagBody,
  TStdStringList	            &billTags,
  eUniKioskStatus             &uniKioskStatus
  )
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::addIntegratorRequestTags : ";
  int											    Result = OK;

  std::string                 tagName;
  std::string                 idCompany;
  TStdStringListConstIter it;

  it = billTagsToAdd.begin();

  while (it != billTagsToAdd.end()) {

    tagName = it->second;
    
    // Если пустой таг ничего не делаем
    if (tagName == "") 
    {
    } 
    else if (tagName == TAG_CHECK1) 
    {
      // Если чек формирует предметный
      if (conf.chequeMode) 
      {
        ++it;
        continue;
      }

      if (ExistsTag(provHDR.billTags, TAG_ID_COMP) && conf.useIdCompanyCheque) {

        if (tagProvBodyResp.tagIdComp.size() == 0) {

          uniKioskStatus = uksChequeTextAbsent;

          Result = ERROR_NOT_EXISTS_TAG_DATA_ID_COMPANY;

          Inform(MSML
            << "Отсутствуют данные тага ID_COMPANY"
            ,ClassID, mtError, Result);

          return Result;
        }

        Result = prepareTags (
          HDR.idService,
          tagProvBodyResp.tagIdComp.begin()->second.idCompany,
          IdBuyer,

          tagBody.tagCheque1,
          billTags
          );
      
      // Если нету тага но стоит признак отдавать чек по таблице uniservice2company
      } else if (!ExistsTag(provHDR.billTags, TAG_ID_COMP) && conf.useIdCompanyCheque) {

          uniKioskStatus = uksChequeTextAbsent;

          Result = ERROR_NOT_EXISTS_TAG_DATA_ID_COMPANY;

          Inform(MSML
            /// << "Выставлен признак отдавать чек по таблице uniservice2company но в ответе отсутствует таг ID_COMPANY"
            << "Выставлен признак отдавать чек, но в ответе отсутствует таг ID_COMPANY. "
            ,ClassID, mtError, Result);

          return Result;
      
      } else {

        Result = prepareTags (
          HDR.idService,
          IdBuyer,

          tagBody.tagCheque1,
          billTags
          );

      }

        if (Result != OK) {
          uniKioskStatus = uksChequeTextAbsent;

          return Result;

      }
    } else if (tagName == TAG_CHECK2) {

      // Если чек формирует предметный
      if (conf.chequeMode) {
        ++it;
        continue;
      }

      if (tagProvBodyResp.tagIdComp.size() > 0 && conf.useIdCompanyCheque) {
        idCompany = tagProvBodyResp.tagIdComp.begin()->second.idCompany;
      } else {
        idCompany = "0";
      }

//       if (!conf.chequeMode) {
        Result = prepareTags (
          HDR.idService,
          HDR.idSubService,
          IdBuyer,
          tagBody.tagAgentCode.extReqNo,
          idCompany,

          tagBody.tagCheque2,
          billTags
          );

        if (Result != OK) {
          uniKioskStatus = uksChequeTextAbsent;

          return Result;
//         }
      }

    } else if (tagName == TAG_CHECK3) {

      // Если чек формирует предметный
      if (conf.chequeMode) {
        ++it;
        continue;
      }

//       if (tagProvBodyResp.tagIdComp.size() == 0) {
//         idCompany = "0";
//       } else {
//         idCompany = tagProvBodyResp.tagIdComp.begin()->second.idCompany;
//       }

      if (tagProvBodyResp.tagIdComp.size() > 0 && conf.useIdCompanyCheque) {
        idCompany = tagProvBodyResp.tagIdComp.begin()->second.idCompany;
      } else {
        idCompany = "0";
      }

//       if (!conf.chequeMode) {
        Result = prepareTags (
          HDR.idService,
          HDR.idSubService,
          IdBuyer,
          tagBody.tagAgentCode.extReqNo,
          idCompany,

          tagBody.tagCheque3,
          billTags
          );

        if (Result != OK) {
          uniKioskStatus = uksChequeTextAbsent;

          return Result;
        }
//       }
    } else if (tagName == TAG_AGENT_CODE) {

      AddTag(billTags, TAG_AGENT_CODE);
      tagBody.tagAgentCode = tagBodyReq.tagAgentCode;

    } else if (tagName == TAG_TRANSACTION) {

      AddTag(billTags, TAG_TRANSACTION);
      tagBody.tagTransaction.idComBill = idComBill;

    } 
    else if (tagName == TAG_DATE_OP) {

      AddTag(billTags, TAG_DATE_OP);
      tagBody.tagDateOp.DateOperation = TPosDateTime_To_DateTimeString( GetCurrentDateTime() );

    } 
    else {
      Result = ERROR_UNKNOWN_INTEG_TAG;

      Inform(MSML
        << "Неизвестный таг tagName = " << tagName
        ,ClassID, mtError, Result);

      return Result;
    }

    ++it;
  };

  return Result;
}

//----------------------------------------------------------------------
// Сохранить присланные в запросе таги в бд
// Вход
//    idComBill - номер транзакции по бд 
//    HDR - заголовок запроса
//    tag - данные тага
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::storeTags (
  std::string                 idComBill,
  S_INTEG_HEADER_REQ	        HDR,
  TagCounter                  tag
  )
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::storeTags : ";
  int											    Result = OK;

    // Данные счетчика
// struct S_tagCounterData {
//   std::string					prevCounter;      // N Предыд. показания счетчика. В запросе 112 может быть не заполнено (=0). Это означает, что счетчик есть, но его предыд. значение неизвестно. Клиент может внести другое значение, которое должно быть передано в запросе 113.
//   std::string					currCounter;      // N Текущее значение счетчика. В запросе 112 заполняется 0, при оплате (запрос 113) заполняется значением, которое указал клиент или 0 (если клиент ничег не указал)
//   std::string					usedCounter;      // N Использовано.
//   std::string					nameCounter;      // A Наименование счетчика (тип, номер, место установки и т. д.). Это значение передает провайдер и не может быть изменено. Если значение не передано, то заполняется  «*»
// };
  int DropToDbErr = OK;
  TagCounterConstIter it;

  it = tag.begin();

  while (it != tag.end()) {

    Result = DATABASE->StoreCounters (
      idComBill,
      it->second.idCounter,
      HDR.idService,
      HDR.idClient,
      it->second.nameCounter,
      it->second.prevCounter,
      it->second.currCounter,
      it->second.usedCounter
      );

    if (Result != OK) {
      DropToDbErr = Result;
      Inform(MSML << "Ошибка сохранения счетчиков в бд", ClassID, mtError, Result);
    }

    ++it;
  }

  if ( DropToDbErr != OK )
    Result = DropToDbErr;

  return Result;
}

//----------------------------------------------------------------------
// Сохранить присланные в запросе таги в бд
// Вход
//    idComBill - номер транзакции по бд 
//    HDR - заголовок запроса
//    tag - данные тага
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::storeTags (
  std::string                 idComBill,
  S_INTEG_HEADER_REQ	        HDR,
  S_tagPeriod                 tag
  )
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::storeTags : ";
  int											    Result = OK;

// Таг периода оплаты
// struct S_tagPeriod {
//   std::string					periodID;         // N Идентификатор периода (№ п/п). Уникальный в пределах тага и сообщения.
//   std::string					periodMode;       // N Режим тага 
//   TPosDateTime			  periodBegin;      // A Дата в формате dd.mm.yyyy  - начало периода , за который выполняется оплата
//   TPosDateTime        periodEnd;        // A Дата в формате dd.mm.yyyy  - окончание периода , за который выполняется оплата
// };

  Result = DATABASE->StorePeriod (
    idComBill,
    "-1",
    tag.periodMode,
    tag.periodBegin,
    tag.periodEnd
    );

  if (Result != OK) {
    Inform(MSML
      << "Ошибка сохранения тага в бд"
      ,ClassID, mtError, Result);
  }

  return Result;
}

//----------------------------------------------------------------------
// Сохранить присланные в запросе таги в бд
// Вход
//    idComBill - номер транзакции по бд 
//    HDR - заголовок запроса
//    tagList - данные тага
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::storeTags (
  std::string                 idComBill,
  S_INTEG_HEADER_REQ	        HDR,
  TagInfoFromClient           tagList
  )
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::storeTags : ";
  int											    Result = OK;

// Информация от клиента
// struct S_tagInfoFromClient {
//   std::string					mode;             // N
//   std::string					InfoName;         // A Название информационного поля (что спрашивать у клиента)
//   std::string					InfoValue;        // A Данные, введенные клиентом. ( В запросе 112
// };

  Result = DATABASE->StoreInfoFromClient (
    idComBill,
    tagList
    );

  if (Result != OK) {
    Inform(MSML
      << "Ошибка сохранения тага в бд"
      ,ClassID, mtError, Result);
  }

  return Result;
}

//----------------------------------------------------------------------
// Сохранить присланные в запросе таги в бд
// Вход
//    std::string                 idComBill,
//    HDR - заголовок запроса
//    tagBody - данные тагов
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::storeIntegratorRequestTags (
  std::string                 idComBill,
  S_INTEG_HEADER_REQ	        HDR,
  S_TAG_BODY                  tagBody
  )
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::storeIntegratorRequestTags : ";
  int											    Result = OK;

  std::string       tagName;
  TStdStringListConstIter it;

  it = HDR.billTags.begin();

  while (it != HDR.billTags.end()) {

    tagName = it->second;
    
    if (tagName == TAG_COUNTER) {
      Result = storeTags (
        idComBill,
        HDR,
        tagBody.tagCounter
        );

      if ( Result != OK )
      {
        Result = ERROR_FIELD_OUT_OF_RESULT_SET_RANGE; /// 818006
        return Result;
      }

      /// CHECK_ERROR_RETURN(Result);
    
    } else if (tagName == TAG_PERIOD) {
      Result = storeTags (
        idComBill,
        HDR,
        tagBody.tagPeriod
        );

      CHECK_ERROR_RETURN(Result);
    /*
    } else if (tagName == TAG_CHECK3) {
      Result = prepareTags (
        HDR.idService,
        IdBuyer,

        tagBody.tagCheque3,
        billTags
        );

      CHECK_ERROR_RETURN(Result);
    */
    } else {
//       Inform(MS
//         << "Не сохраняем tagName = " << tagName
//         ,ClassID, mtWarning);

//       return Result;
    }

    ++it;
  };

  return Result;
}

//----------------------------------------------------------------------
// Сохранить присланные в запросе таги в бд
// Вход
//    idComBill - ид транзакции
//    conf - конфигурация для сервиса
//    HDR - заголовок запроса
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::storeTagsList (
  std::string                 idComBill,
  S_INTER_CONFIG		          conf,
  S_INTEG_HEADER_REQ	        HDR
  )
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::storeTagsList : ";
  int											    Result = OK;

  bool							          isTagCounter = false;
  bool							          isTagPeriod = false;
  bool							          isTagInfoFromClient = false;
  bool							          isTagPaySubSrvFix = false;
  bool							          isTagPaySubSrvCount = false;
  bool							          isTagPayDept = false;

  // Если не прописано в конфигурации не сохраняем список тагов
  if (SERVER_CONF->StoreITransactionsComm != "1" || !conf.saveTrans) {
    Result = OK;

    return Result;
  }

//   `IdComBill` bigint(20) NOT NULL DEFAULT '0',
//   `IdService` int(11) NOT NULL,
//   `tagCounter` tinyint(4) DEFAULT '0' COMMENT '0 -счетчиков нет, 1 -счетчики есть',
//   `tagPeriod` tinyint(4) DEFAULT '0' COMMENT '0 - периодов нет, 1 - период есть',
//   `tagInfoFromClient` tinyint(4) DEFAULT '0' COMMENT '0 - тага инф.от клиента нет, 1 - есть',
//   `tagPaySubSrvFix` tinyint(4) DEFAULT '0' COMMENT '0 - нет тага PF, 1 есть',
//   `tagPaySubSrvCount` tinyint(4) DEFAULT '0' COMMENT '0 - нет тага PC, 1 - есть',
//   `tagPayDept` tinyint(4) DEFAULT '0' COMMENT '0 - нет тага PD, 1 - есть',
//   `ChequeOwner` tinyint(4) DEFAULT '0' COMMENT '0 - чек формирует ТрэйдСерв., 1 - чек получаем от Поставщика',

  std::string       tagName;
  TStdStringListConstIter it;

  it = HDR.billTags.begin();

  while (it != HDR.billTags.end()) {

    tagName = it->second;
    
    if (tagName == TAG_COUNTER) {
      isTagCounter = true;
    
    } else if (tagName == TAG_PERIOD) {
      isTagPeriod = true;

    } else if (tagName == TAG_INFO_FROM_CLIENT) {
      isTagInfoFromClient = true;

    } else if (tagName == TAG_PAY_SUB_SRV_FIX) {
      isTagPaySubSrvFix = true;
  
    } else if (tagName == TAG_PAYSUB_SRV_COUNT) {
      isTagPaySubSrvCount = true;
  
    } else if (tagName == TAG_PAY_DEPT) {
      isTagPayDept = true;
  
    } else {
      Inform(MS
        << "Пропускаем таг в iTransactionsComm tagName = " << tagName
        ,ClassID, mtWarning);

//       return Result;
    }

    ++it;
  };

  Result = DATABASE->StorePayTagList (
    idComBill,
    HDR.idService,
    conf.chequeMode,
    isTagCounter,
    isTagPeriod,
    isTagInfoFromClient,
    isTagPaySubSrvFix,
    isTagPaySubSrvCount,
    isTagPayDept
    );

  CHECK_ERROR_RETURN(Result);

  return Result;
}

//----------------------------------------------------------------------
// Выполнить запрос на внутреннем сервере
// Вход
//    inIdService - номер сервиса
//    inIdSubService - номер услуги
//		requestList - запрос
// Выход
//    uniKioskStatus - статус универсального киоска
//    responseList - ответ
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::execInternalServRequest (
  std::string                 inIdService,
  std::string                 inIdSubService,
  TWebRequestList             requestList,

  eUniKioskStatus             &uniKioskStatus,
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
  //----------------------
  // создаем запрос
  //----------------------
  request = CreateWebServerResponse(
    requestList, 
    TelRecordDivider,
    Etx
    );
  //----------------------
  // Берем адрес куда подключаться
  //----------------------
  Result = ROUTE_DATABASE->GetInternalServAddress (
    inIdService,
    inIdSubService,

    address,
    port
  );
  //----------------------
  if (Result != OK) 
  {
    // uniKioskStatus = uksStutzerError;
    uniKioskStatus = uksServerBusy;

    return Result;
  }

#ifdef TEST_SERVER_AG
  address = "172.27.72.131";
  port    = 12346;
  // port = 12345;
  /// address = "172.27.72.84";
  /// port    = 52300;
  /// port    = 54665;
  /// port    = 12340;
  /// port    = 55116;
  if ( inIdService == "700" )
  {
    address = "127.0.0.1";
    port    = 12345;
  }
  Result  = OK;
#endif

  Result = OpenTcpConnection(
		HostCon,
    address,
    port
	);
  //----------------------
  //# ifndef TEST_SERVER_AG
  //# else
  //  Result = OK;
  //# endif /// TEST_SERVER_AG
  //----------------------
  CHECK_ERROR_RETURN(Result);
  //----------------------
  Result = ExecTcpRequest(
    HostCon,
    Etx,
    Etx,
    request, 
    response
  );  
  //----------------------
  //# ifndef TEST_SERVER_AG
  //# else
  //  Result = OK;
  //# endif /// TEST_SERVER_AG
  //----------------------
  CHECK_ERROR_RETURN(Result);
  //----------------------
#ifdef TEST_SERVER_AG
  /// response = "212;0;*;2022.06.15 09:10:51;IDC|AM|MC;IDC;1;82;AM;8127;0;3;MC;пр-т Звiльнення Ока,б.42,кв.51_ перiод: 06.2018";
  /// response = "212;0;*;2021.08.16 16:50:03;AM|CN|PR|CH1|AW|AT|AC|AF|CA;AF;4;21108220;РАСПРЕДЕЛЕНИЕ ПРИРОДНОГО ГАЗА#109482628#М ХАРКІВ,О/Р: 109482628 (109482628);-16176;1;21108221;ВЫВОЗ ТБО#109482628#М ХАРКІВ, О/Р: 109482628 (109482628);-22115;1;21108223;ЭЛ.ЭН. (рассрочка)#109482628#М ХАРКІВ, О/Р: 109482628 (109482628);0;1;21108224;КАНАЛИЗАЦИЯ (решение суда)#109482628#М ХАРКІВ, О/Р: 109482628 (109482628);0;1;AM;0;3;0;AT;1;0;AW;*;М ХАРКІВ,ВУЛ НИЗОВА,13,2;*;CA;148742;AC;4;21108217;ЭЛЕКТРИЧЕСТВО#109482628#М ХАРКІВ, Iд.клiєнта: 109482628;0.0000;0;21108217;21108218;КАНАЛИЗАЦИЯ#109482628#М ХАРКІВ, Iд.клiєнта: 109482628;0.0000;0;21108218;21108219;ГАЗ ПРИРОДНЫЙ#109482628#М ХАРКІВ, Iд.клiєнта: 109482628;0.0000;0;21108219;21108222;ОПЛАТА ПО АКТАМ#109482628#М ХАРКІВ, Iд.клiєнта: 109482628;0.0000;0;21108222;CN;4;21108217;0;0;0;0;1;21108218;0;0;0;0;6;21108219;0;0;0;0;7;21108222;0;0;0;0;24;PR;2;2021.07.01;2021.07.31;CH1;Банк платника: ПрАТ \"Банк Фамільний\"|МФО: 334840|Отримувач: АТ \"МЕГАБАНК\"|Код: 09804119|Рахунок: UA893516290000000000029022235|Банк Отримувача: АТ \"МЕГАБАНК\"|МФО: 0|Призначення платежу: Оплата за послуги";
  /// response = "212;0;*;2018.11.28 15:54:18;CN|AC|AT|CH2C|AW|AFD|AM|CA;CN;1;7798301;0;913;0;0;ЦЕНТРАЛ_ЗОВАНЕ ПОСТАЧАННЯ ХОЛОДНОЇ ВОДИ ТА ВОДОВ_ДВЕДЕННЯ Л_ЧИЛЬНИК;AC;1;77983;ЦЕНТРАЛ_ЗОВАНЕ ПОСТАЧАННЯ ХОЛОДНОЇ ВОДИ ТА ВОДОВ_ДВЕДЕННЯ Л_ЧИЛЬНИК;18.3880;1;7798301;AT;1;1;CH2C;2;77983;ПРАТ \"АК\" КИЇВВОДОКАНАЛ\";03327629;ПАТ \"БАНК КРЕДИТ ДН_ПРО\"(м. Київ);305749;2600931011902;ЦЕНТРАЛ_ЗОВАНЕ ПОСТАЧАННЯ ХОЛОДНОЇ ВОДИ ТА ВОДОВ_ДВЕДЕННЯ Л_ЧИЛЬНИК;77984;К П Г I О Ц;04013755;ф_л_ї - ГУ по м.Києву та Київськ_й област_ АТ \"Ощадбанк\";322669;26031300763387;УТРИМАННЯ БУДИНК_В ТА ПРИБУД. ТЕРИТОР_Й;AW;ФИО НЕ УКАЗАНО;СТРАТЕГ_ЧНЕ ШОСЕ Б4/27 КВ10;*;AFD;1;77984;УТРИМАННЯ БУДИНК_В ТА ПРИБУД. ТЕРИТОР_Й;46758;3259;0;1;AM;34;3;3;CA;19670";
  /// response = "214;15;*;2018.08.01 16:00:48;*";
  /// response = "212;0;*;2018.01.04 12:10:21;AM|CN|PR|MC;AM;0;1;0;CN;1;0;0;1901;0;0;*;PR;0;*;*;MC;Херсоноблэнерго,ЩЕДРОЛОСЕВ  ЕВГЕНИЙ НИКОНО,ХЕРСОН,Червоностудентська д.7А,кв.4";
  /// response = "212;0;*;2018.01.04 12:10:21;AM|CNZ|PR|MC;AM;0;1;0;CNZ;1;1;1001;ZL-01;1.68;*;123;0;0;1;0.7;168;PR;0;*;*;MC;Херсоноблэнерго,ЩЕДРОЛОСЕВ  ЕВГЕНИЙ НИКОНО,ХЕРСОН,Червоностудентська д.7А,кв.4";
  /// Add Tag TR
  /// response = "212;0;*;2018.01.04 12:10:21;AM|CNZ|PR|MC|TR;AM;0;1;0;CNZ;1;1;1001;ZL-01;1.68;*;123;0;0;1;0.7;168;PR;0;*;*;MC;Херсоноблэнерго,ЩЕДРОЛОСЕВ  ЕВГЕНИЙ НИКОНО,ХЕРСОН,Червоностудентська д.7А,кв.4;TR;1;1001;Eлектроенергiя;кВтчЧ;1.70;0;150;"; 
  /// response = "112;0;2;0;155641;AFD|AT|CH2C|AW|AM|CA|AN;AFD;3;43978;ЦЕНТРАЛIЗОВАНЕ ПОСТАЧАННЯ ХОЛОДНОЇ ВОДИ ТА ВОДОВIДВЕДЕННЯ;31524;0;947;1;43979;УТРИМАННЯ БУДИНКIВ ТА ПРИБУД. ТЕРИТОРIЙ;0;-1005;0;1;43980;ЦЕНТРАЛIЗОВАНЕ ВОДОВIДВЕДЕННЯ ГАРЯЧОЇ ВОДИ;4120;-032;294;1;AN;3;43978;1;0;12345;За несплату;43979;2;0;45123;За вибiр шлемазла;43980;3;0;34500;За зраду;AT;1;1;CH2C;3;43978;ПрАТ АК Київводоканал;03327629;ПАТ \"БАНК КРЕДИТ ДН?ПРО\"(м. Київ);305749;2600931011902;ЦЕНТРАЛIЗОВАНЕ ПОСТАЧАННЯ ХОЛОДНОЇ ВОДИ ТА ВОДОВIДВЕДЕННЯ;43979;К П Г I О Ц;04013755;ФГОЛОВНЕ УПРАВЛ?ННЯ ПО АТОЩАД М.КИЇВ;322669;26031300763387;УТРИМАННЯ БУДИНКIВ ТА ПРИБУД. ТЕРИТОРIЙ;43980;ПрАТ АК Київводоканал;03327629;ПАТ \"БАНК КРЕДИТ ДНIПРО\"(м. Київ);305749;2600931011902;ЦЕНТРАЛIЗОВАНЕ ВОДОВ?ДВЕДЕННЯ ГАРЯЧОЇ ВОДИ;AW;КАЧМАЛА Л.М.;ГАЙДАЙ 3ОЇ ВУЛ. Д7 КВ136;*;AM;0;3;3;CA;9483;2017.11.10 10:52:43;999900000001;Fba7WoMYXLqGJdRtHMM0VxWdRGsmCJFBJXwf1UzO6DIydIAOl8y02Et1tAKP22V96oafkMj7HWAPtKZNXOLQeFZ6tTU+LbiP+CKSjppWdMYaezPAZML07HMx7GcmZXL00tqnB486wZhzNfNAdrPirDoHHkNtiypLb2CRj56RSGQ=";
  /// response = "212;0;*;2019.05.28 15:05:25;AFD|AT|CH2C|AW|AM|CA|AN;AFD;2;85657;ЦЕНТРАЛ_ЗОВАНЕ ПОСТАЧАННЯ ХОЛОДНОЇ ВОДИ ТА ВОДОВ_ДВЕДЕННЯ;47863;0;0;1;85658;ЦЕНТРАЛ_ЗОВАНЕ ВОДОВ_ДВЕДЕННЯ ГАРЯЧОЇ ВОДИ;6127;0;0;1;AN;2;85657;1;0;12345;За несплату;85658;2;0;45123;За вибiр шлемазла;AT;1;1;CH2C;2;85657;ПРАТ \"АК\" КИЇВВОДОКАНАЛ\";03327629;ПАТ \"БАНК КРЕДИТ ДН_ПРО\"(м. Київ);305749;2600931011902;ЦЕНТРАЛ_ЗОВАНЕ ПОСТАЧАННЯ ХОЛОДНОЇ ВОДИ ТА ВОДОВ_ДВЕДЕННЯ;85658;ПРАТ \"АК\" КИЇВВОДОКАНАЛ\";03327629;ПАТ \"БАНК КРЕДИТ ДН_ПРО\"(м. Київ);305749;2600931011902;ЦЕНТРАЛ_ЗОВАНЕ ВОДОВ_ДВЕДЕННЯ ГАРЯЧОЇ ВОДИ;AW;КАЧМАЛА Л.М.;ГАЙДАЙ 3ОЇ ВУЛ. Б7 КВ136;*;AM;0;3;3;CA;22968";
  /// response = "212;1;Рахунок не сформовано. Причини: клiєнту не було надiслано послуги вiд ТРЦ. надiслани послуги, по яким не маємо параметри чека. у Банка не пiдписаний договiр по будь-якiй з послуг, якi отриманi вiд ТРЦ.  Код ошибки Result =  924604. ;2019.06.03 17:35:55;*";
  /// response = "212;0;*;2019.07.02 01:18:01;ACE|AFE|AM|AN|AT|AW|CA|CN|PR;AM;0;3;0;AW;ТИМОШЕHКО К Г;пpоспект Молодi 11 кв 80;*;AT;1;0;CA;1331;PR;3;2019.05.01;2019.05.31;AFE;11;15002;УТРИМ.БУДИHКУ;0;*;24722;-24722;0;0;0;1;15003;ВИВIЗ СМIТТЯ;0;*;6164;-6164;0;0;0;1;15004;АВАHС за газ;0;*;15616;0;0;0;15616;1;15005;ОПАЛЕHHЯ;0;*;0;0;0;0;0;1;15006;СТОКИ ГАР.ВОДИ;0;*;0;0;0;0;0;1;15007;Г А З;0;*;10095;-103155;0;0;-93060;1;15008;ВОДА;0;*;12038;0;0;0;12038;1;15009;ПЛАТА ЗА ЗЕМЛЮ;0;*;21;-21;0;0;0;1;15010;Каб.Телб.Воля;0;*;19900;-36321;0;0;-16421;1;15011;ДОМОФОHт788055;0;*;1600;-1600;0;0;0;1;15012;ЕЛЕКТР.ПО ЛIЧИЛ;0;*;43608;-43608;0;0;0;1;ACE;2;16013;ГАРЯЧ.ВОДА ЛIЧ.;107.0700000;*;0;0;0;0;1;16013;16014;ГАЗ ПО ЛIЧИЛЬHИ;8.4125300;*;0;0;0;0;1;16014;CN;2;16013;0;625.50;0;0;ГАРЯЧ.ВОДА ЛIЧ.;16014;0;54.00;0;0;ГАЗ ПО ЛIЧИЛЬHИ;AN;1;15004;1;0;100;*";
  /// Tags CNE|IP
  /// response = "112;0;57;0;030350040546;AM|MC|CNE|IP;AM;0;0;0;MC;ул. Андриевского, д. 16;CNE;HW_1;22.07.2022;0;2;ZC1;5.10;12;19;7;ZC2;3.15;10;15;5;IP;1111;2222;Add info;2;1;One;2;Two;2022.07.21 10:10:44;999900000001;iN45jZY1n4NgjMzn4WPha9xEBSzrW0XOglXHIKv1UYpj6GF73lRMNumYk6UciPgoHquUMbQcT5emxDzADAC3QkgeXApEQoSpI7w7LgzQD6i4GcJXTIed9qEQmcVH0iushJtCq8+9ryDPwa+htYCxMrdToOPLA+3+3lgiky3ygqbFj67G7ZgFCrkA+x5AvMCVb/EJwQK7m2RCLkVBEh3rMlrRk29ot6NFWPCm0FVyjVcW4IKcjEekISEHcfOLF7KgmA4Yhnkic+kzw9BsuS4QFxpdmIDQf2BpQPUWwDUlUvudl/pJBSn/TkweUsKrHk8lKYikVRw5i4rAS9CgS38/zQ==";
  /// response = "212;0;*;2022.07.25 10:09:01;AM|MC|CNE|IP;AM;0;0;0;MC;ул. Андриевского, д. 16;CNE;HW_1;22.07.2022;0;2;ZC1;5.10;12;19;7;ZC2;3.15;10;15;5;IP;1111;2222;Add info;2;1;One;2;Two;";
  /// response = "212;0;*;2022.10.25 03:01:24;AF|AT|CN|AC|PR|AW|AM|CA|ERR;AF;3;45330;ХКП \"Спецкомунтранс\";4931;1;45331;Утримання  будинку;0;1;45332;БУ № 2 Ракове;0;1;AT;1;1;CN;2;4533300;0;0;0;0;*;4533400;0;0;0;0;*;AC;2;45333;Водоканал (вода);8.4200;1;4533300;45334;Водоканал (стоки);8.0800;1;4533400;PR;2;2019.03.01;2019.03.31;AW;*;НЕЗАЛЕЖНОЇ УКРАЇНИ, буд. 36, кв. 41;*;AM;4931;3;3;CA;14268;ERR;11018;технічні роботи на стороні постачальника, сервіс тимчасово недоступний;21018;технічні роботи на стороні постачальника, сервіс тимчасово недоступний;";
  // response = "212;0;*;2022.12.19 15:32:16;AF|AT|CN|AC|PR|AW|AM|CA;AF;3;46383;ХКП \"Спецкомунтранс\";4931;1;46384;Утримання  будинку;0;1;46385;БУ № 2 Ракове;0;1;AT;1;1;CN;2;4638600;0;0;0;0;*;4638700;0;0;0;0;*;AC;2;46386;Водоканал (вода);8.4200;1;4638600;46387;Водоканал (стоки);8.0800;1;4638700;PR;2;2019.03.01;2019.03.31;AW;*;НЕЗАЛЕЖНОЇ УКРАЇНИ, буд. 36, кв. 41;*;AM;4931;3;3;CA;14587";
  // response = "212;0;*;2023.06.08 10:37:56;AM|AW|AF|CA|CH2C;AM;000;0;3;CA;1506;CH2C;4;4102;ТОВ \"ПРИКАРПАТЕНЕРГОТРЕЙД\";42129720;АТ \"ОЩАДБАНК\";0;UA883365030000026034300046328;Оплата за послуги;4103;ТОВ \"ПРИКАРПАТЕНЕРГОТРЕЙД\";42129720;АТ \"ОЩАДБАНК\";0;UA213365030000026001300046328;Оплата за послуги;4104;ТОВ \"ПРИКАРПАТЕНЕРГОТРЕЙД\";42129720;АТ \"ОЩАДБАНК\";0;UA213365030000026001300046328;Оплата за послуги;4105;ТОВ \"ПРИКАРПАТЕНЕРГОТРЕЙД\";42129720;АТ \"ОЩАДБАНК\";0;UA213365030000026001300046328;Оплата за послуги;AF;4;4102;активна електроенергія (універсальна послуга), Iд.клiєнта: 290941;0;1;4103;пеня (універсальна послуга), Iд.клiєнта: 290941;0;1;4104;інфляційні (універсальна послуга), Iд.клiєнта: 290941;0;1;4105;% річних (універсальна послуга), Iд.клiєнта: 290941;0;1";
  // response = "212;0;*;2023.10.26 17:58:32;AM|MC|CN;AM;0;1;0;MC;*;CN;1;0;0;0;0;0;*";
  // response = "218;5;*;2025.08.08 11:48:09;TT;TT;467144731";

#endif 
  //----------------------
  // Разбиваем телеграмму на отдельные поля
  //----------------------
  responseList = ParseWebServerRequest(
    response, 
    TelRecordDivider,
    Etx
  );
  //----------------------
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
int MillExtAcceptor::GetBillProvider (void)
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::GetBillProvider : ";
  int											    Result = OK;

  TWebRequestList             requestProviderList;
  TWebRequestList	            responseProviderList;
//   eInterReqType               respType;

  Result = CreateGetBillProviderReq ();

  CHECK_ERROR_RETURN(Result);

  requestProviderList = ResponseList;

//   Inform(MS
//     << "\n....................."
//     << "\nЗапрос счета - запрос"
//     << "\n.....................\n"
//     << getLogRequestText_HDR (GetBillProviderReq.HDR)
//     << getLogRequestText_BODY (GetBillProviderReq.HDR.billTags, GetBillProviderReq.BODY)
//     ,ClassID, mtMessage);

  LogRequest (MS
    << "\n....................."
    << "\nЗапрос счета - запрос"
    << "\n.....................\n"
    << getLogRequestText_HDR (GetBillProviderReq.HDR)
    << getLogRequestText_BODY (GetBillProviderReq.HDR.billTags, GetBillProviderReq.BODY)
    );


#ifdef EMULATE_INTEG_INTERNAL_REQUEST
  int N = 0;

  GetBillProviderResp.HDR.idMsgType = ukcGetBillProviderResp;
  GetBillProviderResp.HDR.uniKioskStatus = uksOK;
  GetBillProviderResp.HDR.answerMessage = "";
  GetBillProviderResp.HDR.payDateTime = GetCurrentDateTime();

  GetBillProviderResp.HDR.billTags[N++] = TAG_AMOUNT_PAY;
  GetBillProviderResp.BODY.tagAmountPay.amountToPay = GetRandomNumber(5);
  GetBillProviderResp.BODY.tagAmountPay.amountCommis = GetRandomNumber(3);

  GetBillProviderResp.HDR.billTags[N++] = TAG_MSG;
  GetBillProviderResp.BODY.tagMsg.msg2Client = "Test payment";

#else // EMULATE_INTEG_INTERNAL_REQUEST

  Result = execInternalServRequest (
    GetBillProviderReq.HDR.idService,
    GetBillProviderReq.HDR.idSubService,
    requestProviderList,

    GetBillProviderResp.HDR.uniKioskStatus,
    responseProviderList
    );

  CHECK_ERROR_RETURN(Result);

  RequestList = responseProviderList;

  Result = ParseGetBillProviderResp();

  CHECK_ERROR_RETURN(Result);

#endif // TEST_SERVER

//   Inform(MS
//     << "\n....................."
//     << "\nЗапрос счета - ответ"
//     << "\n.....................\n"
//     << getLogRequestText_HDR (GetBillProviderResp.HDR)
//     << getLogRequestText_BODY (GetBillProviderResp.HDR.billTags, GetBillProviderResp.BODY)
//     ,ClassID, mtMessage);

  LogRequest (MS
    << "\n....................."
    << "\nЗапрос счета - ответ"
    << "\n.....................\n"
    << getLogRequestText_HDR (GetBillProviderResp.HDR)
    << getLogRequestText_BODY (GetBillProviderResp.HDR.billTags, GetBillProviderResp.BODY)
    );

  return Result;
}

//----------------------------------------------------------------------
// Оплатить счет
// Вход
//		
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::PayBillProvider (void)
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::GetBillProvider : ";
  int											    Result = OK;

  TWebRequestList             requestProviderList;
  TWebRequestList	            responseProviderList;
  std::string							    stValue;
//   eInterReqType               respType;

  Result = CreatePayBillProviderReq ();

  CHECK_ERROR_RETURN(Result);

  requestProviderList = ResponseList;

//   Inform(MS
//     << "\n....................."
//     << "\nОплата счета - запрос"
//     << "\n.....................\n"
//     << getLogRequestText_HDR (PayBillProviderReq.HDR)
//     << getLogRequestText_BODY (PayBillProviderReq.HDR.billTags, PayBillProviderReq.BODY)
//     ,ClassID, mtMessage);

  LogRequest (MS
    << "\n....................."
    << "\nОплата счета - запрос"
    << "\n.....................\n"
    << getLogRequestText_HDR (PayBillProviderReq.HDR)
    << getLogRequestText_BODY (PayBillProviderReq.HDR.billTags, PayBillProviderReq.BODY)
    );

#ifdef EMULATE_INTEG_INTERNAL_REQUEST

  int N = 0;

  PayBillProviderResp.HDR.idMsgType = ukcPayBillProviderResp;
  PayBillProviderResp.HDR.uniKioskStatus = uksBillPayed;
  PayBillProviderResp.HDR.answerMessage = "";
  PayBillProviderResp.HDR.payDateTime = GetCurrentDateTime();

  PayBillProviderResp.HDR.billTags[N++] = TAG_AGENT_CODE;
  PayBillProviderResp.BODY.tagAgentCode.extReqNo = GetRandomNumber(7);

  PayBillProviderResp.HDR.billTags[N++] = TAG_AMOUNT_PAY;
  PayBillProviderResp.BODY.tagAmountPay.amountToPay = GetRandomNumber(5);
  PayBillProviderResp.BODY.tagAmountPay.amountCommis = GetRandomNumber(3);

#else // EMULATE_INTEG_INTERNAL_REQUEST

  Result = execInternalServRequest (
    PayBillProviderReq.HDR.idService,
    PayBillProviderReq.HDR.idSubService,
    requestProviderList,

    PayBillProviderResp.HDR.uniKioskStatus,
    responseProviderList
    );

  CHECK_ERROR_RETURN(Result);

  RequestList = responseProviderList;

  Result = ParsePayBillProviderResp();

  CHECK_ERROR_RETURN(Result);

#endif // TEST_SERVER

//   Inform(MS
//     << "\n....................."
//     << "\nОплата счета - ответ"
//     << "\n.....................\n"
//     << getLogRequestText_HDR (PayBillProviderResp.HDR)
//     << getLogRequestText_BODY (PayBillProviderResp.HDR.billTags, PayBillProviderResp.BODY)
//     ,ClassID, mtMessage);

  LogRequest (MS
    << "\n....................."
    << "\nОплата счета - ответ"
    << "\n.....................\n"
    << getLogRequestText_HDR (PayBillProviderResp.HDR)
    << getLogRequestText_BODY (PayBillProviderResp.HDR.billTags, PayBillProviderResp.BODY)
    );

  return Result;
}

//----------------------------------------------------------------------
// Запрос состояния операции
// Вход
//		
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::OperStatusProvider (void)
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::OperStatusProvider : ";
  int											    Result = OK;

  TWebRequestList             requestProviderList;
  TWebRequestList	            responseProviderList;
  std::string							    stValue;
//   eInterReqType               respType;

  Result = CreateServiceOperStatusProviderReq();

  CHECK_ERROR_RETURN(Result);

  requestProviderList = ResponseList;

//   Inform(MS
//     << "\n..................................."
//     << "\nЗапрос статуса транзакции - запрос"
//     << "\n...................................\n"
//     << getLogRequestText_HDR (ServiceOperStatusProviderReq.HDR)
//     << getLogRequestText_BODY (ServiceOperStatusProviderReq.HDR.billTags, ServiceOperStatusProviderReq.BODY)
//     ,ClassID, mtMessage);

  LogRequest (MS
    << "\n..................................."
    << "\nЗапрос статуса транзакции - запрос"
    << "\n...................................\n"
    << getLogRequestText_HDR (ServiceOperStatusProviderReq.HDR)
    << getLogRequestText_BODY (ServiceOperStatusProviderReq.HDR.billTags, ServiceOperStatusProviderReq.BODY)
    );

#ifdef EMULATE_INTEG_INTERNAL_REQUEST

  int N = 0;

  ServiceOperStatusProviderResp.HDR.idMsgType = ukcServiceOperStatusProviderResp;
  ServiceOperStatusProviderResp.HDR.uniKioskStatus = uksOK;
  ServiceOperStatusProviderResp.HDR. answerMessage = "";
  ServiceOperStatusProviderResp.HDR.payDateTime = GetCurrentDateTime();

//   PayBillProviderResp.HDR.billTags[N++] = TAG_AGENT_CODE;
//   PayBillProviderResp.BODY.tagAgentCode.extReqNo = GetRandomNumber(7);
// 
//   PayBillProviderResp.HDR.billTags[N++] = TAG_AMOUNT;
//   PayBillProviderResp.BODY.tagAmount.amountToPay = GetRandomNumber(5);
//   PayBillProviderResp.BODY.tagAmount.amountCommis = GetRandomNumber(3);
//   PayBillProviderResp.BODY.tagAmount.payMode = "0";

#else // EMULATE_INTEG_INTERNAL_REQUEST

  Result = execInternalServRequest (
    ServiceOperStatusProviderReq.HDR.idService,
    ServiceOperStatusProviderReq.HDR.idSubService,
    requestProviderList,

    ServiceOperStatusProviderResp.HDR.uniKioskStatus,
    responseProviderList
    );

  CHECK_ERROR_RETURN(Result);

  RequestList = responseProviderList;

  Result = ParseServiceOperStatusProviderResp();

  CHECK_ERROR_RETURN(Result);

#endif // TEST_SERVER

//   Inform(MS
//     << "\n....................."
//     << "\nЗапрос статуса транзакции - ответ"
//     << "\n.....................\n"
//     << getLogRequestText_HDR (ServiceOperStatusProviderResp.HDR)
//     << getLogRequestText_BODY (ServiceOperStatusProviderResp.HDR.billTags, ServiceOperStatusProviderResp.BODY)
//     ,ClassID, mtMessage);

  LogRequest (MS
    << "\n....................."
    << "\nЗапрос статуса транзакции - ответ"
    << "\n.....................\n"
    << getLogRequestText_HDR (ServiceOperStatusProviderResp.HDR)
    << getLogRequestText_BODY (ServiceOperStatusProviderResp.HDR.billTags, ServiceOperStatusProviderResp.BODY)
    );

  return Result;
}

//----------------------------------------------------------------------
// Сделать сторно по счету
// Вход
//		
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::StornoBillProvider (void)
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::StornoBillProvider : ";
  int											    Result = OK;

  TWebRequestList             requestProviderList;
  TWebRequestList	            responseProviderList;
  std::string							    stValue;
//   eInterReqType               respType;

  Result = CreateStornoProviderReq();

  CHECK_ERROR_RETURN(Result);

  requestProviderList = ResponseList;

//   Inform(MS
//     << "\n..................................."
//     << "\nСторно счета - запрос"
//     << "\n...................................\n"
//     << getLogRequestText_HDR (StornoProviderReq.HDR)
//     << getLogRequestText_BODY (StornoProviderReq.HDR.billTags, StornoProviderReq.BODY)
//     ,ClassID, mtMessage);

  LogRequest (MS
    << "\n..................................."
    << "\nСторно счета - запрос"
    << "\n...................................\n"
    << getLogRequestText_HDR (StornoProviderReq.HDR)
    << getLogRequestText_BODY (StornoProviderReq.HDR.billTags, StornoProviderReq.BODY)
    );

#ifdef EMULATE_INTEG_INTERNAL_REQUEST

  StornoProviderResp.HDR.idMsgType = ukcStornoProviderResp;
  // DEBUG >>>
  StornoProviderResp.HDR.uniKioskStatus = uksStornoSuccess;
//   StornoProviderResp.HDR.uniKioskStatus = uksOK;
  // <<< DEBUG
  StornoProviderResp.HDR.answerMessage = "";
  StornoProviderResp.HDR.payDateTime = GetCurrentDateTime();

#else // EMULATE_INTEG_INTERNAL_REQUEST

  Result = execInternalServRequest (
    StornoProviderReq.HDR.idService,
    StornoProviderReq.HDR.idSubService,
    requestProviderList,

    StornoProviderResp.HDR.uniKioskStatus,
    responseProviderList
    );

  CHECK_ERROR_RETURN(Result);

  RequestList = responseProviderList;

  Result = ParseStornoProviderResp();

  CHECK_ERROR_RETURN(Result);

#endif // TEST_SERVER

//   Inform(MS
//     << "\n....................."
//     << "\nСторно счета - ответ"
//     << "\n.....................\n"
//     << getLogRequestText_HDR (StornoProviderResp.HDR)
//     << getLogRequestText_BODY (StornoProviderResp.HDR.billTags, StornoProviderResp.BODY)
//     ,ClassID, mtMessage);

  LogRequest (MS
    << "\n....................."
    << "\nСторно счета - ответ"
    << "\n.....................\n"
    << getLogRequestText_HDR (StornoProviderResp.HDR)
    << getLogRequestText_BODY (StornoProviderResp.HDR.billTags, StornoProviderResp.BODY)
    );

  return Result;
}

//----------------------------------------------------------------------
// Запрос состояния операции ОДБ
// Вход
//		
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::StatusODBProvider (void)
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::StatusODBProvider : ";
  int											    Result = OK;

  TWebRequestList             requestProviderList;
  TWebRequestList	            responseProviderList;
  std::string							    stValue;
//   eInterReqType               respType;

  Result = CreateStatusODBProviderReq();

  CHECK_ERROR_RETURN(Result);

  requestProviderList = ResponseList;

  LogRequest (MS
    << "\n..................................."
    << "\nЗапрос статуса транзакции в ОДБ - запрос"
    << "\n...................................\n"
    << getLogRequestText_HDR (StatusODBProviderReq.HDR)
    << getLogRequestText_BODY (StatusODBProviderReq.HDR.billTags, StatusODBProviderReq.BODY)
    );

#ifdef EMULATE_INTEG_INTERNAL_REQUEST

  int N = 0;

  StatusODBProviderResp.HDR.idMsgType = ukcServiceOperStatusProviderResp;
  StatusODBProviderResp.HDR.uniKioskStatus = uksOK;
  StatusODBProviderResp.HDR. answerMessage = "";
  StatusODBProviderResp.HDR.payDateTime = GetCurrentDateTime();

//   PayBillProviderResp.HDR.billTags[N++] = TAG_AGENT_CODE;
//   PayBillProviderResp.BODY.tagAgentCode.extReqNo = GetRandomNumber(7);
// 
//   PayBillProviderResp.HDR.billTags[N++] = TAG_AMOUNT;
//   PayBillProviderResp.BODY.tagAmount.amountToPay = GetRandomNumber(5);
//   PayBillProviderResp.BODY.tagAmount.amountCommis = GetRandomNumber(3);
//   PayBillProviderResp.BODY.tagAmount.payMode = "0";

#else // EMULATE_INTEG_INTERNAL_REQUEST

  Result = execInternalServRequest (
    StatusODBProviderReq.HDR.idService,
    StatusODBProviderReq.HDR.idSubService,
    requestProviderList,

    StatusODBProviderResp.HDR.uniKioskStatus,
    responseProviderList
    );

  CHECK_ERROR_RETURN(Result);

  RequestList = responseProviderList;

  Result = ParseStatusODBProviderResp();

  CHECK_ERROR_RETURN(Result);

#endif // TEST_SERVER

//   Inform(MS
//     << "\n....................."
//     << "\nЗапрос статуса транзакции - ответ"
//     << "\n.....................\n"
//     << getLogRequestText_HDR (ServiceOperStatusProviderResp.HDR)
//     << getLogRequestText_BODY (ServiceOperStatusProviderResp.HDR.billTags, ServiceOperStatusProviderResp.BODY)
//     ,ClassID, mtMessage);

  LogRequest (MS
    << "\n....................."
    << "\nЗапрос статуса транзакции в ОДБ - ответ"
    << "\n.....................\n"
    << getLogRequestText_HDR (StatusODBProviderResp.HDR)
    << getLogRequestText_BODY (StatusODBProviderResp.HDR.billTags, StatusODBProviderResp.BODY)
    );

  return Result;
}

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
//   S_PROVIDER_GET_BILL_REQ     providerGetBillReq;
//   S_PROVIDER_GET_BILL_RESP    providerGetBillResp;
  TStdStringList	            billTagsForBill;
  std::string                 limAccount;
  std::string                 stLimit;
  std::string                 accountAmount;
  std::string					        MsgError;

  // Ставим статус по умолчанию ошибка сервера
  GetBillResp.HDR.uniKioskStatus = uksServerError;

  GetBillResp.HDR.idService = GetBillReq.HDR.idService;
  GetBillResp.HDR.idSubService = GetBillReq.HDR.idSubService;
  GetBillResp.HDR.idClient = GetBillReq.HDR.idClient;

  // DEBUG >>>
  /*
  if (GetBillReq.END.keyValue == "999900000022") {
    Inform (MS
      << "Отрабатываем патч для 22 байера"
      ,ClassID, mtWarning);
      
    if (GetTickCount() % 10 != 0) {
      int sleepTime = (GetTickCount() % 15000) + 5000;

      Inform (MS
        << "Ждем " << sleepTime << " msec"
        ,ClassID, mtWarning);

      Sleep(sleepTime);

      GetBillResp.HDR.uniKioskStatus = uksRepeatWithTheSameCertificate;

      Result = OK;

      return Result;
    }
  }
  */
  // <<< DEBUG

  // Вычитываем нужно ли проверять байер по лимитам денег на счету
  Result = DATABASE->getBuyerLimit (	
    GetBillReq.END.keyValue,

    IdBuyer,
    limAccount,
    stLimit
    );

  CHECK_ERROR_RETURN(Result);

  // Если нужно проверять лимиты байера
  if (stLimit == "1") {

      Result = BASE_SERVER_OBJECTS->MillRequest->GetAmountMill (	
        IdBuyer,
        accountAmount
        );

      CHECK_ERROR_RETURN(Result);

      // Если на счету недостаточно денег
      if (AmountToAmountCentInt(accountAmount) < AmountToAmountCentInt(limAccount)) {

        GetBillResp.HDR.uniKioskStatus = uksNoMoney;

        Result = ERROR_OUT_OF_BUYER_LIMIT;

        Inform(MSML
          << "Байер имеет недостаточную сумму на счете"
          << " accountAmount = " << accountAmount
          << " limAccount = " << limAccount
          ,ClassID, mtWarning, Result);

        return Result;
      }
  }

  // Проверяем параметры платежа в бд
  Result = DATABASE->IntegProcess111Stage1 (	
    GetBillReq.HDR.idService,
    GetBillReq.HDR.idSubService,
    GetBillReq.HDR.idClient,
    GetBillReq.END.idTerminal,
    GetBillReq.END.keyValue,

    GetBillResp.HDR.uniKioskStatus,
    IdBuyer,
    MsgError
  );

  if (!IsOkUniKioskStatus(GetBillResp.HDR.uniKioskStatus)) {

    Result = TagERRHandler(GetBillReq.HDR.idService, IdBuyer, MsgError, GetBillResp.BODY.tagErrInfo, GetBillResp.HDR.billTags);
    CHECK_ERROR_RETURN(Result);
    //if (MsgError[0] == '*')
    //{
    //  Result = prepareTags(
    //    GetBillReq.HDR.idService,
    //    IdBuyer,
    //    MsgError,
    //    GetBillResp.BODY.tagErrInfo,
    //    GetBillResp.HDR.billTags
    //  );
    //  CHECK_ERROR_RETURN(Result);
    //}

    return Result;
  }

  CHECK_ERROR_RETURN(Result);
  // -----------------------------------
  // Контроль тагов AWP|AWF
  // -----------------------------------
  if ( (ExistsTag(GetBillReq.HDR.billTags, TAG_ACC_OWNER_PAY)) || (ExistsTag(GetBillReq.HDR.billTags, TAG_ACC_OWNER_FACT)) )
  {
    TPosDateTime locPayDate = GetCurrentDateTime();
    // -----------------------------------
    // Якщо присутній таг AWP, то фіксуємо дані тага у БД
    // -----------------------------------
    if (ExistsTag(GetBillReq.HDR.billTags, TAG_ACC_OWNER_PAY))
    {
      Result = TagNameHandle("0", IdBuyer, GetBillReq.HDR.idService, "0", locPayDate, TAG_ACC_OWNER_PAY, GetBillResp.HDR.billTags, GetBillReq.BODY, GetBillResp.BODY, GetBillResp.HDR.uniKioskStatus);
      CHECK_ERROR_RETURN(Result);
/*
      Result = DATABASE->Add_AWPAWF(
        "0", // IdComBill
        IdBuyer,
        GetBillReq.HDR.idService,
        "0",
        locPayDate,
        TAG_ACC_OWNER_PAY,
        GetBillReq.BODY.tagAccOwnerPay,

        GetBillResp.HDR.uniKioskStatus,
        MsgError
      );

      if (!IsOkUniKioskStatus(GetBillResp.HDR.uniKioskStatus)) 
      {
        Result = TagERRHandler(GetBillReq.HDR.idService, IdBuyer, MsgError, GetBillResp.BODY.tagErrInfo, GetBillResp.HDR.billTags);
        CHECK_ERROR_RETURN(Result);
       
        return Result;
      }
*/
    }
    // -----------------------------------
    // Якщо присутній таг AWF, то фіксуємо дані тага у БД
    // -----------------------------------
    if (ExistsTag(GetBillReq.HDR.billTags, TAG_ACC_OWNER_FACT))
    {
      Result = TagNameHandle("0", IdBuyer, GetBillReq.HDR.idService, "0", locPayDate, TAG_ACC_OWNER_FACT, GetBillResp.HDR.billTags, GetBillReq.BODY, GetBillResp.BODY, GetBillResp.HDR.uniKioskStatus);
      CHECK_ERROR_RETURN(Result);
/*
      Result = DATABASE->Add_AWPAWF(
        "0", // IdComBill
        IdBuyer,
        GetBillReq.HDR.idService,
        "0",
        locPayDate,
        TAG_ACC_OWNER_FACT,
        (S_tagAccOwnerPay)GetBillReq.BODY.tagAccOwnerFact,

        GetBillResp.HDR.uniKioskStatus,
        MsgError
      );

      if (!IsOkUniKioskStatus(GetBillResp.HDR.uniKioskStatus))
      {
        Result = TagERRHandler(GetBillReq.HDR.idService, IdBuyer, MsgError, GetBillResp.BODY.tagErrInfo, GetBillResp.HDR.billTags);
        CHECK_ERROR_RETURN(Result);

        return Result;
      }
*/
    }
  }

  // -----------------------------------
  // Контроль тагов AP|AL|AW
  // -----------------------------------
  if ( ( ((StringToInt(GetBillReq.HDR.idService) >= 9000) && (StringToInt(GetBillReq.HDR.idService) <= 9100)) == false ) 
    && ( (ExistsTag(GetBillReq.HDR.billTags, TAG_AMOUNT_PAY))||( ExistsTag(GetBillReq.HDR.billTags, TAG_AMOUNT_CASHLESS) ) ) )
  {
    bool isAWPresent = false;
    std::string	locTagType, locAmount = "0";

    if (ExistsTag(GetBillReq.HDR.billTags, TAG_AMOUNT_PAY))
    {
      locTagType = TAG_AMOUNT_PAY; // AP
      locAmount  = GetBillReq.BODY.tagAmountPay.amountToPay;
    }
    
    if (ExistsTag(GetBillReq.HDR.billTags, TAG_AMOUNT_CASHLESS))
    {
      locTagType = TAG_AMOUNT_CASHLESS; // AL
      locAmount  = GetBillReq.BODY.tagAmountCashless.AmountToPay;
    }

    if (ExistsTag(GetBillReq.HDR.billTags, TAG_ACC_OWNER))
      isAWPresent = true;

    /*
      std::string	inTagType,
      std::string	inAWperson,
      std::string	inAWaddress,
      std::string	inAWother,
      std::string	inIdService,

      std::string	inIdBuyer,
      std::string	inIdClient,
      std::string	inIdTerm,
      std::string	inAmount,
    */

    Result = DATABASE->IntegProcess111Stage0_v1 (	
      locTagType,
      (isAWPresent ? GetBillReq.BODY.tagAccOwner.person  : "*"),
      (isAWPresent ? GetBillReq.BODY.tagAccOwner.address : "*"),
      (isAWPresent ? GetBillReq.BODY.tagAccOwner.other   : "*"),
      GetBillReq.HDR.idService,
      IdBuyer,
      GetBillReq.HDR.idClient,
      GetBillReq.END.idTerminal,
      locAmount,

      GetBillResp.HDR.uniKioskStatus,
      MsgError
    );

    if (!IsOkUniKioskStatus(GetBillResp.HDR.uniKioskStatus)) {

      Result = TagERRHandler(GetBillReq.HDR.idService, IdBuyer, MsgError, GetBillResp.BODY.tagErrInfo, GetBillResp.HDR.billTags);
      CHECK_ERROR_RETURN(Result);
      //if (MsgError[0] == '*')
      //{
      //  Result = prepareTags(
      //    GetBillReq.HDR.idService,
      //    IdBuyer,
      //    MsgError,
      //    GetBillResp.BODY.tagErrInfo,
      //    GetBillResp.HDR.billTags
      //  );
      //  CHECK_ERROR_RETURN(Result);
      //}

      return Result;
    }

    CHECK_ERROR_RETURN(Result);
  }
  // -----------------------------------
  // Читаем интегрированные настройки для полученного сервиса
  Result = DATABASE->GetIntegServiceConfig (
    GetBillReq.HDR.idService,

    conf
    );
  CHECK_ERROR_RETURN(Result);

  // Если ошибка выходим со статусом общей ошибки
  if (Result != OK) {
    GetBillResp.HDR.uniKioskStatus = uksServerError;

    return Result;
  }

  // Если статус универсального киоска не успешен сразу отвечаем ошибкой
  if (!IsOkUniKioskStatus(GetBillResp.HDR.uniKioskStatus)) {
    return Result;
  }

  // Ставим ошибку по умолчанию
  GetBillResp.HDR.uniKioskStatus = uksRepeatWithTheSameCertificate;

// Заголовок телеграмм запроса
// struct S_PROVIDER_HEADER_REQ {
//   eUniKioskClientRequest idMsgType;		    // Идентификатор сообщения
//   std::string					idService;        // Номер сервиса
//   std::string					idSubService;     // Код подуслуги
//   std::string					idClient;         // Номер счета клиента
//   TPosDateTime        payDateTime;      // Дата время запроса 
//   std::string					idTerminal;	      // Идентификатор терминала
//   std::string					idBuyer;	        // Идентификатор байера
//   TStdStringList	    billTags;         // Таги , переданные в сообщении, Если нет, то «*»
// };

  GetBillProviderReq.HDR.idService    = GetBillReq.HDR.idService;
  GetBillProviderReq.HDR.idSubService = GetBillReq.HDR.idSubService;
  GetBillProviderReq.HDR.idClient     = GetBillReq.HDR.idClient;
  GetBillProviderReq.HDR.payDateTime  = GetCurrentDateTime();
  GetBillProviderReq.HDR.idTerminal   = GetBillReq.END.idTerminal;
  GetBillProviderReq.HDR.idBuyer      = IdBuyer;
//   GetBillProviderReq.HDR.billTags.clear();

  // Копируем таги для запроса по предметному серверу
  GetBillProviderReq.HDR.billTags = GetBillReq.HDR.billTags;
  GetBillProviderReq.BODY = GetBillReq.BODY;

  if (ExistsTag(GetBillProviderReq.HDR.billTags, TAG_ACC_OWNER_PAY))
  {
    RemoveTag(GetBillProviderReq.HDR.billTags, TAG_ACC_OWNER_PAY);
  }

  if (ExistsTag(GetBillProviderReq.HDR.billTags, TAG_ACC_OWNER_FACT))
  {
    RemoveTag(GetBillProviderReq.HDR.billTags, TAG_ACC_OWNER_FACT);
  }

  // Заполняем таг криптографии
  if (ExistsTag(GetBillReq.HDR.billTags, TAG_CRYPTO) ) {
    eKeyType		 keyType;

    RemoveTag(GetBillProviderReq.HDR.billTags, TAG_CRYPTO);

    Result = DATABASE->GetKeyType (	
      GetBillReq.END.idTerminal,
			GetBillReq.END.keyValue,

			keyType
			);

    CHECK_ERROR_RETURN(Result);

    GetBillProviderReq.BODY.tagCryptoInternal.msgCrypto = GetBillReq.BODY.tagCrypto.msgCrypto;
    GetBillProviderReq.BODY.tagCryptoInternal.idkey = GetBillReq.END.keyValue;
    GetBillProviderReq.BODY.tagCryptoInternal.keytype = IntToString(keyType);

    AddTag(GetBillProviderReq.HDR.billTags, TAG_CRYPTO_INTERNAL);
  }

  // Обращаемся к серверу и вычитываем данные счета
  Result = GetBillProvider ();

  CHECK_ERROR_RETURN(Result);

  GetBillResp.HDR.billTags = GetBillProviderResp.HDR.billTags;
  GetBillResp.BODY = GetBillProviderResp.BODY;

  // Удаляем ненужные таги
  RemoveTag (GetBillResp.HDR.billTags, TAG_ID_COMP);
  RemoveTag (GetBillResp.HDR.billTags, TAG_ID_MERCHANT);

//   if (!IsOkUniKioskStatus(GetBillProviderResp.HDR.uniKioskStatus)) {
//     return Result;
//   }

  // Делаем обработку запроса после оплаты
  Result = DATABASE->IntegProcess111Stage2 (	
    GetBillReq.HDR.idService,
    GetBillReq.HDR.idSubService,
    GetBillReq.HDR.idClient,
    GetBillReq.END.idTerminal,
    GetBillReq.END.keyValue,
    GetBillProviderResp.HDR.uniKioskStatus,

    GetBillResp.HDR.uniKioskStatus,
    MsgError
  );

  CHECK_ERROR_RETURN(Result);

  if (!IsOkUniKioskStatus(GetBillResp.HDR.uniKioskStatus)) {

    Result = TagERRHandler(GetBillReq.HDR.idService, IdBuyer, MsgError, GetBillResp.BODY.tagErrInfo, GetBillResp.HDR.billTags);
    CHECK_ERROR_RETURN(Result);

    //if (MsgError[0] == '*')
    //{
    //  Result = prepareTags(
    //    GetBillReq.HDR.idService,
    //    IdBuyer,
    //    MsgError,
    //    GetBillResp.BODY.tagErrInfo,
    //    GetBillResp.HDR.billTags
    //  );
    //  CHECK_ERROR_RETURN(Result);
    //}
  }

  if (IsOkUniKioskStatus(GetBillResp.HDR.uniKioskStatus)) {

    Result = DATABASE->GetServiceTagsBill (
      GetBillReq.HDR.idService,
      IdBuyer,

      billTagsForBill
    );

    CHECK_ERROR_RETURN(Result);

    Result = addIntegratorRequestTags (
      billTagsForBill,
      GetBillReq.HDR,
      GetBillReq.BODY,
      GetBillProviderResp.HDR, 
      GetBillProviderResp.BODY,

      GetBillResp.BODY,
      GetBillResp.HDR.billTags,
      GetBillResp.HDR.uniKioskStatus
      );

    CHECK_ERROR_RETURN(Result);
  }

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
  int	Result  = OK;
  TID	ClassID = tidGivs;
  std::string	ProcedureName = "MillExtAcceptor::AcceptPayBillMill : ";

  std::string           MillOperationId;
  TPosDateTime					MillAcceptTime;
  eMillAcceptStatus     MillAcceptStatus;
  eProviderAcceptStatus providerAcceptStatus = pasError;
  // std::string newIdComBill;
  eRepeatReq  repeatReq = errFirstReq;
  std::string	providerMessage;
  std::string	providerExtNo = "-1";
  std::string	providerIdCompany = "0";
  std::string	idCompany;
  std::string	checkText;
  
  bool           isUpdateMillAcceptTime = false;
  TPosDateTime   payDateTime = GetCurrentDateTime();
  TStdStringList billTagsForPay;

  std::string		 MsgError;
  // ------------------------------------------------
  // S_PROVIDER_PAY_BILL_REQ  providerPayBillReq;
  // S_PROVIDER_PAY_BILL_RESP providerPayBillResp;
  // ------------------------------------------------
  //std::string TelNumber, ClientCode;

  //Result = CheckIdClientforSmsParams(PayBillReq.HDR.idClient, TelNumber, ClientCode);
  //CHECK_ERROR_RETURN(Result);
  // ------------------------------------------------
  // Додано АГ 2019-03-05. Змушений був за наявнiстю тага AP чи AL
  // зробити iнiцiалiзацiю змiнних amountToPay та amountCommis,
  // щоб можна було пiдставити цi значення.
  // ------------------------------------------------
  std::string amountToPay = "0", amountCommis = "0";

  if ( ExistsTag(PayBillReq.HDR.billTags, TAG_AMOUNT_PAY) == true )
  {
    amountToPay  = PayBillReq.BODY.tagAmountPay.amountToPay;
    amountCommis = PayBillReq.BODY.tagAmountPay.amountCommis;
  }

  if ( ExistsTag(PayBillReq.HDR.billTags, TAG_AMOUNT_CASHLESS) == true )
  {
    amountToPay  = PayBillReq.BODY.tagAmountCashless.AmountToPay;
    amountCommis = PayBillReq.BODY.tagAmountCashless.AmountCommis;
  }  
  // ------------------------------------------------
	// Ставим статус по умолчанию ошибка сервера
  // ------------------------------------------------
	PayBillResp.HDR.uniKioskStatus = uksRepeatWithTheSameCertificate;
  PayBillResp.END.msgDateTime    = GetCurrentDateTime();

  MillAcceptTime = GetCurrentDateTime();
  MillAcceptStatus = masError;
  MillOperationId = "-1";

  // PayBillResp.BODY.tagAgentCode.extReqNo = PayBillReq.BODY.tagAgentCode.extReqNo;
  PayBillResp.HDR.idService    = PayBillReq.HDR.idService;
  PayBillResp.HDR.idSubService = PayBillReq.HDR.idSubService;
  PayBillResp.HDR.idClient     = PayBillReq.HDR.idClient;

  // Добавляем выходные обязательные параметры
  AddTag(PayBillResp.HDR.billTags, TAG_AGENT_CODE);
  PayBillResp.BODY.tagAgentCode.extReqNo = PayBillReq.BODY.tagAgentCode.extReqNo;

  // Копируем таги для запроса по предметному серверу
  PayBillProviderReq.HDR.billTags = PayBillReq.HDR.billTags;
  PayBillProviderReq.BODY         = PayBillReq.BODY;

  // Удаляем таги, которые не должны идти на предметный сервер
  RemoveTag (PayBillProviderReq.HDR.billTags, TAG_AGENT_CODE);

  // ------------------------------------------------
  // 2022-01-12 at 16:00 by AG, Reason: https://mail.google.com/mail/u/0/?tab=rm&ogbl#inbox/FMfcgzGmtNjKsSNvwJdKZdVkZvTPKKTh
  // ------------------------------------------------
  if (amountToPay == "0" || amountToPay == "0.00") {
    Result = ERROR_INVALID_BILL_AMOUNT;

    PayBillResp.HDR.uniKioskStatus = uksToSmallAmount;

    Inform(MSML << "Сума операції менша або дорівнює нулю:\nAmountToPay = " << amountToPay
      << "\nIdBuyer = " << IdBuyer
      << "\nextReqNo = " << PayBillReq.BODY.tagAgentCode.extReqNo
      // << "\nidComBill = " << idComBill
      , ClassID, mtError, Result);

    return Result;
  }

  // Обновляем мониторинг
  Result = SetMonitoring_IdService(PayBillReq.HDR.idService);
  CHECK_ERROR_RETURN(Result);

  /// Result = SetMonitoring_Amt(PayBillReq.BODY.tagAmountPay.amountToPay);
  Result = SetMonitoring_Amt(amountToPay);
  CHECK_ERROR_RETURN(Result);

  // Читаем интегрированные настройки для полученного сервиса
  Result = DATABASE->GetIntegServiceConfig ( PayBillReq.HDR.idService, conf );
  CHECK_ERROR_RETURN(Result);

  // Выделяем новый номер тразакции
  Result = BASE_DATABASE->GetIdComBill(idComBill);
  CHECK_ERROR_RETURN(Result);

#ifdef DISABLE_SEQUENCER

  idMillRq = "-1";
  idTransaction = "-1";

#else 
  Result = BASE_DATABASE->GetIdMillRq (	idMillRq );
  CHECK_ERROR_RETURN(Result);

  Result = BASE_DATABASE->GetIdTransaction ( idTransaction );
  CHECK_ERROR_RETURN(Result);
#endif // DISABLE_SEQUENCER

  // Проверяем статус операции в БД, версія IntegProcess113Stage1_v5
  //Result = DATABASE->IntegProcess113Stage1 (
  //  idComBill,
  //  idMillRq,
  //  idTransaction,
  //  PayBillReq.BODY.tagAgentCode.extReqNo,
  //  PayBillReq.HDR.idService,
  //  PayBillReq.HDR.idSubService,
  //  PayBillReq.HDR.idClient,
  //  amountToPay,   /// PayBillReq.BODY.tagAmountPay.amountToPay,
  //  amountCommis,  /// PayBillReq.BODY.tagAmountPay.amountCommis,
  //  PayBillReq.END.idTerminal,
  //  PayBillReq.END.keyValue,

  //  PayBillResp.HDR.uniKioskStatus,
  //  uniBillStatus,
  //  idComBill,
  //  IdBuyer,
  //  checkText,
  //  idCompany,
  //  isMillOvertaking,
  //  isProviderOvertaking,
  //  isSuccessLocked,
  //  isAlwaysSendToProvider,
  //  payDateTime,
  //  MsgError
  //);
  // Проверяем статус операции в БД, версія IntegProcess113Stage1_v6
  Result = DATABASE->IntegProcess113Stage1(
    idComBill,
    idMillRq,
    idTransaction,
    PayBillReq.BODY.tagAgentCode.extReqNo,
    PayBillReq.HDR.idService,
    PayBillReq.HDR.idSubService,
    PayBillReq.HDR.idClient,
    amountToPay,   /// PayBillReq.BODY.tagAmountPay.amountToPay,
    amountCommis,  /// PayBillReq.BODY.tagAmountPay.amountCommis,
    PayBillReq.END.idTerminal,
    PayBillReq.END.keyValue,

    PayBillResp.HDR.uniKioskStatus,
    MsgError,
    uniBillStatus,
    idComBill,
    IdBuyer,
    // checkText,
    idCompany,
    isMillOvertaking,
    isProviderOvertaking,
    isSuccessLocked,
    isAlwaysSendToProvider,
    payDateTime
  );
  CHECK_ERROR_RETURN(Result);

  Result = TagERRHandler(PayBillReq.HDR.idService, IdBuyer, MsgError, PayBillResp.BODY.tagErrInfo, PayBillResp.HDR.billTags);
  CHECK_ERROR_RETURN(Result);

  std::string TelNumber, ClientCode;

  Result = CheckIdClientforSmsParams(PayBillReq.HDR.idClient, TelNumber, ClientCode);
  CHECK_ERROR_RETURN(Result);

  // Обновляем мониторинг
  Result = SetMonitoring_idComBill(idComBill);
  CHECK_ERROR_RETURN(Result);

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
      << "\nextReqNo = " << PayBillReq.BODY.tagAgentCode.extReqNo
      << "\nidComBill = " << idComBill
      ,ClassID, mtError, Result);

    return Result;
  }

  // Если неуспешный статус то тоже выходим
  if (!IsOkUniKioskStatus(PayBillResp.HDR.uniKioskStatus)) {
    return Result;
  }

  // -----------------------------------
  // Контроль тагов AWP|AWF
  // -----------------------------------
  if ((ExistsTag(PayBillReq.HDR.billTags, TAG_ACC_OWNER_PAY)) || (ExistsTag(PayBillReq.HDR.billTags, TAG_ACC_OWNER_FACT)))
  {
    TPosDateTime locPayDate = GetCurrentDateTime();
    // -----------------------------------
    // Якщо присутній таг AWP, то фіксуємо дані тага у БД
    // -----------------------------------
    if (ExistsTag(PayBillReq.HDR.billTags, TAG_ACC_OWNER_PAY))
    {
      Result = TagNameHandle(idComBill, IdBuyer, PayBillReq.HDR.idService, amountToPay, locPayDate, TAG_ACC_OWNER_PAY, PayBillResp.HDR.billTags, PayBillReq.BODY, PayBillResp.BODY, PayBillResp.HDR.uniKioskStatus);
      CHECK_ERROR_RETURN(Result);
/*
      Result = DATABASE->Add_AWPAWF(
        idComBill, // "0", // IdComBill
        IdBuyer,
        PayBillReq.HDR.idService,
        amountToPay,
        locPayDate,
        TAG_ACC_OWNER_PAY,
        PayBillReq.BODY.tagAccOwnerPay,

        PayBillResp.HDR.uniKioskStatus,
        MsgError
      );

      if (!IsOkUniKioskStatus(PayBillResp.HDR.uniKioskStatus))
      {
        Result = TagERRHandler(PayBillReq.HDR.idService, IdBuyer, MsgError, PayBillResp.BODY.tagErrInfo, PayBillResp.HDR.billTags);
        CHECK_ERROR_RETURN(Result);

        return Result;
      }
*/
    }
    // -----------------------------------
    // Якщо присутній таг AWF, то фіксуємо дані тага у БД
    // -----------------------------------
    if (ExistsTag(PayBillReq.HDR.billTags, TAG_ACC_OWNER_FACT))
    {
      Result = TagNameHandle(idComBill, IdBuyer, PayBillReq.HDR.idService, amountToPay, locPayDate, TAG_ACC_OWNER_FACT, PayBillResp.HDR.billTags, PayBillReq.BODY, PayBillResp.BODY, PayBillResp.HDR.uniKioskStatus);
      CHECK_ERROR_RETURN(Result);
/*
      Result = DATABASE->Add_AWPAWF(
        idComBill, // "0", // IdComBill
        IdBuyer,
        PayBillReq.HDR.idService,
        amountToPay,
        locPayDate,
        TAG_ACC_OWNER_FACT,
        (S_tagAccOwnerPay)PayBillReq.BODY.tagAccOwnerFact,

        PayBillResp.HDR.uniKioskStatus,
        MsgError
      );

      if (!IsOkUniKioskStatus(PayBillResp.HDR.uniKioskStatus))
      {
        Result = TagERRHandler(PayBillReq.HDR.idService, IdBuyer, MsgError, PayBillResp.BODY.tagErrInfo, PayBillResp.HDR.billTags);
        CHECK_ERROR_RETURN(Result);

        return Result;
      }
*/
    }
  }

  AddTag(PayBillResp.HDR.billTags, TAG_TRANSACTION);
  PayBillResp.BODY.tagTransaction.idComBill = idComBill;

  Result = storeTagsList (
    idComBill,
    conf,
    PayBillReq.HDR
    );

  CHECK_ERROR_RETURN(Result);

  // Если счет уже оплачен то ничего не делаем и просто выходим
  if (uniBillStatus == ubsBillPayed && !isAlwaysSendToProvider) {

    Result = DATABASE->GetServiceTagsPay_v2 (
      PayBillReq.HDR.idService,
      IdBuyer,

      billTagsForPay
    );

    CHECK_ERROR_RETURN(Result);

    // Если необходимо выбирать чек по IdCompany
    if (StringToInt(idCompany) > 0 && conf.useIdCompanyCheque) {

      S_tagIdCompData idCompData;

      idCompData.idCompany = idCompany;

      AddTag(PayBillProviderResp.HDR.billTags, TAG_ID_COMP);
      PayBillProviderResp.BODY.tagIdComp[1] = idCompData;
    }

    Result = addIntegratorRequestTags (
      billTagsForPay,
      PayBillReq.HDR,
      PayBillReq.BODY,
      PayBillProviderResp.HDR,
      PayBillProviderResp.BODY,

      PayBillResp.BODY,
      PayBillResp.HDR.billTags,
      PayBillResp.HDR.uniKioskStatus
      );

//     CHECK_ERROR_RETURN(Result);

    // Если не смогли смогли сформировать ответные таги выставляем статус повторить транзакцию позже
    if (Result != OK) {
      PayBillResp.HDR.uniKioskStatus = uksServerBusy;
    } else {
      PayBillResp.HDR.uniKioskStatus = uksBillPayedEarly;
      /// PayBillResp.END.msgDateTime = payDateTime;
      PayBillResp.END.msgDateTime = GetCurrentDateTime();

      Result = OK;
    }
    /// Робимо ініціаліацію тага DO
    /// if (IsOkUniKioskStatus(PayBillResp.HDR.uniKioskStatus))
    if (IsOkUniKioskStatus(PayBillResp.HDR.uniKioskStatus) || IsNotOkFinalUniKioskStatus (PayBillResp.HDR.uniKioskStatus) )
    { 
      if ( ExistsTag(PayBillResp.HDR.billTags, TAG_DATE_OP) == true )
      {
        TPosDateTime locDateOp;
        /// Вичитуємо з таблиці transactionscomm DateOp
        Result = DATABASE->GetDateOp ( idComBill, locDateOp ); /// CHECK_ERROR_RETURN(Result);

        if ( Result != OK )
        {
          Result = OK;
          RemoveTag (PayBillResp.HDR.billTags, TAG_DATE_OP);
        } /// if ( Result != OK )
        else 
        {
          PayBillResp.BODY.tagDateOp.DateOperation = TPosDateTime_To_DateTimeString(locDateOp);
        } /// else
      } /// if ( ExistsTag(PayBillResp.HDR.billTags, TAG_DATE_OP) == true )
    } /// if (IsOkUniKioskStatus(PayBillProviderResp.HDR.uniKioskStatus))

    Inform(MS << "Счет был уже оплачен ранее", ClassID, mtWarning);

    return Result;

  } if (uniBillStatus == ubsBillPayed && isAlwaysSendToProvider) {
    uniBillStatus = ubsDangCertificateAccepted;

    Inform(MS << "Счет был уже оплачен ранее", ClassID, mtWarning);
  }

	// Ставим статус по умолчанию повторить с тем же ваучером
	PayBillResp.HDR.uniKioskStatus = uksRepeatWithTheSameCertificate;

  // Сохраняем в бд присланные таги 
  Result = storeIntegratorRequestTags (
    PayBillResp.BODY.tagTransaction.idComBill,
    PayBillReq.HDR,
    PayBillReq.BODY
    );

  if (Result != OK)
  {
    if ( Result == ERROR_FIELD_OUT_OF_RESULT_SET_RANGE )
    {
      PayBillResp.HDR.uniKioskStatus = uksServerError;
    }

    CHECK_ERROR_RETURN(Result);
  }

  /// CHECK_ERROR_RETURN(Result);

  // Якщо ваучер не акцептовано і це перша спроба списати кошти з рахунку, тоді робимо запис у таблиці
  // select * from payowner; тага AW та AL
  // if ( (uniBillStatus == ubsPayStarted) || (uniBillStatus == ubsDangCertificateAccepted) )
  if ( uniBillStatus == ubsPayStarted )
  {
    S_tagAccOwner       locTagAccOwner;
    S_tagAmountCashless locTagAL;
    locTagAL.PayType   = 0; /// Вид оплати:0 - готівка, 1 - платіжною картою, 2 - нша безготівкова оплата

    bool isDropTagAwAlToDb = false;

    std::string locAmountTx = "0";
    TStdStringList locBillTags;
    locBillTags.clear();

    locBillTags = PayBillReq.HDR.billTags;
    /// AP
    if ( ExistsTag( locBillTags, TAG_AMOUNT_PAY ) == true )
    {
      locAmountTx = PayBillReq.BODY.tagAmountPay.amountToPay;
    }
    /// AM
    //if ( ExistsTag( locBillTags, TAG_AMOUNT_MERCHANT ) == true )
    //{
    //  locAmountTx = PayBillReq.BODY.tagAmountMerchant.amount;
    //}
     /// AW
    if ( ExistsTag( locBillTags, TAG_ACC_OWNER ) == true )
    {   
      isDropTagAwAlToDb = true;
      locTagAccOwner.person  = PayBillReq.BODY.tagAccOwner.person;
      locTagAccOwner.address = PayBillReq.BODY.tagAccOwner.address;
      locTagAccOwner.other   = PayBillReq.BODY.tagAccOwner.other; // ІПН| дата народження (dd.mm.yyyy) | місце народження | телефон |серія номер паспорта (Idкартки)|дата видачі парспорта.        
    } /// if ( ExistsTag( locBillTags, TAG_ACC_OWNER ) == true )
    //// AL
    if ( ExistsTag( locBillTags, TAG_AMOUNT_CASHLESS ) == true )
    {
      isDropTagAwAlToDb = true;
      locAmountTx        = PayBillReq.BODY.tagAmountCashless.AmountToPay;
      locTagAL.PayType   = PayBillReq.BODY.tagAmountCashless.PayType;
      locTagAL.IdPayFund = PayBillReq.BODY.tagAmountCashless.IdPayFund;
    } /// if ( ExistsTag( locBillTags, TAG_AMOUNT_CASHLESS ) == true )

    if ( isDropTagAwAlToDb == true )
    {
      Result = DATABASE->ag_AddToPayOwner (
        idComBill,
        IdBuyer,
        PayBillReq.HDR.idService,
        locAmountTx,
        payDateTime,
        locTagAccOwner.person,
        locTagAccOwner.address,
        locTagAccOwner.other,
        locTagAL.PayType, 
        locTagAL.IdPayFund,
        MsgError
      );

      if ( Result != OK)
      {
        PayBillResp.HDR.uniKioskStatus = uksInvalidFBData; /// 89

        Result = TagERRHandler(PayBillReq.HDR.idService, IdBuyer, MsgError, PayBillResp.BODY.tagErrInfo, PayBillResp.HDR.billTags);
        CHECK_ERROR_RETURN(Result);

        //if (MsgError[0] == '*')
        //{
        //  Result = prepareTags(
        //    PayBillReq.HDR.idService,
        //    IdBuyer,
        //    MsgError,
        //    PayBillResp.BODY.tagErrInfo,
        //    PayBillResp.HDR.billTags
        //  );

        //  if (ExistsTag(PayBillResp.HDR.billTags, TAG_ERR_INFO)) {
        //    return Result;
        //  }
        //}

        return Result;
      } /// if ( Result != OK)
    } /// if ( isDropTagAwAlToDb == true )

  } /// if ( uniBillStatus == ubsPayStarted )
  // -----------------------------------------------------------------------------------
  // Робимо перевірку по лімітам відповідно до даних тагів AW та AL (якщо вони присутні)
  // -----------------------------------------------------------------------------------
  eUniBillStatus locUniBillStatus = ubsUnknown; 

  //Result = DATABASE->IntegProcess113Stage1Add0_v1 (
  //  idComBill,
  //  PayBillReq.HDR.idService,
  //  IdBuyer,
  //  amountToPay,
  //  PayBillReq.END.idTerminal,

  //  PayBillResp.HDR.uniKioskStatus,
  //  locUniBillStatus                 // uniBillStatus
  //);

  /// Замінили v1 на v2. Reason: https://mail.google.com/mail/u/0/#inbox/FMfcgxwJZJSxmvhkgXljbMscrTDCmrhW
  Result = DATABASE->IntegProcess113Stage1Add0_v2 (
    idComBill,
    PayBillReq.HDR.idService,
    IdBuyer,
    amountToPay,
    PayBillReq.END.idTerminal,
    PayBillReq.HDR.idClient,

    PayBillResp.HDR.uniKioskStatus,
    locUniBillStatus,                 // uniBillStatus
    MsgError
  );

  CHECK_ERROR_RETURN(Result);

  Result = TagERRHandler(PayBillReq.HDR.idService, IdBuyer, MsgError, PayBillResp.BODY.tagErrInfo, PayBillResp.HDR.billTags);
  CHECK_ERROR_RETURN(Result);

  //if (MsgError[0] == '*')
  //{
  //  Result = prepareTags(
  //    PayBillReq.HDR.idService,
  //    IdBuyer,
  //    MsgError,
  //    PayBillResp.BODY.tagErrInfo,
  //    PayBillResp.HDR.billTags
  //  );

  //  if (ExistsTag(PayBillResp.HDR.billTags, TAG_ERR_INFO)) {
  //    return Result;
  //  }
  //}

  if ( PayBillResp.HDR.uniKioskStatus != uksOK )
  {
    Result = ERROR_OUT_OF_BUYER_LIMIT; // 859513

    return Result;
  }
  
  // -----------------------------------------------------------------------------------
  // Ставим статус по умолчанию повторить с тем же ваучером
	PayBillResp.HDR.uniKioskStatus = uksRepeatWithTheSameCertificate;
  // -----------------------------------------------------------------------------------
	// Если ваучер не акцептирован то акцептируем ваучер
  // -----------------------------------------------------------------------------------
  if ( (uniBillStatus == ubsPayStarted) || (uniBillStatus == ubsDangCertificateAccepted) ) {

#ifdef USE_OLD_MILL_REQUEST
    Result = AcceptMoneyMill (
      PayBillResp.BODY.tagTransaction.idComBill,
      IdBuyer,
      PayBillReq.HDR.idService,
      PayBillReq.BODY.tagAgentCode.extReqNo,
      AddAmount(PayBillReq.BODY.tagAmountPay.amountToPay, PayBillReq.BODY.tagAmountPay.amountCommis), // На молотилку уходит сумма с комиссией
      millReturnGuard,

      PayBillResp.HDR.uniKioskStatus,
      MillOperationId,
      MillAcceptTime,
      MillAcceptStatus
      );

#else // USE_OLD_MILL_REQUEST

    std::string				IdTPPFlashPay;
    // Проверяємо торгівельну точку Партнера на наявність у alltradepoints
    Result = DATABASE->GetFpTppId(
      IdBuyer,
      PayBillReq.END.idTerminal,

      PayBillResp.HDR.uniKioskStatus,
      IdTPPFlashPay
    );
    CHECK_ERROR_RETURN(Result);

    Result = AcceptMoneyMill (
      PayBillResp.BODY.tagTransaction.idComBill,
      IdBuyer,
      PayBillReq.HDR.idService,
      PayBillReq.BODY.tagAgentCode.extReqNo,
      amountToPay,  /// PayBillReq.BODY.tagAmountPay.amountToPay, 
      amountCommis, /// PayBillReq.BODY.tagAmountPay.amountCommis,
      IdTPPFlashPay, // PayBillReq.END.idTerminal, <- змінено по задачі https://redmine.bf.com.ua/issues/106705
      millReturnGuard,

      PayBillResp.HDR.uniKioskStatus,
      MillOperationId,
      MillAcceptTime,
      MillAcceptStatus
      );

    PayBillResp.END.msgDateTime = GetCurrentDateTime();

#endif // USE_OLD_MILL_REQUEST

    if (Result != OK && PayBillResp.HDR.uniKioskStatus == uksNoMoney) 
    {
      int   execResult;

      execResult = DATABASE->SetTransactionsCommStatus ( PayBillResp.BODY.tagTransaction.idComBill, tsNoMoney);
      CHECK_ERROR_RETURN(execResult);

      return Result;

    } else if (Result != OK) {
      return Result;
    }

    if (uniBillStatus == ubsPayStarted) {
      isUpdateMillAcceptTime = true;
    } else {
      isUpdateMillAcceptTime = false;
    }

    // Отмечаем в бд результат акцептации ваучера
    Result = DATABASE->AcceptMillIntegrator (	
      amountToPay,  /// PayBillReq.BODY.tagAmountPay.amountToPay,
      amountCommis, /// PayBillReq.BODY.tagAmountPay.amountCommis,
      IdBuyer,
      PayBillReq.BODY.tagAgentCode.extReqNo,
      MillOperationId,
      MillAcceptStatus,
      MsgError
    );
    CHECK_ERROR_RETURN(Result);

    Result = TagERRHandler(PayBillReq.HDR.idService, IdBuyer, MsgError, PayBillResp.BODY.tagErrInfo, PayBillResp.HDR.billTags);
    CHECK_ERROR_RETURN(Result);

    //if (MsgError[0] == '*')
    //{
    //  Result = prepareTags(
    //    PayBillReq.HDR.idService,
    //    IdBuyer,
    //    MsgError,
    //    PayBillResp.BODY.tagErrInfo,
    //    PayBillResp.HDR.billTags
    //  );
    //
    //  if (ExistsTag(PayBillResp.HDR.billTags, TAG_ERR_INFO)) {
    //    return Result;
    //  }
    //}

    uniBillStatus = ubsDangCertificateAccepted;
  } // if ( (uniBillStatus == ubsPayStarted) || (uniBillStatus == ubsDangCertificateAccepted) ) 
  // -----------------------------------------------------------------------------------
  // Робимо запит на перевірку, чи потрібно надсилати смс на 
  // -----------------------------------------------------------------------------------
  // bool isNeedM85Send = false; 
  int SendAuthType = 0;
  std::string SmsAmnt = "0", SmsText = "Code: ";

  Result = DATABASE->IntegProcess113Stage2Add(
    idComBill,
    PayBillReq.HDR.idService,
    IdBuyer,
    amountToPay,
    PayBillReq.END.idTerminal,
    PayBillReq.HDR.idClient,

    PayBillResp.HDR.uniKioskStatus,
    locUniBillStatus,                 // uniBillStatus
    MsgError, 
    SendAuthType,
    SmsAmnt,
    SmsText
  );

  CHECK_ERROR_RETURN(Result);

  Result = TagERRHandler(PayBillReq.HDR.idService, IdBuyer, MsgError, PayBillResp.BODY.tagErrInfo, PayBillResp.HDR.billTags);
  CHECK_ERROR_RETURN(Result);

  // -----------------------------------------------------------------------------------
  // если результат OUT_result=51 процедуры IntegProcess113Stage2Add_v1(), то идет вызов функции отправки смс.В функцию передается идкомбилл., номер телефону або код
  // -----------------------------------------------------------------------------------
  if (PayBillResp.HDR.uniKioskStatus == uksPayCanceled)
  {
/*
    MillAcceptStatus = masReturned;
    
    // Подтверждаем в базе данных что счет оплачен
    Result = DATABASE->IntegProcess113Stage2(
      idComBill,
      idMillRq,
      idTransaction,
      PayBillReq.BODY.tagAgentCode.extReqNo,
      PayBillReq.HDR.idService,
      PayBillReq.HDR.idSubService,
      PayBillReq.HDR.idClient,
      PayBillReq.BODY.tagMsg.msg2Client,
      amountToPay,  /// PayBillReq.BODY.tagAmountPay.amountToPay,
      amountCommis, /// PayBillReq.BODY.tagAmountPay.amountCommis,
      providerIdCompany,
      PayBillReq.END.idTerminal,
      PayBillReq.END.keyValue,
      PayBillProviderResp.HDR.payDateTime, /// PayBillResp.END.msgDateTime = GetCurrentDateTime();
      MillOperationId,
      MillAcceptTime,
      MillAcceptStatus,
      providerAcceptStatus,
      providerExtNo,
      isUpdateMillAcceptTime,
      PayBillProviderResp.HDR.uniKioskStatus,

      PayBillResp.HDR.uniKioskStatus,
      PayBillResp.END.msgDateTime,
      isReturnVoucher,
      isSendToOvertaking,
      MsgError
    );

    PayBillResp.END.msgDateTime = GetCurrentDateTime();

    Result = TagERRHandler(PayBillReq.HDR.idService, IdBuyer, MsgError, PayBillResp.BODY.tagErrInfo, PayBillResp.HDR.billTags);
    CHECK_ERROR_RETURN(Result);

    isReturnVoucher = true;

    // Если надо возвращать ваучер
    if (isReturnVoucher) {
      millReturnGuard->addCertificate(
        PayBillReq.HDR.idService,
        IdBuyer,
        PayBillReq.BODY.tagAgentCode.extReqNo,
        PayBillResp.BODY.tagTransaction.idComBill,
        amountToPay /// PayBillReq.BODY.tagAmountPay.amountToPay
      );
    }
*/
    return Result;
  }
  // -----------------------------------------------------------------------------------
  // если результат OUT_result=51 процедуры IntegProcess113Stage2Add_v1(), то идет вызов функции отправки смс.В функцию передается идкомбилл., номер телефону або код
  // -----------------------------------------------------------------------------------
  if (PayBillResp.HDR.uniKioskStatus == uksReturnMillSuccessEarly)
  {
    Result = SendSMSHandler(idComBill, TelNumber, ClientCode, SendAuthType, SmsAmnt, SmsText, PayBillResp.HDR.uniKioskStatus);
    
    return Result;
/*
    std::string locTelNumber, locClientCode;
    // 1 - записано телефон для відправки СМС (це початковий статус при створенні запису). В разі повторних запитів статус не змінюється.
    // 2 - записан код для відправки в смс.
    // 3 - смс відправлена.
    // 9 - відміна. код від партнера не приймати,
    // 
    // 
    //10 - отриман код в операції. Все Ок, співпало. Можна виконувати операці. у постачальника.
    // Зробити вибірку select code_send, phone from SMS_CONTROL where idcombill= <idcombill> and mstatus=2 ;
    // 2 - записан код для відправки в смс.
    Result = DATABASE->GetSmsParamsByStatus(idComBill, 2, locClientCode, locTelNumber);
    
    if (Result != OK)
    {
      PayBillResp.HDR.uniKioskStatus = uksServerBusy; // 23

      Inform(MSML
        << "Помилка вибірки з табл. SMS_CONTROL при отриманні коду для смс для: IdBuyer = " << IdBuyer
        << ",  extReqNo = " << PayBillReq.BODY.tagAgentCode.extReqNo << ", idComBill = " << idComBill << ", TelNumber = " << TelNumber << ". ", ClassID, mtError, Result);

      return Result;
    }

    if ((!locTelNumber.empty()) && (!locClientCode.empty()))
    {
      Result = SendSMS(idComBill, locTelNumber, locClientCode);

      if (Result != OK)
      {
        PayBillResp.HDR.uniKioskStatus = uksServerBusy; // 23

        Inform(MSML << "Помилка при надсиланні смс клієнтові на номер TelNumber = " << locTelNumber << ",  ClientCode = " << locClientCode << ". ", ClassID, mtError, Result);

        return Result;
      }
    }
    else
    {
      PayBillResp.HDR.uniKioskStatus = uksServerBusy; // 23

      Inform(MSML << "Помилка в даних TelNumber/ClientCode: " << locTelNumber << "/" << locClientCode << ". ", ClassID, mtError, Result);

      return Result;
    }
    // Змінили статус у locClientCode
    // 3 - смс відправлена.
    Result = DATABASE->UpdateSmsParamsByStatus(idComBill, 3);
    
    if (Result != OK)
    {
      PayBillResp.HDR.uniKioskStatus = uksServerBusy; // 23

      Inform(MSML << "Виникла помилка при оновленні табл. SMS_CONTROL: idComBill = " << idComBill << ", mStatus = 3. ", ClassID, mtError, Result);

      return Result;
    }

    PayBillResp.HDR.uniKioskStatus = uksReturnMillSuccessEarly; // 51

    return Result;
*/
  }
  // -----------------------------------------------------------------------------------
  // uniKioskStatus = 52
  // -----------------------------------------------------------------------------------
  if (PayBillResp.HDR.uniKioskStatus == uksReturnMillForbidden)
  {
    return Result;
  }
  // -----------------------------------------------------------------------------------
  // uniKioskStatus = 53
  // -----------------------------------------------------------------------------------
  if (PayBillResp.HDR.uniKioskStatus == uksM85NotSent)
  {
    return Result;
  }
  //if (( PayBillResp.HDR.uniKioskStatus == uksM85NotSent) && (isNeedM85Send == true))
  //{
  //  Result = SendM85Handler(idComBill, TelNumber, ClientCode, SmsAmnt);

  //  if (Result != OK)
  //  {
  //    // PayBillResp.HDR.uniKioskStatus
  //    PayBillResp.HDR.uniKioskStatus = uksRepeatWithTheSameCertificate; // 21

  //    return Result;
  //  }

  //  PayBillResp.HDR.uniKioskStatus = uksReturnMillSuccessEarly; // 51

  //  return Result;
  //}
  // -----------------------------------------------------------------------------------
	// Ставим статус по умолчанию повторить с тем же ваучером
  // -----------------------------------------------------------------------------------
	PayBillResp.HDR.uniKioskStatus = uksServerBusy;
  // -----------------------------------------------------------------------------------
	// Формируем запрос к предметному серверу
  // -----------------------------------------------------------------------------------
  if ( (uniBillStatus == ubsDangCertificateAccepted) 
    || (uniBillStatus == ubsBillPayed && isAlwaysSendToProvider)
    ) {

    PayBillProviderReq.HDR.idService    = PayBillReq.HDR.idService;
    PayBillProviderReq.HDR.idSubService = PayBillReq.HDR.idSubService;
    PayBillProviderReq.HDR.idClient     = PayBillReq.HDR.idClient;
    PayBillProviderReq.HDR.payDateTime  = GetCurrentDateTime();
    PayBillProviderReq.HDR.idTerminal   = PayBillReq.END.idTerminal;
    PayBillProviderReq.HDR.idBuyer      = IdBuyer;

    // Добавляем необходимые таги в запрос
    AddTag(PayBillProviderReq.HDR.billTags, TAG_TRADE_SRV);
    PayBillProviderReq.BODY.tagTradeSrv.idComBill = idComBill;

    if (uniBillStatus == ubsBillPayed && isAlwaysSendToProvider) {
      PayBillProviderReq.BODY.tagTradeSrv.repeat = "1"; 
    } else {
      PayBillProviderReq.BODY.tagTradeSrv.repeat = "0"; 
    }

    // Виадаляємо таги AWF з запиту до Провайдера
    if (ExistsTag(PayBillProviderReq.HDR.billTags, TAG_ACC_OWNER_PAY))
    {
      RemoveTag(PayBillProviderReq.HDR.billTags, TAG_ACC_OWNER_PAY);
    }

    // Виадаляємо таги AWF з запиту до Провайдера
    if (ExistsTag(PayBillProviderReq.HDR.billTags, TAG_ACC_OWNER_FACT))
    {
      RemoveTag(PayBillProviderReq.HDR.billTags, TAG_ACC_OWNER_FACT);
    }

    // Заполняем таг криптографии
    if (ExistsTag(PayBillReq.HDR.billTags, TAG_CRYPTO) ) {
      eKeyType		 keyType;

      RemoveTag(PayBillProviderReq.HDR.billTags, TAG_CRYPTO);

      Result = DATABASE->GetKeyType (	
        PayBillReq.END.idTerminal,
			  PayBillReq.END.keyValue,

			  keyType
			  );

      CHECK_ERROR_RETURN(Result);

      PayBillProviderReq.BODY.tagCryptoInternal.msgCrypto = PayBillReq.BODY.tagCrypto.msgCrypto;
      PayBillProviderReq.BODY.tagCryptoInternal.idkey = PayBillReq.END.keyValue;
      PayBillProviderReq.BODY.tagCryptoInternal.keytype = IntToString(keyType);

      AddTag(PayBillProviderReq.HDR.billTags, TAG_CRYPTO_INTERNAL);
    }

    // Якщо у запиті 113 присутній таг AL, то до Прикладного сервера формуємо з даних AL таг AP
    if (ExistsTag(PayBillReq.HDR.billTags, TAG_AMOUNT_CASHLESS) == true )
    {
      if (ExistsTag(PayBillProviderReq.HDR.billTags, TAG_AMOUNT_CASHLESS) == true )
      {
         RemoveTag(PayBillProviderReq.HDR.billTags, TAG_AMOUNT_CASHLESS);
      }

      AddTag(PayBillProviderReq.HDR.billTags, TAG_AMOUNT_PAY);
      PayBillProviderReq.BODY.tagAmountPay.amountToPay  = PayBillReq.BODY.tagAmountCashless.AmountToPay;
      PayBillProviderReq.BODY.tagAmountPay.amountCommis = PayBillReq.BODY.tagAmountCashless.AmountCommis;
    }


  	// Ставим статус по умолчанию повторить с тем же ваучером
    PayBillProviderResp.HDR.uniKioskStatus = uksServerBusy;

    // Отправляем запрос на сервер на оплату счета
    Result = PayBillProvider ();

    // Перезаписываем поля, которые не меняются
//     AddTag(PayBillResp.HDR.billTags, TAG_AGENT_CODE);
//     PayBillResp.BODY.tagAgentCode = PayBillReq.BODY.tagAgentCode;
//     
//     AddTag(PayBillResp.HDR.billTags, TAG_TRANSACTION);
//     PayBillResp.BODY.tagTransaction.idComBill = idComBill;

//     AddTag(PayBillResp.HDR.billTags, TAG_CHECK1);
//     PayBillResp.BODY.tagCheque1.chequeText = checkText;

    PayBillResp.END.msgDateTime = GetCurrentDateTime();

    if (Result == OK) {
      providerAcceptStatus = pasOK;

      // Копируем таги для запроса по предметному серверу
      PayBillResp.HDR.billTags = PayBillProviderResp.HDR.billTags;
      PayBillResp.BODY = PayBillProviderResp.BODY;

      // Удаляем таги, которые не должны идти от интегрированного сервера
      RemoveTag (PayBillResp.HDR.billTags, TAG_PROVIDER);
      RemoveTag (PayBillResp.HDR.billTags, TAG_ID_COMP);
      RemoveTag (PayBillResp.HDR.billTags, TAG_ID_MERCHANT);

      // Добавляем выходные обязательные параметры
      AddTag(PayBillResp.HDR.billTags, TAG_AGENT_CODE);
      PayBillResp.BODY.tagAgentCode.extReqNo = PayBillReq.BODY.tagAgentCode.extReqNo;

      AddTag(PayBillResp.HDR.billTags, TAG_TRANSACTION);
      PayBillResp.BODY.tagTransaction.idComBill = idComBill;

      // Копируем время выполнения на предметном сервере
      /// PayBillResp.END.msgDateTime = PayBillProviderResp.HDR.payDateTime;
      PayBillResp.END.msgDateTime = GetCurrentDateTime();
    } else {
      providerAcceptStatus = pasError;

      // Если получили успешный статус но не смогли до конца распарсить ответ
      if (PayBillProviderResp.HDR.uniKioskStatus == uksBillPayed 
        || PayBillProviderResp.HDR.uniKioskStatus == uksBillPayedEarly) {

          isReturnVoucher = false;
          PayBillProviderResp.HDR.uniKioskStatus = uksServerBusy;
      }
    }

    // Если оплата по провайдеру успешна
    if (IsOkUniKioskStatus(PayBillProviderResp.HDR.uniKioskStatus)) {

      Result = DATABASE->GetServiceTagsPay_v2 (
        PayBillReq.HDR.idService,
        IdBuyer,

        billTagsForPay
      );

      CHECK_ERROR_RETURN(Result);

      // Добавляем Таги
      Result = addIntegratorRequestTags (
        billTagsForPay,
        PayBillReq.HDR,
        PayBillReq.BODY,
        PayBillProviderResp.HDR,
        PayBillProviderResp.BODY,

        PayBillResp.BODY,
        PayBillResp.HDR.billTags,
        PayBillProviderResp.HDR.uniKioskStatus
        );

      PayBillResp.END.msgDateTime = GetCurrentDateTime();
      // Если не смогли смогли сформировать ответные таги выставляем статус повторить транзакцию позже
      if (Result != OK) {
        PayBillProviderResp.HDR.uniKioskStatus = uksServerBusy;
        PayBillResp.HDR.uniKioskStatus = uksServerBusy;
        providerAcceptStatus = pasError;
      }
  
    } else {
      providerAcceptStatus = pasError;

      PayBillResp.HDR.uniKioskStatus = PayBillProviderResp.HDR.uniKioskStatus;
    }
  }

	// Если счет уже был успешно оплачен
  if (uniBillStatus == ubsBillPayed) {
    providerAcceptStatus = pasOK;
  }

  // Если получили номер оплаты по провайдеру
  if ( ExistsTag(PayBillProviderResp.HDR.billTags, TAG_PROVIDER)
    && (PayBillProviderResp.BODY.tagProvider.begin() != PayBillProviderResp.BODY.tagProvider.end()) 
    ){
    providerExtNo = PayBillProviderResp.BODY.tagProvider.begin()->second.providerExtNo;

    if (providerExtNo == "") {
      providerExtNo = "-1";
    }

  } else {
    providerExtNo = "-1";
  }

  // Определяем код компании
  if (PayBillProviderResp.BODY.tagIdComp.size() > 0) 
  {
    providerIdCompany = PayBillProviderResp.BODY.tagIdComp.begin()->second.idCompany;
  } 
  else if (PayBillProviderResp.BODY.tagIdMerchant.size() > 0) 
  {
    providerIdCompany = PayBillProviderResp.BODY.tagIdMerchant.begin()->second.idCompany;
  }

  // Подтверждаем в базе данных что счет оплачен
  Result = DATABASE->IntegProcess113Stage2 (	
    idComBill,
    idMillRq,
    idTransaction,
    PayBillReq.BODY.tagAgentCode.extReqNo,
    PayBillReq.HDR.idService,
    PayBillReq.HDR.idSubService,
    PayBillReq.HDR.idClient,
    PayBillReq.BODY.tagMsg.msg2Client,
    amountToPay,  /// PayBillReq.BODY.tagAmountPay.amountToPay,
    amountCommis, /// PayBillReq.BODY.tagAmountPay.amountCommis,
    providerIdCompany,
    PayBillReq.END.idTerminal,
    PayBillReq.END.keyValue,
    PayBillProviderResp.HDR.payDateTime, /// PayBillResp.END.msgDateTime = GetCurrentDateTime();
    MillOperationId,
    MillAcceptTime,
    MillAcceptStatus,
    providerAcceptStatus,
    providerExtNo,
    isUpdateMillAcceptTime,
    PayBillProviderResp.HDR.uniKioskStatus,

    PayBillResp.HDR.uniKioskStatus,
    PayBillResp.END.msgDateTime,
    isReturnVoucher,
    isSendToOvertaking,
    MsgError
  );

  PayBillResp.END.msgDateTime = GetCurrentDateTime();

  Result = TagERRHandler(PayBillReq.HDR.idService, IdBuyer, MsgError, PayBillResp.BODY.tagErrInfo, PayBillResp.HDR.billTags);
  CHECK_ERROR_RETURN(Result);

  //if (MsgError[0] == '*')
  //{
  //  Result = prepareTags(
  //    PayBillReq.HDR.idService,
  //    IdBuyer,
  //    MsgError,
  //    PayBillResp.BODY.tagErrInfo,
  //    PayBillResp.HDR.billTags
  //  );
  //
  //  if (ExistsTag(PayBillResp.HDR.billTags, TAG_ERR_INFO)) {
  //    return Result;
  //  }
  //}

  /// Робимо ініціаліацію тага DO
  if (IsOkUniKioskStatus(PayBillResp.HDR.uniKioskStatus) || IsNotOkFinalUniKioskStatus (PayBillResp.HDR.uniKioskStatus) )
  {
    if ( ExistsTag(PayBillResp.HDR.billTags, TAG_DATE_OP) == true )
    {
      TPosDateTime locDateOp;
      /// Вичитуємо з таблиці transactionscomm DateOp
      Result = DATABASE->GetDateOp ( idComBill, locDateOp ); /// CHECK_ERROR_RETURN(Result);

      if ( Result != OK )
      {
        Result = OK;
        RemoveTag (PayBillResp.HDR.billTags, TAG_DATE_OP);
      } /// if ( Result != OK )
      else 
      {
        PayBillResp.BODY.tagDateOp.DateOperation = TPosDateTime_To_DateTimeString(locDateOp);
      } /// else
    } /// if ( ExistsTag(PayBillResp.HDR.billTags, TAG_DATE_OP) == true )
  } /// if (IsOkUniKioskStatus(PayBillProviderResp.HDR.uniKioskStatus))
 
  // Если надо возвращать ваучер
  if (isReturnVoucher) {
    millReturnGuard->addCertificate (
      PayBillReq.HDR.idService,
      IdBuyer,
      PayBillReq.BODY.tagAgentCode.extReqNo,
      PayBillResp.BODY.tagTransaction.idComBill,
      amountToPay /// PayBillReq.BODY.tagAmountPay.amountToPay
      );
  }

  // Если запрос был отправлен на догонку добавляем таги для успешной оплаты
  if (PayBillResp.HDR.uniKioskStatus == uksBillAcceptedWithoutMill 
    || PayBillResp.HDR.uniKioskStatus == uksBillAcceptedWithoutProvider) {

    int execResult;
    execResult = DATABASE->GetServiceTagsPay_v2 (
      PayBillReq.HDR.idService,
      IdBuyer,

      billTagsForPay
    );

    CHECK_ERROR_RETURN(execResult);

    // Добавляем Таги
    execResult = addIntegratorRequestTags (
      billTagsForPay,
      PayBillReq.HDR,
      PayBillReq.BODY,
      PayBillProviderResp.HDR,
      PayBillProviderResp.BODY,

      PayBillResp.BODY,
      PayBillResp.HDR.billTags,
      PayBillResp.HDR.uniKioskStatus
      );

    PayBillResp.END.msgDateTime = GetCurrentDateTime();
    // Если не смогли смогли сформировать ответные таги выставляем статус повторить транзакцию позже
    if (execResult != OK) {
      PayBillResp.HDR.uniKioskStatus = uksServerBusy;
    }
  
  }

  return Result;
}

//----------------------------------------------------------------------
// Обработать полученный запрос статуса транзакции
// Вход
//		
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::AcceptServiceOperStatusReq (void)
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::AcceptServiceOperStatusReq : ";
  int											    Result = OK;

  eTransactionStatus transactionStatus = tsUnknown;
  eMillAcceptStatus  millRqStatus;
  std::string				 amountPayed;
  std::string				 repeatMode;
  std::string				 stReturn;
  TPosDateTime       payDateTime;
  TPosDateTime       fulFilledDateTime;
  std::string				 outIdComBill;
  std::string				 outIdService;
  std::string				 outIdSubService;
  std::string				 outIdClient;
  std::string				 outAmount;
  std::string				 outAmountCommis;
  TPosDateTime       outDateOperation;
  std::string				 outIdCompany;
  TStdStringList	   billTagsForPay;
  bool               isGotTagIp = false;
  S_tagInfoPay       tagInfoPay;
  std::string		     MsgError;

	// Ставим статус по умолчанию ошибка сервера
	ServiceOperStatusResp.HDR.uniKioskStatus = uksServerError;

  // Читаем интегрированные настройки для полученного сервиса
  Result = DATABASE->GetIntegServiceConfig (
    ServiceOperStatusReq.HDR.idService,

    conf
    );

  ServiceOperStatusResp.END.msgDateTime = GetCurrentDateTime();

  CHECK_ERROR_RETURN(Result);
  
  // Заголовок телеграмм ответа
// struct S_INTEG_HEADER_RESP {
//   eUniKioskClientRequest idMsgType;		    // Идентификатор сообщения
//   eUniKioskStatus			uniKioskStatus;   // Статус универсального киоска
//   std::string					idService;        // Номер сервиса
//   std::string					idSubService;     // Код подуслуги
//   std::string					idClient;         // Номер счета клиента
//   TStdStringList      billTags;         // Таги , переданные в сообщении, Если нет, то «*»
// };

  // Формируем параметры по умолчанию для ответа
  ServiceOperStatusResp.HDR.idService    = ServiceOperStatusReq.HDR.idService;
  ServiceOperStatusResp.HDR.idSubService = ServiceOperStatusReq.HDR.idSubService;
  ServiceOperStatusResp.HDR.idClient     = ServiceOperStatusReq.HDR.idClient;

  // Вычитываем номер байера по его ключу
	Result = DATABASE->GetIdBuyerByKey (	
    ServiceOperStatusReq.END.keyValue,

    ServiceOperStatusProviderReq.HDR.idBuyer
		);

  ServiceOperStatusResp.END.msgDateTime = GetCurrentDateTime();

  CHECK_ERROR_RETURN(Result);

  IdBuyer = ServiceOperStatusProviderReq.HDR.idBuyer;

  // Копируем таги запроса
  // ServiceOperStatusProviderReq.BODY = ServiceOperStatusReq.BODY;
  // ServiceOperStatusProviderReq.HDR.billTags = ServiceOperStatusReq.HDR.billTags;
  // 
  // Якщо одночасно отримали два тага UA та IP
  if (ExistsTag(ServiceOperStatusReq.HDR.billTags, TAG_INFO_PAY) == true)
  {
    if (ExistsTag(ServiceOperStatusReq.HDR.billTags, TAG_AGENT_CODE) == true)
    {
      // Перевіряємо, що UnicAgentCode та extReqNo не порожні або не *
      if ( ( (ServiceOperStatusReq.BODY.tagInfoPay.UnicAgentCode == "*") || (ServiceOperStatusReq.BODY.tagInfoPay.UnicAgentCode.empty() )  ) && 
           ( (ServiceOperStatusReq.BODY.tagAgentCode.extReqNo    == "*") || (ServiceOperStatusReq.BODY.tagAgentCode.extReqNo.empty()    )  ) )
      {
        ServiceOperStatusResp.HDR.uniKioskStatus = uksCannotParseRequest; // 99 — нарушена структура сообщения

        Result = ERROR_TRANSACTION_ABSENT;

        Inform(MSML << "tagIP.UnicAgentCode та tagUA.extReqNo одночасно порошні або мають значення * (зірочка). ", ClassID, mtWarning, Result);

        return Result;
      }
      
      if ( (ServiceOperStatusReq.BODY.tagInfoPay.UnicAgentCode != ServiceOperStatusReq.BODY.tagAgentCode.extReqNo) )
      {
        // extReqNo = *, UnicAgentCode != *, тоді extReqNo = UnicAgentCode + billTags - таг UA
        if (  ((ServiceOperStatusReq.BODY.tagAgentCode.extReqNo == "*") || (ServiceOperStatusReq.BODY.tagAgentCode.extReqNo.empty())) && 
              (!ServiceOperStatusReq.BODY.tagInfoPay.UnicAgentCode.empty()))
        {
          ServiceOperStatusReq.BODY.tagAgentCode.extReqNo = ServiceOperStatusReq.BODY.tagInfoPay.UnicAgentCode;
          // RemoveTag(ServiceOperStatusReq.HDR.billTags, TAG_AGENT_CODE);
        }
        else if (((ServiceOperStatusReq.BODY.tagInfoPay.UnicAgentCode == "*") || (ServiceOperStatusReq.BODY.tagInfoPay.UnicAgentCode.empty())) &&
          (!ServiceOperStatusReq.BODY.tagAgentCode.extReqNo.empty()))
        {
          ServiceOperStatusReq.BODY.tagInfoPay.UnicAgentCode = ServiceOperStatusReq.BODY.tagAgentCode.extReqNo;
          // RemoveTag(ServiceOperStatusReq.HDR.billTags, TAG_AGENT_CODE);
        }
        else {
          ServiceOperStatusResp.HDR.uniKioskStatus = uksCannotParseRequest; // 99 — нарушена структура сообщения

          Result = ERROR_TRANSACTION_ABSENT;

          Inform(MSML << "tagIP.UnicAgentCode != tagUA.extReqNo. Різна інформація у тагах IP та UA по Коду Агента. ", ClassID, mtWarning, Result);

          return Result;
        }      
        RemoveTag(ServiceOperStatusReq.HDR.billTags, TAG_AGENT_CODE);
      }     
    }
    // 
    if ( ((ServiceOperStatusReq.BODY.tagInfoPay.UnicAgentCode.empty()) || (ServiceOperStatusReq.BODY.tagInfoPay.UnicAgentCode == "*")) &&
         (!ExistsTag(ServiceOperStatusReq.HDR.billTags, TAG_AGENT_CODE))
       )
    {
      ServiceOperStatusResp.HDR.uniKioskStatus = uksCannotParseRequest; // 99 — нарушена структура сообщения

      Result = ERROR_TRANSACTION_ABSENT;

      Inform(MSML << "Данi поля tagIP.UnicAgentCode = {EMPTY,*} i таг UA вiдсутнiй. ", ClassID, mtWarning, Result);

      return Result;
    }

    ServiceOperStatusReq.BODY.tagAgentCode.extReqNo = ServiceOperStatusReq.BODY.tagInfoPay.UnicAgentCode;
  }

  // Вычитываем данные необходимые для сторнирования на предметном сервере
  Result = DATABASE->GetIntegTransactionData (	
    ServiceOperStatusProviderReq.HDR.idBuyer,
    ServiceOperStatusReq.BODY.tagAgentCode.extReqNo,

    idComBill,
    transactionStatus,
    millRqStatus,
    amountPayed,
    repeatMode,
    stReturn,
    payDateTime,
    fulFilledDateTime,
    MsgError
  );

  ServiceOperStatusResp.END.msgDateTime = GetCurrentDateTime();

  CHECK_ERROR_RETURN(Result);

  Result = TagERRHandler(ServiceOperStatusProviderReq.HDR.idService, IdBuyer, MsgError, ServiceOperStatusResp.BODY.tagErrInfo, ServiceOperStatusResp.HDR.billTags);
  CHECK_ERROR_RETURN(Result);

  // Если транзакция отстутсвует сразу отвечаем
  if (idComBill == "-1") {
    ServiceOperStatusResp.HDR.uniKioskStatus = uksTransactionAbsent;

    Result = ERROR_TRANSACTION_ABSENT;

    Inform(MSML 
      << "Никуда не идем - транзакция отсутствует в бд."
      ,ClassID, mtWarning, Result);

    return Result;
  }

  // Обновляем мониторинг
  Result = SetMonitoring_idComBill(idComBill);
  CHECK_ERROR_RETURN(Result);

  // Виконуємо ініціалізацію тага IP
  if (ExistsTag(ServiceOperStatusReq.HDR.billTags, TAG_INFO_PAY) == true)
  {
    isGotTagIp = true;
    tagInfoPay.IdTransaction = (((idComBill.empty())||(idComBill == "-1")) ? "*" : idComBill);
    tagInfoPay.UnicAgentCode = (ServiceOperStatusReq.BODY.tagAgentCode.extReqNo.empty()) ? "*" : ServiceOperStatusReq.BODY.tagAgentCode.extReqNo;
    tagInfoPay.AddInfo       = "*";
    tagInfoPay.IpDataList.clear();

    S_IP_DATA locIpData;
    locIpData.infoType = "0";
    locIpData.infoValue = IntToString((int)transactionStatus);

    tagInfoPay.IpDataList["0"] = locIpData;
    tagInfoPay.tagValue = IntToString(tagInfoPay.IpDataList.size());
    //
    ServiceOperStatusResp.BODY.tagInfoPay = tagInfoPay;
    AddTag(ServiceOperStatusResp.HDR.billTags, TAG_INFO_PAY);    
  }

  // Если не надо отсылать на предметный сервер вызываем процедуру статуса транзакции
  if (repeatMode == "0") {

    Result = DATABASE->IntegratorGetOperationState (	
      idComBill,

      ServiceOperStatusResp.HDR.uniKioskStatus,
      outIdComBill,
      outIdService,
      outIdSubService,
      outIdClient,
      outAmount,
      outAmountCommis,
      outDateOperation,
      outIdCompany,
      MsgError
    );

    ServiceOperStatusResp.END.msgDateTime = GetCurrentDateTime();

    CHECK_ERROR_RETURN(Result);

    Result = TagERRHandler(ServiceOperStatusProviderReq.HDR.idService, IdBuyer, MsgError, ServiceOperStatusResp.BODY.tagErrInfo, ServiceOperStatusResp.HDR.billTags);
    CHECK_ERROR_RETURN(Result);

//     ServiceOperStatusResp.BODY = ServiceOperStatusProviderResp.BODY;
//     ServiceOperStatusResp.HDR.billTags = ServiceOperStatusProviderResp.HDR.billTags;

    // Если тразакция успешна добавляем чеки
    if (ServiceOperStatusResp.HDR.uniKioskStatus == uksOK) {
    
      /// Result = DATABASE->GetServiceTagsPay (
      Result = DATABASE->GetServiceTagsPay_v2 (
        ServiceOperStatusReq.HDR.idService,
        ServiceOperStatusProviderReq.HDR.idBuyer,

        billTagsForPay
      );

      ServiceOperStatusResp.END.msgDateTime = GetCurrentDateTime();

      CHECK_ERROR_RETURN(Result);

      IdBuyer = ServiceOperStatusProviderReq.HDR.idBuyer;

      // Добавляем код компании если выставлен признак брать чек из таблицы uniservice2company
      if (conf.useIdCompanyCheque) {
//       if (outIdCompany != "0") {

        S_tagIdCompData tagIdCompData;
        tagIdCompData.idCompany = outIdCompany;

        ServiceOperStatusProviderResp.BODY.tagIdComp[0] = tagIdCompData;
        AddTag (ServiceOperStatusProviderResp.HDR.billTags, TAG_ID_COMP);
      }

      Result = addIntegratorRequestTags (
        billTagsForPay,
        ServiceOperStatusReq.HDR,
        ServiceOperStatusReq.BODY,
        ServiceOperStatusProviderResp.HDR,
        ServiceOperStatusProviderResp.BODY,

        ServiceOperStatusResp.BODY,
        ServiceOperStatusResp.HDR.billTags,
        ServiceOperStatusResp.HDR.uniKioskStatus
        );

      ServiceOperStatusResp.END.msgDateTime = GetCurrentDateTime();

      CHECK_ERROR_RETURN(Result);

      // Добавляем таги
      ServiceOperStatusResp.BODY.tagTransaction.idComBill = idComBill;
      AddTag (ServiceOperStatusResp.HDR.billTags, TAG_TRANSACTION);

      if (isGotTagIp == true)
      {
        // Фінальна ініціалізація тагу IP 
        Result = DATABASE->ag_GetTagIPValues(
          tagInfoPay.IdTransaction,
          tagInfoPay.UnicAgentCode,
          ServiceOperStatusProviderReq.HDR.idBuyer,
          ServiceOperStatusReq.HDR.idService,

          tagInfoPay.IpDataList
        );

        ServiceOperStatusResp.END.msgDateTime = GetCurrentDateTime();

        CHECK_ERROR_RETURN(Result);

        // ServiceOperStatusResp.HDR.billTags.clear();

        tagInfoPay.tagValue = IntToString(tagInfoPay.IpDataList.size());
        ServiceOperStatusResp.BODY.tagInfoPay = tagInfoPay;
        AddTag(ServiceOperStatusResp.HDR.billTags, TAG_INFO_PAY);
      }      
    }

    // Добавляем обязательные таги
    ServiceOperStatusResp.BODY.tagAgentCode.extReqNo = ServiceOperStatusReq.BODY.tagAgentCode.extReqNo;
    AddTag (ServiceOperStatusResp.HDR.billTags, TAG_AGENT_CODE);

    // Добавляем обязательный таг DO
    if ( ExistsTag(ServiceOperStatusResp.HDR.billTags, TAG_DATE_OP) == true )
    {
      TPosDateTime locDateOp;
      /// Вичитуємо з таблиці transactionscomm DateOp
      Result = DATABASE->GetDateOp ( outIdComBill, locDateOp ); /// CHECK_ERROR_RETURN(Result);

      if ( Result != OK )
      {
        Result = OK;
        RemoveTag (ServiceOperStatusResp.HDR.billTags, TAG_DATE_OP);
      } /// if ( Result != OK )
      else 
      {
        ServiceOperStatusResp.BODY.tagDateOp.DateOperation = TPosDateTime_To_DateTimeString(locDateOp);
      } /// else
    } /// if ( ExistsTag(PayBillResp.HDR.billTags, TAG_DATE_OP) == true )

    if ( (isGotTagIp == true) && (ServiceOperStatusResp.HDR.uniKioskStatus == uksOK))
    {
      // ServiceOperStatusResp.HDR.billTags.clear();
      if (ExistsTag(ServiceOperStatusResp.HDR.billTags, TAG_AGENT_CODE) == true)
      {
        RemoveTag(ServiceOperStatusResp.HDR.billTags, TAG_AGENT_CODE);
        RemoveTag(ServiceOperStatusResp.HDR.billTags, TAG_TRANSACTION);
      }

      AddTag(ServiceOperStatusResp.HDR.billTags, TAG_INFO_PAY);
    }

    if ((isGotTagIp == true) && (ServiceOperStatusResp.HDR.uniKioskStatus != uksOK))
    {
      RemoveTag(ServiceOperStatusResp.HDR.billTags, TAG_AGENT_CODE);
      // RemoveTag(ServiceOperStatusResp.HDR.billTags, TAG_TRANSACTION);
      RemoveTag(ServiceOperStatusResp.HDR.billTags, TAG_INFO_PAY);
    }

    ServiceOperStatusResp.END.msgDateTime = GetCurrentDateTime();

    return Result;
  
  // Если необходимо отсылать на предметный сервер
  } else if (repeatMode == "1") {
    // && transactionStatus == tsServerBusy
    // && millRqStatus == masAccepted
    // ) {

    // Если стоит 101 статус транзакции то никуда не идем а сразу отвечаем что транзакция не акцептована
    /// if 
    if (
      (transactionStatus == tsTransactionCanceled) ||
      (transactionStatus == (eTransactionStatus)20)
      )
    {
      // ServiceOperStatusResp.HDR.uniKioskStatus = uksNothingToPay; /// tsRejected  = 2, // — транзакция не принята (операция не выполнена)
      // Before task https://redmine.bf.com.ua/issues/117030
      // ServiceOperStatusResp.HDR.uniKioskStatus = uksBillNotPrepared; /// tsInProcess = 1, // — в процессе выполнения
      // After task https://redmine.bf.com.ua/issues/117030
      ServiceOperStatusResp.HDR.uniKioskStatus = (eUniKioskStatus)tsRejected; // tsRejected  = 2 — транзакция не принята (операция не выполнена), task https://redmine.bf.com.ua/issues/117030
      ServiceOperStatusResp.END.msgDateTime    = GetCurrentDateTime();

      Result = ERROR_TRANSACTION_CANCELED;

      Inform (MSML << "Транзакція має статус 101 або 20. ", ClassID, mtError, Result);

      Result = OK;      

      return Result;
    }

    //   1. Сформировать запрос статуса транзакции для предметного сервера
    ServiceOperStatusProviderReq.HDR.idService    = ServiceOperStatusReq.HDR.idService;
    ServiceOperStatusProviderReq.HDR.idSubService = ServiceOperStatusReq.HDR.idSubService;
    ServiceOperStatusProviderReq.HDR.idClient     = ServiceOperStatusReq.HDR.idClient;
    ServiceOperStatusProviderReq.HDR.payDateTime  = ServiceOperStatusReq.END.msgDateTime;
    ServiceOperStatusProviderReq.HDR.idTerminal   = ServiceOperStatusReq.END.idTerminal;

    // Копируем таги в запрос предметному серверу
    ServiceOperStatusProviderReq.BODY         = ServiceOperStatusReq.BODY;
    ServiceOperStatusProviderReq.HDR.billTags = ServiceOperStatusReq.HDR.billTags;

    ServiceOperStatusProviderReq.BODY.tagTradeSrv.repeat    = "0";
    ServiceOperStatusProviderReq.BODY.tagTradeSrv.idComBill = idComBill;

    // Добавляем таги
    AddTag (ServiceOperStatusProviderReq.HDR.billTags, TAG_TRADE_SRV);

    // Удаляем лишние таги
    RemoveTag (ServiceOperStatusProviderReq.HDR.billTags, TAG_AGENT_CODE);
    RemoveTag (ServiceOperStatusProviderReq.HDR.billTags, TAG_TRANSACTION);
    RemoveTag (ServiceOperStatusProviderReq.HDR.billTags, TAG_INFO_PAY);
    
    /*
    // Заполняем таг криптографии
    if (ExistsTag(ServiceOperStatusProviderReq.HDR.billTags, TAG_CRYPTO) ) {
      eKeyType		 keyType;

      RemoveTag(ServiceOperStatusProviderReq.HDR.billTags, TAG_CRYPTO);

      Result = DATABASE->GetKeyType (	
        ServiceOperStatusReq.END.idTerminal,
			  ServiceOperStatusReq.END.keyValue,

			  keyType
			  );

      CHECK_ERROR_RETURN(Result);

      ServiceOperStatusProviderReq.BODY.tagCryptoInternal.msgCrypto = ServiceOperStatusReq.BODY.tagCrypto.msgCrypto;
      ServiceOperStatusProviderReq.BODY.tagCryptoInternal.idkey = ServiceOperStatusReq.END.keyValue;
      ServiceOperStatusProviderReq.BODY.tagCryptoInternal.keytype = IntToString(keyType);

      AddTag(ServiceOperStatusProviderReq.HDR.billTags, TAG_CRYPTO_INTERNAL);
    }
    */

    // Отослать запрос на предметный сервер
    Result = OperStatusProvider();

    // Проверить ответ от предметного сервера
    if (Result != OK) {
      ServiceOperStatusResp.HDR.uniKioskStatus = uksServerBusy;
      ServiceOperStatusResp.END.msgDateTime    = GetCurrentDateTime();

      return Result;
    } /// if (Result != OK)

    ServiceOperStatusResp.HDR.uniKioskStatus = ServiceOperStatusProviderResp.HDR.uniKioskStatus;
    ServiceOperStatusResp.BODY               = ServiceOperStatusProviderResp.BODY;
    ServiceOperStatusResp.HDR.billTags       = ServiceOperStatusProviderResp.HDR.billTags;

    // Добавляем таги
    ServiceOperStatusResp.BODY.tagTransaction.idComBill = idComBill;
    AddTag (ServiceOperStatusResp.HDR.billTags, TAG_TRANSACTION);

    ServiceOperStatusResp.BODY.tagAgentCode.extReqNo = ServiceOperStatusReq.BODY.tagAgentCode.extReqNo;
    AddTag (ServiceOperStatusResp.HDR.billTags, TAG_AGENT_CODE);

    if (isGotTagIp == true)
    {
      ServiceOperStatusResp.BODY.tagInfoPay = tagInfoPay;
      AddTag(ServiceOperStatusResp.HDR.billTags, TAG_INFO_PAY);
    }

    // Если предметный вернул статус что не может выполнить запрос статуса
    if (ServiceOperStatusProviderResp.HDR.uniKioskStatus == uksForbiddenByProvider) {
      ServiceOperStatusResp.HDR.uniKioskStatus = uksForbiddenByProvider;
      ServiceOperStatusResp.END.msgDateTime    = GetCurrentDateTime();

      Result = ERROR_OPER_STATUS_REQUEST_FORBIDDEN_BY_PROVIDER;

      // Inform (MS << "Предметный сервер не поддерживает запрос статуса. ", ClassID, mtWarning, Result);
      Inform(MSML << "Предметний сервер не підтримує запит стану. ", ClassID, mtWarning, Result);

      return Result;
    } /// if (ServiceOperStatusProviderResp.HDR.uniKioskStatus == uksForbiddenByProvider)

    // Если предметный вернул неуспешный статус отмечаем в бд транзакцию как неуспешную
    if (ServiceOperStatusProviderResp.HDR.uniKioskStatus != uksOK && transactionStatus == tsOk) { 
      ServiceOperStatusResp.HDR.uniKioskStatus = ServiceOperStatusProviderResp.HDR.uniKioskStatus;
      ServiceOperStatusResp.END.msgDateTime    = GetCurrentDateTime();

//       Result = DATABASE->SetTransactionsCommStatus (	
//         idComBill
//         ,tsPayCanceled
//       );
// 
//       CHECK_ERROR_RETURN(Result);

      Result = ERROR_INVALID_TRANSACTION_STATUS;

      //Inform (MS
      //  << "Статус транзакции в БД и в предметном отличается: "
      //  << "\nServiceOperStatusProviderResp.HDR.uniKioskStatus = " << ServiceOperStatusProviderResp.HDR.uniKioskStatus
      //  << "\ntransactionStatus" << transactionStatus << ". ",ClassID, mtError, Result);

      Inform(MSML
        << "Статус транзакції в БД відрізняється від статусу на ПРедметному сервері.: "
        << "\nServiceOperStatusProviderResp.HDR.uniKioskStatus = " << ServiceOperStatusProviderResp.HDR.uniKioskStatus
        << "\ntransactionStatus" << transactionStatus << ". ", ClassID, mtError, Result);

      return Result;
    } /// if (ServiceOperStatusProviderResp.HDR.uniKioskStatus != uksOK && transactionStatus == tsOk)

    ServiceOperStatusResp.END.msgDateTime = GetCurrentDateTime();

    // Отмечаем в бд что транзакция успешна
    if (ServiceOperStatusProviderResp.HDR.uniKioskStatus == uksOK) {
      /// ServiceOperStatusProviderResp.END.msgDateTime
      /// Result = DATABASE->SetTransactionsCommStatus ( idComBill,tsOk ); /// ServiceOperStatusProviderResp.HDR.payDateTime
      /// Result = DATABASE->ag_IntegSetTransactionCommStatus ( idComBill, ServiceOperStatusResp.END.msgDateTime, tsOk );
      Result = DATABASE->ag_IntegSetTransactionCommStatus ( idComBill, ServiceOperStatusProviderResp.HDR.payDateTime, tsOk );
      CHECK_ERROR_RETURN(Result);     

      if (isGotTagIp == true)
      {
        // Фінальна ініціалізація тагу IP 
        Result = DATABASE->ag_GetTagIPValues(
          tagInfoPay.IdTransaction,
          tagInfoPay.UnicAgentCode,
          ServiceOperStatusProviderReq.HDR.idBuyer,
          ServiceOperStatusReq.HDR.idService,

          tagInfoPay.IpDataList
        );

        ServiceOperStatusResp.END.msgDateTime = GetCurrentDateTime();

        CHECK_ERROR_RETURN(Result);

        tagInfoPay.tagValue = IntToString(tagInfoPay.IpDataList.size());
        ServiceOperStatusResp.BODY.tagInfoPay = tagInfoPay;
        AddTag(ServiceOperStatusResp.HDR.billTags, TAG_INFO_PAY);
      }
    }

    // Якщо операція була у процесі обробки або у стані 36 - uksStornoInHandProcess, то тоді така транзакція має бути скасована.
    if (ServiceOperStatusProviderResp.HDR.uniKioskStatus == uksCounterWithPrevValue)
    {
      Result = DATABASE->ag_IntegSetTransactionCommStatus(idComBill, ServiceOperStatusProviderResp.HDR.payDateTime, tsPayCanceled);
      CHECK_ERROR_RETURN(Result);
    }

    // Result = DATABASE->GetServiceTagsPay (
    Result = DATABASE->GetServiceTagsPay_v2 (
        ServiceOperStatusReq.HDR.idService,
        ServiceOperStatusProviderReq.HDR.idBuyer,

        billTagsForPay
      );

    CHECK_ERROR_RETURN(Result);

    // Добавляем обязательный таг DO
    if ( ExistsTag(billTagsForPay, TAG_DATE_OP) == true )
    {      
      AddTag (ServiceOperStatusResp.HDR.billTags, TAG_DATE_OP);
    }
    
    /// ServiceOperStatusResp.HDR.billTags
    if ( ExistsTag(ServiceOperStatusResp.HDR.billTags, TAG_DATE_OP) == true )
    {
      TPosDateTime locDateOp;
      /// Вичитуємо з таблиці transactionscomm DateOp
      Result = DATABASE->GetDateOp ( idComBill, locDateOp ); /// CHECK_ERROR_RETURN(Result);

      if ( Result != OK )
      {
        Result = OK;
        RemoveTag (ServiceOperStatusResp.HDR.billTags, TAG_DATE_OP);
      } /// if ( Result != OK )
      else 
      {
        ServiceOperStatusResp.BODY.tagDateOp.DateOperation = TPosDateTime_To_DateTimeString(locDateOp);
      } /// else
    } /// if ( ExistsTag(PayBillResp.HDR.billTags, TAG_DATE_OP) == true )

    if ((isGotTagIp == true) && (ServiceOperStatusResp.HDR.uniKioskStatus == uksOK))
    {
      // ServiceOperStatusResp.HDR.billTags.clear();
      if (ExistsTag(ServiceOperStatusResp.HDR.billTags, TAG_AGENT_CODE) == true)
      {
        RemoveTag(ServiceOperStatusResp.HDR.billTags, TAG_AGENT_CODE);
        RemoveTag(ServiceOperStatusResp.HDR.billTags, TAG_TRANSACTION);
      }
      AddTag(ServiceOperStatusResp.HDR.billTags, TAG_INFO_PAY);
    }

    if ((isGotTagIp == true) && (ServiceOperStatusResp.HDR.uniKioskStatus != uksOK))
    {
      RemoveTag(ServiceOperStatusResp.HDR.billTags, TAG_AGENT_CODE);
      // RemoveTag(ServiceOperStatusResp.HDR.billTags, TAG_TRANSACTION);
      RemoveTag(ServiceOperStatusResp.HDR.billTags, TAG_INFO_PAY);
    }

  } else {

    ServiceOperStatusResp.HDR.uniKioskStatus = uksServerBusy;
    ServiceOperStatusResp.END.msgDateTime    = GetCurrentDateTime();

    Result = ERROR_INVALID_TRANSACTION_STATUS;

    Inform (MSML
      << "Некоректний стан транзакції!!!"
      << "Парметри: idComBill = " << idComBill
      << ", transactionStatus = " << transactionStatus
      << ", millRqStatus = "      << millRqStatus
      << ", amountPayed = "       << amountPayed
      << " repeatMode = "         << repeatMode << ". "
      ,ClassID, mtError, Result);

    return Result;
  }

  ServiceOperStatusResp.END.msgDateTime    = GetCurrentDateTime();

  return Result;
}

//----------------------------------------------------------------------
// Отменить указанную операцию в бд
// Вход
//		
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::AcceptStornoReq (void)
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::AcceptStornorReq : ";
  int											    Result = OK;

  eUniKioskStatus             uniKioskStatus;
  eTransactionStatus          transactionStatus;
  eMillAcceptStatus           millRqStatus;
  std::string					        amountPayed;
  std::string                 stornoAmount;
  std::string					        repeatMode;
  std::string					        stReturn;
  TPosDateTime                payDateTime;
  TPosDateTime                fulFilledDateTime;
  bool                        isStornoMill = false;
  eMillError	                millErrorCode;
  std::string	                millErrorMessage;
  std::string		              MsgError;
  
  // Ставим статус по умолчанию ошибка сервера
	StornoResp.HDR.uniKioskStatus = uksServerBusy;

  // Заголовок телеграмм ответа
// struct S_INTEG_HEADER_RESP {
//   eUniKioskClientRequest idMsgType;		    // Идентификатор сообщения
//   eUniKioskStatus			uniKioskStatus;   // Статус универсального киоска
//   std::string					idService;        // Номер сервиса
//   std::string					idSubService;     // Код подуслуги
//   std::string					idClient;         // Номер счета клиента
//   TStdStringList      billTags;         // Таги , переданные в сообщении, Если нет, то «*»
// };

  // Формируем параметры по умолчанию для ответа
  StornoResp.HDR.idService    = StornoReq.HDR.idService;
  StornoResp.HDR.idSubService = StornoReq.HDR.idSubService;
  StornoResp.HDR.idClient     = StornoReq.HDR.idClient;
  StornoResp.END.msgDateTime  = GetCurrentDateTime();

  // При успешном возврате добавляем таги в ответ
  StornoResp.BODY.tagAgentCode = StornoReq.BODY.tagAgentCode;
  AddTag (StornoResp.HDR.billTags, TAG_AGENT_CODE);

// Заголовок телеграмм запроса
// struct S_PROVIDER_HEADER_REQ {
//   eUniKioskClientRequest idMsgType;		    // Идентификатор сообщения
//   std::string					idService;        // Номер сервиса
//   std::string					idSubService;     // Код подуслуги
//   std::string					idClient;         // Номер счета клиента
//   TPosDateTime        payDateTime;      // Дата время запроса 
//   std::string					idTerminal;	      // Идентификатор терминала
//   std::string					idBuyer;	        // Идентификатор байера
//   TStdStringList	    billTags;         // Таги , переданные в сообщении, Если нет, то «*»
// };

  //   1. Сформировать запрос статуса транзакции для предметного сервера
  StornoProviderReq.HDR.idService    = StornoReq.HDR.idService;
  StornoProviderReq.HDR.idSubService = StornoReq.HDR.idSubService;
  StornoProviderReq.HDR.idClient     = StornoReq.HDR.idClient;
  StornoProviderReq.HDR.payDateTime  = StornoReq.END.msgDateTime;
  StornoProviderReq.HDR.idTerminal   = StornoReq.END.idTerminal;

  // Вычитываем номер байера по его ключу
	Result = DATABASE->GetIdBuyerByKey (	
    StornoReq.END.keyValue,

    StornoProviderReq.HDR.idBuyer
		);

  StornoResp.END.msgDateTime    = GetCurrentDateTime();
  CHECK_ERROR_RETURN(Result);

  IdBuyer = StornoProviderReq.HDR.idBuyer;

  // Копируем таги запроса
//   StornoProviderReq.BODY = StornoReq.BODY;
//   StornoProviderReq.HDR.billTags = StornoReq.HDR.billTags;

  // Вычитываем данные необходимые для сторнирования на предметном сервере
  //Result = DATABASE->GetIntegTransactionData(
  //  StornoProviderReq.HDR.idBuyer,
  //  StornoReq.BODY.tagAgentCode.extReqNo,

  //  idComBill,
  //  transactionStatus,
  //  millRqStatus,
  //  amountPayed,
  //  repeatMode,
  //  stReturn,
  //  payDateTime,
  //  fulFilledDateTime
  //  uniKioskStatus
  //);
  // 
  Result = DATABASE->GetIntegTransactionData (	
      StornoProviderReq.HDR.idBuyer,
      StornoReq.BODY.tagAgentCode.extReqNo,
      StornoReq.HDR.idService,

      idComBill,
      transactionStatus,
      millRqStatus,
      amountPayed,
      repeatMode,
      stReturn,
      payDateTime,
      fulFilledDateTime,
      StornoResp.HDR.uniKioskStatus, 
      MsgError
  );

  StornoResp.END.msgDateTime = GetCurrentDateTime();
  CHECK_ERROR_RETURN(Result);

  Result = TagERRHandler(StornoProviderReq.HDR.idService, IdBuyer, MsgError, StornoProviderResp.BODY.tagErrInfo, StornoProviderResp.HDR.billTags);
  CHECK_ERROR_RETURN(Result);

  // Обновляем мониторинг
  Result = SetMonitoring_idComBill(idComBill);
  CHECK_ERROR_RETURN(Result);

  /// ----------------------------------------------------
  // Если ответ успешный то отмечаем транзакцию для возврата денег
  /// ----------------------------------------------------
  if ( StornoResp.HDR.uniKioskStatus == uksServerError )
  {  
    Result = ERROR_EXEC_STORNO_PROVIDER;

    Inform(MSML
      << "Ошибка сторнирования операции - несовпадение параметров платежа:"
      << " uniKioskStatus = " << StornoProviderResp.HDR.uniKioskStatus
      << ", idBuyer = "       << StornoProviderReq.HDR.idBuyer
      << ", extReqNo = "      << StornoReq.BODY.tagAgentCode.extReqNo
      << ", idService = "     << StornoReq.HDR.idService
      << ", idComBill = "     << idComBill
      << ", amountPayed = "   << amountPayed
      , ClassID, mtError, Result);

    return Result;
  }



  // Если транзакция была сделана не сегодня то возвращать ее нельзя
  TPosDateTime curDate = GetCurrentDateTime();

  StornoProviderReq.BODY.tagTradeSrv.idComBill = idComBill;

  // Если существует операция
  if (idComBill != "-1") {
    StornoResp.BODY.tagTransaction.idComBill = idComBill;
    AddTag (StornoResp.HDR.billTags, TAG_TRANSACTION);
  } else {
    StornoResp.HDR.uniKioskStatus = uksStornoForbidden;
    StornoResp.END.msgDateTime    = GetCurrentDateTime();

    Result = ERROR_INVALID_TRANSACTION_STATUS_FOR_STORNO;

    Inform (MS
      << "Сторно по этому сервису запрещено - транзакция не найдена"
      << " idBuyer = " << StornoProviderReq.HDR.idBuyer
      << " extReqNo = " << StornoReq.BODY.tagAgentCode.extReqNo
      << " idComBill = " << idComBill
      << " amountPayed = " << amountPayed
      << " IdService = " << StornoReq.HDR.idService
      << " stReturn = " << stReturn
      ,ClassID, mtWarning, Result);

    return Result;
  }
  // ---------------------------------------
  /// std::string billTagsForStorno;
  TStdStringList billTagsForStorno;

  Result = DATABASE->GetServiceTagsPay_v2 (
        StornoReq.HDR.idService,
        IdBuyer,

        billTagsForStorno
     );

  StornoResp.END.msgDateTime    = GetCurrentDateTime();
  CHECK_ERROR_RETURN(Result);
  // ---------------------------------------
  if ( ExistsTag(billTagsForStorno, TAG_DATE_OP) == true )
  {
    AddTag( StornoResp.HDR.billTags, TAG_DATE_OP );
    StornoResp.BODY.tagDateOp.DateOperation = TPosDateTime_To_DateTimeString( GetCurrentDateTime() );

    TPosDateTime locDateOp;
    /// Вичитуємо з таблиці transactionscomm DateOp
    Result = DATABASE->GetDateOp ( idComBill, locDateOp ); /// CHECK_ERROR_RETURN(Result);

    if ( Result != OK )
    {
      Result = OK;
      RemoveTag (StornoResp.HDR.billTags, TAG_DATE_OP);
    } /// if ( Result != OK )
    else 
    {
      StornoResp.BODY.tagDateOp.DateOperation = TPosDateTime_To_DateTimeString(locDateOp);
    } /// else
  } /// if ( ExistsTag(billTagsForStorno, TAG_DATE_OP) == true )
  // ---------------------------------------
  // Если транзакция уже была ранее отменена
  if (
    (transactionStatus == tsReturnedByClient
    || transactionStatus == tsReturnedByBank
    || transactionStatus == tsStorned)
    && (millRqStatus == masReturned)
    ) 
  {

    StornoResp.HDR.uniKioskStatus = uksStornoSuccessEarly;
    StornoResp.END.msgDateTime    = GetCurrentDateTime();
    
    Result = OK;

    Inform (MS
      << "Транзакция была отменена раньше"
      << " idBuyer = " << StornoProviderReq.HDR.idBuyer
      << " extReqNo = " << StornoReq.BODY.tagAgentCode.extReqNo
      << " amountPayed = " << amountPayed
      << " idComBill = " << idComBill
      << " transactionStatus = " << transactionStatus
      << " millRqStatus = " << millRqStatus
      ,ClassID, mtWarning, Result);

    return Result;
  }
  
  // Транзакция уходит на ручной возврат
  if (stReturn == "2") 
  {
    StornoResp.END.msgDateTime = GetCurrentDateTime();

    if ( ( fulFilledDateTime.tm_year != curDate.tm_year 
        || fulFilledDateTime.tm_mon  != curDate.tm_mon 
        || fulFilledDateTime.tm_mday != curDate.tm_mday) )
    {
      StornoResp.HDR.uniKioskStatus = uksStornoOperationWasYesterday;
      Result = ERROR_INVALID_TRANSACTION_STATUS_FOR_STORNO;

      Inform (MS
        << "Нельзя сторнировать транзакцию которая была сделана не сегодня"
        << " idBuyer = " << StornoProviderReq.HDR.idBuyer
        << " extReqNo = " << StornoReq.BODY.tagAgentCode.extReqNo
        << " idComBill = " << idComBill
        << " amountPayed = " << amountPayed
        << " transactionStatus = " << transactionStatus
        << " millRqStatus = " << millRqStatus
        << " fulFilledDateTime = " << TPosDateTime_To_DateTimeString(fulFilledDateTime)
        ,ClassID, mtError, Result);
    } else {
      StornoResp.HDR.uniKioskStatus = uksStornoInHandProcess;    

      Result = DATABASE->SetTransactionsCommStatus ( idComBill, tsStornoInHandProcess );

      Inform (MS
        << "Транзакция отмечена для ручного возврата"
        << " idBuyer = " << StornoProviderReq.HDR.idBuyer
        << " extReqNo = " << StornoReq.BODY.tagAgentCode.extReqNo
        << " idComBill = " << idComBill
        << " amountPayed = " << amountPayed
        ,ClassID, mtWarning, Result);

    }
       
    return Result;
  }
  
  // Сторно по этому сервису запрещено
  if ( 
    (stReturn == "0") && ( (stReturn != "1") || (stReturn != "2") )
    ) {

    StornoResp.HDR.uniKioskStatus = uksStornoForbidden;
    StornoResp.END.msgDateTime    = GetCurrentDateTime();

    Result = ERROR_INVALID_TRANSACTION_STATUS_FOR_STORNO;

    Inform (MS
      << "Сторно по этому сервису запрещено"
      << " idBuyer = " << StornoProviderReq.HDR.idBuyer
      << " extReqNo = " << StornoReq.BODY.tagAgentCode.extReqNo
      << " idComBill = " << idComBill
      << " amountPayed = " << amountPayed
      << " IdService = " << StornoReq.HDR.idService
      << " stReturn = " << stReturn
      ,ClassID, mtWarning, Result);

    return Result;
  }
 
  // Если статус транзакции не предусматриваем возврата то выходим
  if ( (transactionStatus != tsOk) 
    && ( (millRqStatus != masAccepted) || (millRqStatus != masReturned) )
    ) 
  {
    StornoResp.HDR.uniKioskStatus = uksStornoInHandProcess;
    StornoResp.END.msgDateTime    = GetCurrentDateTime();

    Result = ERROR_INVALID_TRANSACTION_STATUS_FOR_STORNO;

    Inform (MS
      << "Нельзя сторнировать транзакцию с такими статусами"
      << "Транзакция может быть возвращена только вручную"
      << " idBuyer = " << StornoProviderReq.HDR.idBuyer
      << " extReqNo = " << StornoReq.BODY.tagAgentCode.extReqNo
      << " idComBill = " << idComBill
      << " amountPayed = " << amountPayed
      << " transactionStatus = " << transactionStatus
      << " millRqStatus = " << millRqStatus
      ,ClassID, mtError, Result);

    return Result;
  }

  // Если статус транзакции успешный то проверяем дату проведения транзакции по fulFilled
  if (transactionStatus == tsOk) {
    if (/*!(payDateTime.tm_year != 100 && payDateTime.tm_mon != 0 && payDateTime.tm_mday != 1)
        &&*/ 
        (fulFilledDateTime.tm_year != curDate.tm_year 
        || fulFilledDateTime.tm_mon != curDate.tm_mon 
        || fulFilledDateTime.tm_mday != curDate.tm_mday)
		&& stReturn != "3" )  {

          StornoResp.HDR.uniKioskStatus = uksStornoOperationWasYesterday;
          StornoResp.END.msgDateTime    = GetCurrentDateTime();

          Result = ERROR_INVALID_TRANSACTION_STATUS_FOR_STORNO;

          Inform (MS
            << "Нельзя сторнировать транзакцию которая была сделана не сегодня"
            << " idBuyer = " << StornoProviderReq.HDR.idBuyer
            << " extReqNo = " << StornoReq.BODY.tagAgentCode.extReqNo
            << " idComBill = " << idComBill
            << " amountPayed = " << amountPayed
            << " transactionStatus = " << transactionStatus
            << " millRqStatus = " << millRqStatus
            << " fulFilledDateTime = " << TPosDateTime_To_DateTimeString(fulFilledDateTime)
            ,ClassID, mtError, Result);

          return Result;
    }
  // Если статус транзакции неуспешный или она проведена не до конца то проверяем дату проведения по payed
  } else {

    if ( /*!(payDateTime.tm_year != 100 && payDateTime.tm_mon != 0 && payDateTime.tm_mday != 1)
      && */
      (  payDateTime.tm_year != curDate.tm_year 
      || payDateTime.tm_mon  != curDate.tm_mon 
      || payDateTime.tm_mday != curDate.tm_mday)
	  && stReturn != "3" ) {

      StornoResp.HDR.uniKioskStatus = uksStornoOperationWasYesterday;
      StornoResp.END.msgDateTime    = GetCurrentDateTime();

      Result = ERROR_INVALID_TRANSACTION_STATUS_FOR_STORNO;

      Inform (MS
        << "Нельзя сторнировать транзакцию которая была сделана не сегодня"
        << " idBuyer = " << StornoProviderReq.HDR.idBuyer
        << " extReqNo = " << StornoReq.BODY.tagAgentCode.extReqNo
        << " idComBill = " << idComBill
        << " amountPayed = " << amountPayed
        << " transactionStatus = " << transactionStatus
        << " millRqStatus = " << millRqStatus
        << " payDateTime = " << TPosDateTime_To_DateTimeString(payDateTime)
        ,ClassID, mtError, Result);

      return Result;
    }
  }

  // Добавляем таги для редметного сервера
  StornoProviderReq.BODY.tagTradeSrv.idComBill = idComBill;
  StornoProviderReq.BODY.tagTradeSrv.repeat    = repeatMode;
  AddTag (StornoProviderReq.HDR.billTags, TAG_TRADE_SRV);

  // Отправляем запрос на сторно на предметный сервер
  Result = StornoBillProvider ();

  StornoResp.END.msgDateTime    = GetCurrentDateTime();
  CHECK_ERROR_RETURN(Result);

  // StornoResp.HDR.answerMessage = StornoProviderResp.HDR.answerMessage;
  /// StornoResp.END.msgDateTime = StornoProviderResp.HDR.payDateTime;
  /// 1.
  StornoResp.END.msgDateTime    = GetCurrentDateTime();
  
  // Если транзакция отмечена для ручного возврата ставим соответствующий статус в бд
  if (StornoProviderResp.HDR.uniKioskStatus == uksStornoInHandProcess) 
  {
    StornoResp.HDR.uniKioskStatus = uksStornoInHandProcess;
    StornoResp.END.msgDateTime    = GetCurrentDateTime();

    Result = DATABASE->SetTransactionsCommStatus ( idComBill, tsStornoInHandProcess );

    Inform(MSML
      << "Транзакция отмечена для ручного возврата"
      << " uniKioskStatus = " << StornoProviderResp.HDR.uniKioskStatus
      << " idBuyer = " << StornoProviderReq.HDR.idBuyer
      << " extReqNo = " << StornoReq.BODY.tagAgentCode.extReqNo
      << " idComBill = " << idComBill
      << " amountPayed = " << amountPayed
      ,ClassID, mtWarning, Result);

    return Result;
  }
  /// ----------------------------------------------------
  /// 2.
  /// ----------------------------------------------------
  if ( ExistsTag(billTagsForStorno, TAG_DATE_OP) == true )
  {
    AddTag( StornoResp.HDR.billTags, TAG_DATE_OP );
    
    if (StornoProviderResp.HDR.uniKioskStatus == uksStornoSuccess)
    {
      Result = DATABASE->SetTransactionsCommDateOp ( idComBill, StornoProviderResp.HDR.payDateTime );

      if ( Result != OK )
      {
        Result = OK;        
      }

      StornoResp.BODY.tagDateOp.DateOperation = TPosDateTime_To_DateTimeString(StornoProviderResp.HDR.payDateTime);

    } /// if (StornoProviderResp.HDR.uniKioskStatus == uksStornoSuccess)
    else if (StornoProviderResp.HDR.uniKioskStatus == uksStornoSuccessEarly)
    {
      TPosDateTime locDateOp;
      /// Вичитуємо з таблиці transactionscomm DateOp
      Result = DATABASE->GetDateOp ( idComBill, locDateOp ); /// CHECK_ERROR_RETURN(Result);

      if ( Result != OK )
      {
        Result = OK;
        RemoveTag (StornoResp.HDR.billTags, TAG_DATE_OP);
      } /// if ( Result != OK )
      else 
      {
        StornoResp.BODY.tagDateOp.DateOperation = TPosDateTime_To_DateTimeString(locDateOp);
      } /// else
    } else {
      StornoResp.BODY.tagDateOp.DateOperation = TPosDateTime_To_DateTimeString(StornoProviderResp.HDR.payDateTime);
    } /// else

  } /// if ( ExistsTag(billTagsForStorno, TAG_DATE_OP) == true )
  /// ----------------------------------------------------
  // Если ответ успешный то отмечаем транзакцию для возврата денег
  /// ----------------------------------------------------
  if (StornoProviderResp.HDR.uniKioskStatus != uksStornoSuccess 
    && StornoProviderResp.HDR.uniKioskStatus != uksStornoSuccessEarly) 
  {
    StornoResp.HDR.uniKioskStatus = StornoProviderResp.HDR.uniKioskStatus;
    StornoResp.END.msgDateTime    = GetCurrentDateTime();

    Result = ERROR_EXEC_STORNO_PROVIDER;

    Inform(MSML
      << "Ошибка сторнирования операции на предметном сервере"
      << " uniKioskStatus = " << StornoProviderResp.HDR.uniKioskStatus
      << " idBuyer = " << StornoProviderReq.HDR.idBuyer
      << " extReqNo = " << StornoReq.BODY.tagAgentCode.extReqNo
      << " idComBill = " << idComBill
      << " amountPayed = " << amountPayed
      ,ClassID, mtError, Result);

    return Result;
  }

  // отменяем операцию в БД
  Result = DATABASE->StornoIntegratorBill (	
    StornoProviderReq.HDR.idBuyer,
    StornoReq.BODY.tagAgentCode.extReqNo,

    idComBill, 
    uniKioskStatus, 
    stornoAmount,
    isStornoMill, 
    MsgError
  );
  
  StornoResp.END.msgDateTime    = GetCurrentDateTime();
  CHECK_ERROR_RETURN(Result);

  Result = TagERRHandler(StornoProviderReq.HDR.idService, IdBuyer, MsgError, StornoProviderResp.BODY.tagErrInfo, StornoProviderResp.HDR.billTags);
  CHECK_ERROR_RETURN(Result);

  StornoResp.BODY.tagTransaction.idComBill = idComBill;

  // Если статус возврата по бд не успешный выходим
  if (!IsOkUniKioskStatus(uniKioskStatus)) {
    StornoResp.HDR.uniKioskStatus = uniKioskStatus;
    StornoResp.END.msgDateTime    = GetCurrentDateTime();

    Result = ERROR_EXEC_STORNO_PROCEDURE;

    Inform(MSML
      << "Ошибка сторнирования операции по БД"
      << " uniKioskStatus = " << uniKioskStatus
      << " idBuyer = " << StornoProviderReq.HDR.idBuyer
      << " extReqNo = " << StornoReq.BODY.tagAgentCode.extReqNo
      << " stornoAmount from DB = " << stornoAmount
      << " idComBill = " << idComBill
      ,ClassID, mtError, Result);

    return Result;
  }

  // Возвращаем ваучер на молотилке без обращения к БД
  if (isStornoMill) {
    Result = BASE_SERVER_OBJECTS->MillRequest->ReturnMillVoucher (	
      StornoReq.HDR.idService,
      StornoProviderReq.HDR.idBuyer,
      StornoReq.BODY.tagAgentCode.extReqNo,
      idComBill,
      stornoAmount, 

      millErrorCode,
      millErrorMessage
      );

	if (millErrorCode == meEntryCreated && stReturn == "3") {
		TMillTradeStatusList			trList;
		S_MILL_TRADE_STATUS_DATA		trData;
		TListStringList					notExistsIdComBill,
										unknownCCodeIdComBill;

		StornoResp.HDR.uniKioskStatus = uksStornoInHandProcess;
		StornoResp.END.msgDateTime = GetCurrentDateTime();
		trData.idComBill = idComBill;
		trData.CCode2 = StornoProviderReq.HDR.idClient;
		trData.tranStatus = "9";
		trList["0"] = trData;

		// Видаляємо транзакцію із проводки, яку автоматом вже сформував Сіріус
		Result = BASE_SERVER_OBJECTS->MillRequest->SendTradeStatusMill(
			trList,

			notExistsIdComBill,
			unknownCCodeIdComBill
		);
		CHECK_ERROR_RETURN(Result);

		// А тепер можна відправити запит повернення
		Result = BASE_SERVER_OBJECTS->MillRequest->ReturnMillVoucher(
			StornoReq.HDR.idService,
			StornoProviderReq.HDR.idBuyer,
			StornoReq.BODY.tagAgentCode.extReqNo,
			idComBill,
			stornoAmount,

			millErrorCode,
			millErrorMessage
		);
	}

    // Если возврат неуспешный
    if ( (Result != OK) || (millErrorCode != meOK) ) {
      StornoResp.HDR.uniKioskStatus = uksStornoInHandProcess;
      StornoResp.END.msgDateTime    = GetCurrentDateTime();

      Inform(MSML
        << "Ошибка возврата денег по молотилке - не смогли выполнить запрос на молотилке"
        << " millErrorCode = " << millErrorCode
        << " millErrorMessage = " << millErrorMessage
        ,ClassID, mtError, Result);

      return Result;
    }

    // Отмечаем в бд что деньги успешно возвращены
    Result = BASE_DATABASE->ReturnMillCertificate (	
      StornoProviderReq.HDR.idBuyer,
      StornoReq.HDR.idService,
      StornoReq.BODY.tagAgentCode.extReqNo
      );

    StornoResp.END.msgDateTime    = GetCurrentDateTime();
    CHECK_ERROR_RETURN(Result);
  }

  StornoResp.HDR.uniKioskStatus = uniKioskStatus;
  StornoResp.END.msgDateTime    = GetCurrentDateTime();

  // При успешном возврате добавляем таги в ответ
  StornoResp.BODY.tagTransaction.idComBill = idComBill;
  AddTag (StornoResp.HDR.billTags, TAG_TRANSACTION);

  return Result;
}

//----------------------------------------------------------------------
// Обработать полученный запрос статуса счета в ОБД
// Вход
//		
// Выход
//	результат функции - результат выполнения операции
//----------------------------------------------------------------------
int MillExtAcceptor::AcceptStatusODB (void)
{
  TID											    ClassID = tidGivs;
  std::string							    ProcedureName = "MillExtAcceptor::AcceptStatusODB : ";
  int											    Result = OK;

  eTransactionStatus          transactionStatus;
  eMillAcceptStatus           millRqStatus;
  std::string					        amountPayed;
  std::string					        repeatMode;
  std::string					        stReturn;
  TPosDateTime                payDateTime;
  TPosDateTime                fulFilledDateTime;
//   std::string					        outIdComBill;
//   std::string					        outIdService;
//   std::string					        outIdSubService;
//   std::string					        outIdClient;
//   std::string					        outAmount;
//   std::string					        outAmountCommis;
//   TPosDateTime                outDateOperation;
//   std::string					        outIdCompany;
//   TStdStringList	            billTagsForPay;

	// Ставим статус по умолчанию ошибка сервера
	StatusODBResp.HDR.uniKioskStatus = uksServerError;

  // Читаем интегрированные настройки для полученного сервиса
//   Result = DATABASE->GetIntegServiceConfig (
//     StatusODBReq.HDR.idService,
// 
//     conf
//     );
//   CHECK_ERROR_RETURN(Result);
  
  // Заголовок телеграмм ответа
// struct S_INTEG_HEADER_RESP {
//   eUniKioskClientRequest idMsgType;		    // Идентификатор сообщения
//   eUniKioskStatus			uniKioskStatus;   // Статус универсального киоска
//   std::string					idService;        // Номер сервиса
//   std::string					idSubService;     // Код подуслуги
//   std::string					idClient;         // Номер счета клиента
//   TStdStringList      billTags;         // Таги , переданные в сообщении, Если нет, то «*»
// };

  // Формируем параметры по умолчанию для ответа
  StatusODBResp.HDR.idService = StatusODBReq.HDR.idService;
  StatusODBResp.HDR.idSubService = StatusODBReq.HDR.idSubService;
  StatusODBResp.HDR.idClient = StatusODBReq.HDR.idClient;
  StatusODBResp.END.msgDateTime = GetCurrentDateTime();

  // Вычитываем номер байера по его ключу
	Result = DATABASE->GetIdBuyerByKey (	
    StatusODBReq.END.keyValue,

    StatusODBProviderReq.HDR.idBuyer
		);

  CHECK_ERROR_RETURN(Result);

  // Вычитываем данные необходимые для сторнирования на предметном сервере
  Result = DATABASE->GetIntegTransactionData (
    StatusODBProviderReq.HDR.idBuyer,
    StatusODBReq.BODY.tagAgentCode.extReqNo,

    idComBill,
    transactionStatus,
    millRqStatus,
    amountPayed,
    repeatMode,
    stReturn,
    payDateTime,
    fulFilledDateTime
    );

  CHECK_ERROR_RETURN(Result);

  // Если транзакция отстутсвует сразу отвечаем
  if (idComBill == "-1") {
    ServiceOperStatusResp.HDR.uniKioskStatus = uksTransactionAbsent;

    Result = ERROR_TRANSACTION_ABSENT;

    Inform(MSML 
      << "Никуда не идем - транзакция отсутствует в бд."
      ,ClassID, mtWarning, Result);

    return Result;
  }

  //   1. Сформировать запрос статуса транзакции для предметного сервера
  StatusODBProviderReq.HDR.idService = StatusODBReq.HDR.idService;
  StatusODBProviderReq.HDR.idSubService = StatusODBReq.HDR.idSubService;
  StatusODBProviderReq.HDR.idClient = StatusODBReq.HDR.idClient;
  StatusODBProviderReq.HDR.payDateTime = StatusODBReq.END.msgDateTime;
  StatusODBProviderReq.HDR.idTerminal = StatusODBReq.END.idTerminal;

  // Копируем таги запроса
  StatusODBProviderReq.BODY = StatusODBReq.BODY;
  StatusODBProviderReq.HDR.billTags = StatusODBReq.HDR.billTags;

  StatusODBProviderReq.BODY.tagTradeSrv.repeat = "0";
  StatusODBProviderReq.BODY.tagTradeSrv.idComBill = idComBill;

  // Добавляем таги
  AddTag (StatusODBProviderReq.HDR.billTags, TAG_TRADE_SRV);

  // Удаляем лишние таги
  RemoveTag (StatusODBProviderReq.HDR.billTags, TAG_AGENT_CODE);
  RemoveTag (StatusODBProviderReq.HDR.billTags, TAG_TRANSACTION);
    
  // Отослать запрос на предметный сервер
  Result = StatusODBProvider();

  // Проверить ответ от предметного сервера
  if (Result != OK) {
    StatusODBResp.HDR.uniKioskStatus = uksServerBusy;

    return Result;
  } 

  StatusODBResp.HDR.uniKioskStatus = StatusODBProviderResp.HDR.uniKioskStatus;
  StatusODBResp.BODY = StatusODBProviderResp.BODY;
  StatusODBResp.HDR.billTags = StatusODBProviderResp.HDR.billTags;

  // Добавляем таги
  StatusODBResp.BODY.tagTransaction.idComBill = idComBill;
  AddTag (StatusODBResp.HDR.billTags, TAG_TRANSACTION);

  StatusODBResp.BODY.tagAgentCode.extReqNo = StatusODBReq.BODY.tagAgentCode.extReqNo;
  AddTag (StatusODBResp.HDR.billTags, TAG_AGENT_CODE);

  return Result;
}

//----------------------------------------------
// Обробка запитів на командний порт
//----------------------------------------------
int MillExtAcceptor::processingCmdPortGen(int inPort, const std::string Request, std::string& Response)
{
  int Result  = OK;
  TID ClassID = tidGivs;
  std::string ProcedureName = "MillExtAcceptor::processingCmdPortGen : ";
  
  ConnectHostGuard HostCon;
  std::string address = "127.0.0.1";
  int         port = inPort;
  //----------------------
  //Response = "0;" + Request;
  //if (!Response.empty())
  //{
  //  return Result;
  //}
  //----------------------
  Result = OpenTcpConnection(
    HostCon,
    address,
    port
  );
   
  if (Result != OK)
  {
    Response = IntToString(Result) + MILL_SERVER_PACKET_DIVIDER + "Не зміг підєднатися до " + address + ":" + IntToString(port) + MILL_SERVER_PACKET_DIVIDER;
    CHECK_ERROR_RETURN(Result);
  }
  //----------------------
  Result = ExecTcpRequest(
    HostCon,
    Etx,
    Etx,
    Request,
    Response
  );

  if (Result != OK)
  {
    Response = IntToString(Result) + MILL_SERVER_PACKET_DIVIDER + "Помилка при обміні з " + address + ":" + IntToString(port) + MILL_SERVER_PACKET_DIVIDER;
    CHECK_ERROR_RETURN(Result);
  }

  Response = IntToString(Result) + MILL_SERVER_PACKET_DIVIDER + Response + MILL_SERVER_PACKET_DIVIDER;
  //----------------------
  return Result;
}

//----------------------------------------------------------------------
// Підготувати таг S_tagErrInfo
//----------------------------------------------------------------------
int MillExtAcceptor::prepareTags(
  std::string idService,
  std::string idBuyer,
  std::string MsgError,

  S_tagErrInfo& tag,
  TStdStringList& billTags
)
{
  int Result  = OK;
  TID ClassID = tidGivs;
  std::string ProcedureName = "MillExtAcceptor::prepareTags : ";
  //----------------------
  Result = DATABASE->isa_ERRtag(
    idService,
    idBuyer,
    MsgError,
    tag.IdMsgAdm, tag.IdMsgCl, tag.Info2Adm, tag.Info2Client
  );
  CHECK_ERROR_RETURN(Result);

  // ERR;ERR;112012;Надано неукраїнський номер рахунка IBAN;212012;Надано неукраїнський номер рахунка IBAN
  // tag.IdMsgAdm = "112012"; tag.IdMsgCl = "Надано неукраїнський номер рахунка IBAN"; tag.Info2Adm = "212012"; tag.Info2Client = "Надано неукраїнський номер рахунка IBAN";

  if ( ((tag.IdMsgAdm != "100") || (tag.IdMsgCl != "200")) && (tag.Info2Adm != "*") && (tag.Info2Client != "*"))
  {
    billTags.clear();
    AddTag(billTags, TAG_ERR_INFO);
  }
  //----------------------
  return Result;
}

//----------------------------------------------------------------------
// Підготувати таг S_tagErrInfo
//----------------------------------------------------------------------
int MillExtAcceptor::TagERRHandler(
  std::string idService,
  std::string idBuyer,
  std::string MsgError,

  S_tagErrInfo& tag,
  TStdStringList& billTags
)
{
  int Result = OK;
  TID ClassID = tidGivs;
  std::string ProcedureName = "MillExtAcceptor::TagERRHandler : ";
  //----------------------
  if (MsgError[0] == '*')
  {
    Result = prepareTags(
      idService,
      IdBuyer,
      MsgError,
      tag,
      billTags
    );

    if (ExistsTag(billTags, TAG_ERR_INFO)) {
      Result = ERROR_INVALID_TAG_VALUE; // 2+12000+800000 = 812002 - просто код помилки, щоб відразу сформувати у відповіді лише таг ERR.
      return Result;
    }
  }
  //----------------------
  return Result;
}

/// <summary>
/// Метод відсилки СМС з кодом 
/// </summary>
/// <param name="inIdComBill">Ідентифікатор операції</param>
/// <param name="inTelNumber">Номер телефону</param>
/// <param name="inCode">Коду/повідомлення</param>
/// <returns>Результат виконання методу</returns>
int MillExtAcceptor::SendSMS(std::string inIdComBill, std::string inTelNumber, std::string inCode)
{
  int Result = OK;
  TID ClassID = tidGivs;
  std::string ProcedureName = "MillExtAcceptor::SendSMSHandler : ";
  //----------------------
  /*
  // Закоментував, бо вичитую дані для SMSUrl з файлу конфігурації - опції SMS_URL
  S_PROVIDER_CONFIG SMS_conf;

  Result = DATABASE->GetProviderConf("5555", SMS_conf);

  if (Result != OK)
  {
    Result = OK;
    SMS_conf.IP_serv = "http://127.0.0.1";
    SMS_conf.port_serv = 55845;
  }

  BASE_PRE_ALLOC_CONF::instance()->SMSUrl = SMS_conf.IP_serv + ":" + IntToString(SMS_conf.port_serv);
  */

  Result = SendSMS_v2(inTelNumber, inCode);
  //----------------------
  return Result;
}
/// <summary>
/// Метод відсилки коду за допомогою телефоного дзвінка та обробники помилок
/// </summary>
/// <param name="inIdComBill">Ідентифікатор операції</param>
/// <param name="inTelNumber">Номер телефону</param>
/// <param name="inCode">Коду/повідомлення</param>
/// <param name="inIpPort">IP адреса та порт у форматі IP:PORT</param>
/// <returns>Результат виконання методу</returns>
int MillExtAcceptor::SendPhoneCall(std::string inIdComBill, std::string inTelNumber, std::string inCode, std::string inIpPort)
{
  int Result = OK;
  TID ClassID = tidGivs;
  std::string ProcedureName = "MillExtAcceptor::SendPhoneCall : ";
  //----------------------
  // std::string IpAddress = "127.0.0.1";
  // int Port = 55985;

  std::string IpAddress;
  int Port = 0;

  std::string Request;
  std::string Response;

  IpAddress = split_string_first(inIpPort, ":");
  Port      = StringToInt(split_string_second(inIpPort, ":"));
  //----------------------
  Result = ExecSendPhoneCall(inTelNumber, inCode, IpAddress, Port, Response);
  CHECK_ERROR_RETURN(Result);

  // [send_call_resp];0;+38094290X_0717;200
  TWebRequestList responseList; responseList.clear();

  responseList = ParseWebServerRequest(
    Response,
    TelRecordDivider,
    Etx
  );

  if (responseList[0] != "[send_call_resp]")
  {
    Result = ERROR_CALLPHONE_WRONG_TEL_ID; // 859517
    return Result;
  }

  if (responseList[1] != "0")
  {
    Result = ERROR_CALLPHONE_EXCHANGE_FAIL; // 859518

    Inform(MS << responseList[2], ClassID, mtMessage, Result);

    return Result;
  }

  if (responseList[3] != "200")
  {
    Result = ERROR_CALLPHONE_EXCHANGE_FAIL; // 859518
    return Result;
  }

  //
  //----------------------
  return Result;
}

/// <summary>
/// Перевірити IdClient на наявність номеру телефону або коду, який передає клієнт у цьому полі
/// </summary>
/// <param name="inIdClient">Ідентифікатор клієнта, який отримали у запиті</param>
/// <param name="outTelNumber">Отриманий з IdClient номер телефону клієнта</param>
/// <param name="outClientCode">Отриманий з IdClient код, який клієнт отримав з sms</param>
/// <returns>Результат виконання методу</returns>
int MillExtAcceptor::CheckIdClientforSmsParams(std::string& inIdClient, std::string& outTelNumber, std::string& outClientCode)
{
  int Result = OK;
  TID ClassID = tidGivs;
  std::string ProcedureName = "MillExtAcceptor::CheckIdClientforSmsParams : ";
  //----------------------
  outTelNumber = split_string_second(inIdClient, "#SMSPH");

  if ((!outTelNumber.empty()) && (outTelNumber.length() == 10))
  {
    inIdClient = split_string_first(inIdClient, "#SMSPH");

    return Result;
  }

  outClientCode = split_string_second(inIdClient, "#SMSCD");

  if ((!outClientCode.empty()) && (outClientCode.length() >= 1))
  {
    inIdClient = split_string_first(inIdClient, "#SMSCD");

    return Result;
  }

  //----------------------
  return Result;
}

/// <summary>
/// Метод відсилки СМС з кодом та обробники помилок
/// </summary>
/// <param name="inIdComBill">Ідентифікатор операції</param>
/// <param name="inTelNumber">Номер телефону</param>
/// <param name="inCode">Коду/повідомлення</param>
/// <param name="outUniKioskStatus">Статус виконання - код відповіді на запит</param>
/// <param name="outSendAuthType">Тип аутентифікації - 0 - sms, 1 - phone call</param>
/// <param name="outSmsAmnt">Сума за смс</param>
/// <param name="inSmsText">Смс текст до коду</param>
/// <returns>Результат виконання методу</returns>
int MillExtAcceptor::SendSMSHandler(std::string inIdComBill, std::string inTelNumber, std::string inCode, int inSendAuthType, std::string inSmsAmnt, std::string inSmsText, eUniKioskStatus& outUniKioskStatus)
{
  int Result = OK;
  TID ClassID = tidGivs;
  std::string ProcedureName = "MillExtAcceptor::SendSMSHandler : ";
  //----------------------
  std::string locTelNumber, locClientCode, locIpPort;
  int locStatus = 0;
  // 1 - записано телефон для відправки СМС (це початковий статус при створенні запису). В разі повторних запитів статус не змінюється.
  // 2 - записан код для відправки в смс.
  // 3 - смс відправлена.
  // 9 - відміна. код від партнера не приймати,
  // 10 - отриман код в операції. Все Ок, співпало. Можна виконувати операці. у постачальника.
  // 
  // 
  //10 - отриман код в операції. Все Ок, співпало. Можна виконувати операці. у постачальника.
  // Зробити вибірку select code_send, phone from SMS_CONTROL where idcombill= <idcombill> and mstatus=2 ;
  // 2 - записан код для відправки в смс.
  // Result = DATABASE->GetSmsParamsByStatus(inIdComBill, 2, locClientCode, locTelNumber);
  Result = DATABASE->GetSmsParams (inIdComBill, locStatus, locClientCode, locTelNumber, locIpPort);

  if (Result != OK)
  {
    // PayBillResp.HDR.uniKioskStatus 
    outUniKioskStatus = uksRepeatWithTheSameCertificate; // 21

    Inform(MSML
      << "Помилка вибірки з табл. SMS_CONTROL при отриманні коду для смс для: IdBuyer = " << IdBuyer
      << ",  extReqNo = " << PayBillReq.BODY.tagAgentCode.extReqNo << ", idComBill = " << inIdComBill << ", TelNumber = " << inTelNumber << ". ", ClassID, mtError, Result);

    return Result;
  }

  if (locStatus == 99)
  {
    outUniKioskStatus = uksRepeatWithTheSameCertificate; // 21

    Inform(MS << "Помилка вибірки з табл. SMS_CONTROL - відсутній запис для: idComBill = " << inIdComBill << ", TelNumber = " << inTelNumber << ". ", ClassID, mtMessage, Result);

    return Result;
  }

  if (locStatus == 3)
  {
    outUniKioskStatus = uksReturnMillSuccessEarly; // 51

    Inform(MS << "SMS вже була надіслана для: idComBill = " << inIdComBill << ", на TelNumber = " << inTelNumber << ". ", ClassID, mtMessage, Result);

    return Result;
  }

  if ((locStatus == 2) && (!locTelNumber.empty()) && (!locClientCode.empty()))  
  {
    switch (inSendAuthType)
    {
      case 0:
        Result = SendSMS(idComBill, locTelNumber, inSmsText + locClientCode);
        break;
      case 1:
        Result = SendPhoneCall(idComBill, locTelNumber, locClientCode, locIpPort);
        break;
      default:
        Result = ERROR_DEFINE_SEND_AUTH_TYPE; // 859515
        break;
    }

    if (Result != OK)
    {
      // PayBillResp.HDR.uniKioskStatus 
      outUniKioskStatus = uksRepeatWithTheSameCertificate; // 21

      Inform(MSML << "Помилка при надсиланні смс клієнтові/дзвінка з кодом на номер TelNumber = " << locTelNumber << ",  ClientCode = " << locClientCode << ". ", ClassID, mtError, Result);

      return Result;
    }
  }
  else
  {
    // PayBillResp.HDR.uniKioskStatus 
    outUniKioskStatus = uksRepeatWithTheSameCertificate; // 21

    Inform(MSML << "Помилка в даних TelNumber/ClientCode: " << locTelNumber << "/" << locClientCode << ". ", ClassID, mtError, Result);

    return Result;
  }
  // Змінили статус у locClientCode
  // 3 - смс відправлена.
  int locResult = DATABASE->UpdateSmsParamsByStatus(idComBill, 3);

  if (locResult != OK)
  {
    // PayBillResp.HDR.uniKioskStatus 
    outUniKioskStatus = uksRepeatWithTheSameCertificate; // 21

    Inform(MSML << "Виникла помилка при оновленні табл. SMS_CONTROL: idComBill = " << idComBill << ", mStatus = 3. ", ClassID, mtError, Result);

    return Result;
  }
  //----------------------
  // Надіслати запит M85 доя Sirius
  //----------------------
  //if (isNeedM85)
  //{
  //  Result = SendM85Handler(idComBill, locTelNumber, locClientCode, inSmsAmnt);

  //  if (Result != OK)
  //  {
  //    // PayBillResp.HDR.uniKioskStatus 
  //    outUniKioskStatus = uksRepeatWithTheSameCertificate; // 21
  //    ////// Невідіслана смс
  //    ////locResult = DATABASE->UpdateSmsParamsByStatus(idComBill, 13);
  //    ////
  //    ////if (locResult != OK)
  //    ////{
  //    ////  // PayBillResp.HDR.uniKioskStatus 
  //    ////  // outUniKioskStatus = uksServerBusy; // 23
  //    ////  
  //    ////  Inform(MSML << "Виникла помилка при оновленні табл. SMS_CONTROL: idComBill = " << idComBill << ", mStatus = 13. ", ClassID, mtError, Result);
  //    ////  
  //    ////  return Result;
  //    ////}
  //    ////  
  //    ////Inform(MSML << "Виникла помилка при спробі надіслати запит M85 до Sirius: idComBill = " << idComBill << ". ", ClassID, mtError, Result);
  //
  //    return Result;
  //  }
  //}  
  //----------------------
  // PayBillResp.HDR.uniKioskStatus 
  outUniKioskStatus = uksReturnMillSuccessEarly; // 51
  //----------------------
  return Result;
}

/// <summary>
/// Метод відсилки запиту M85 для фіксації даних про надсилання смс клієнтові (для списання з Агента плати за смс)
/// </summary>
/// <param name="inIdComBill">Ідентифікатор операції</param>
/// <param name="inTelNumber">Номер телефону</param>
/// <param name="inCode">Коду/повідомлення</param>
/// <returns>Результат виконання методу</returns>
int MillExtAcceptor::SendM85Handler(std::string inIdComBill, std::string inTelNumber, std::string inCode, std::string inSmsAmnt)
{
  int Result = OK;
  TID ClassID = tidGivs;
  std::string ProcedureName = "MillExtAcceptor::SendM85Handler : ";
  //----------------------
  TListStringList AcceptedIdComBill;    AcceptedIdComBill.clear();
  TListStringList NotAcceptedIdComBill; NotAcceptedIdComBill.clear();

  S_MILL_TRADE_PAYOWNER_DATA data;
  TMillTradePayownerList payownerDataList; payownerDataList.clear();
  //----------------------
  data.idcombill = inIdComBill;
  data.sms       = inSmsAmnt;

  payownerDataList[data.idcombill] = data;

  Result = BASE_SERVER_OBJECTS->MillRequest->SendPayOwnerAddonTxData(
    payownerDataList,

    AcceptedIdComBill,
    NotAcceptedIdComBill
  );
  // CHECK_ERROR_RETURN(Result);

  if (Result != OK)
  {
    // PayBillResp.HDR.uniKioskStatus 
    // outUniKioskStatus = uksRepeatWithTheSameCertificate; // 21
    // Невідіслана смс
    int locResult = DATABASE->UpdateSmsParamsByStatus(idComBill, 13);

    if (locResult != OK)
    {
      // PayBillResp.HDR.uniKioskStatus 
      // outUniKioskStatus = uksServerBusy; // 23

      Inform(MSML << "Виникла помилка при оновленні табл. SMS_CONTROL: idComBill = " << inIdComBill << ", mStatus = 13. ", ClassID, mtError, Result);

      return Result;
    }

    Inform(MSML << "Виникла помилка при спробі надіслати запит M85 до Sirius: idComBill = " << inIdComBill << ". ", ClassID, mtError, Result);

    return Result;
  }

  //----------------------
  return Result;
}

/// <summary>
/// Для зазначеного тага викликати певний обробник, виконати аналіз на помилку (сформувати таг ERR) та повернути результат
/// </summary>
/// <param name="inIdComBill">Ідентифікатор операції</param>
/// <param name="inIdBuyer">Ідентифікатор Агента</param>
/// <param name="inIdService">Ідентифікатор  сервісу</param>
/// <param name="inAmount">Сума операції. Якщо запит рахунку, то сума 0</param>
/// <param name="inPayDate">Дата чи час формування запиту</param>
/// <param name="tagName">Назва тага</param>
/// <param name="inBillTags">Перелік тагів у списку</param>
/// <param name="outBODYReq">Набір тагів для обробки у запиті</param>
/// <param name="outBODYResp">Набір тагів для обробки у відповіді </param>
/// <param name="outUniKioskStatus">Статус запиту</param>
/// <returns>Результат виконання: 0 - успіх, не 0 - помилка</returns>
int MillExtAcceptor::TagNameHandle(
  std::string	 inIdComBill,
  std::string	 inIdBuyer,
  std::string	 inIdService,
  std::string	 inAmount,
  TPosDateTime inPayDate,
  const std::string tagName,
  TStdStringList& inBillTags,
  S_TAG_BODY& outBODYReq,
  S_TAG_BODY& outBODYResp,
  eUniKioskStatus& outUniKioskStatus
)
{
  int Result = OK;
  TID ClassID = tidGivs;
  std::string ProcedureName = "MillExtAcceptor::TagNameHandle : ";
  //----------------------
  std::string MsgError;
  //----------------------
  // RFU
  //----------------------
  // if (ExistsTag(inBillTags, tagName))
  if (tagName == TAG_ACC_OWNER_PAY)
  {
    Result = DATABASE->Add_AWPAWF(
      inIdComBill, // "0", // IdComBill
      inIdBuyer,
      inIdService,
      inAmount,
      inPayDate,
      tagName,
      outBODYReq.tagAccOwnerPay,

      outUniKioskStatus,
      MsgError
    );

    if (!IsOkUniKioskStatus(outUniKioskStatus))
    {
      Result = TagERRHandler(inIdService, inIdBuyer, MsgError, outBODYResp.tagErrInfo, inBillTags);
      CHECK_ERROR_RETURN(Result);

      return Result;
    }
  }
  //----------------------
  if (tagName == TAG_ACC_OWNER_FACT)
  {
    Result = DATABASE->Add_AWPAWF(
      inIdComBill, // "0", // IdComBill
      inIdBuyer,
      inIdService,
      inAmount,
      inPayDate,
      tagName,
      (S_tagAccOwnerPay)outBODYReq.tagAccOwnerFact,

      outUniKioskStatus,
      MsgError
    );

    if (!IsOkUniKioskStatus(outUniKioskStatus))
    {
      Result = TagERRHandler(inIdService, inIdBuyer, MsgError, outBODYResp.tagErrInfo, inBillTags);
      CHECK_ERROR_RETURN(Result);

      return Result;
    }
  }
  //----------------------
  return Result;
}
