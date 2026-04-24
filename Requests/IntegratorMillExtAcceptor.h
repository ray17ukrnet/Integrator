#ifndef __MILL_EXT_ACCEPTOR_H__
#define __MILL_EXT_ACCEPTOR_H__

// Парсинг запросов по универсальному ГИВЦу MILL
#include "UniKiosk/IntegratorBase.h"

//*****************************************************************************
// Класс парсера запросов для ГИВЦа без блокировок
//*****************************************************************************
class MillExtAcceptor : public IntegratorBase
{

public:

  /// <summary>
  /// Конструктор
  /// </summary>
  MillExtAcceptor ( );
    
  /// <summary>
  /// Деструктор
  /// </summary>
  virtual ~MillExtAcceptor ();

protected:

  // Текущий период счета
  std::string                 Period;

  // Конфигурация для интегрированного севиса
  S_INTER_CONFIG              conf;

  //-------------------------------------------------------------------------
  // Обрабатывает запрос
  // Вход
  //	
  // Выход 
  //	  результат функции - результат выполнения
  //-------------------------------------------------------------------------
  virtual int ProcessRequest (void);

  //-------------------------------------------------------------------------
  // Получить и обработать сервисный запрос
  // Вход
  //  нет
  // Выход 
  //	результат функции - результат выполнения
  //-------------------------------------------------------------------------
  virtual int ProcessServiceRequest (void);

  //---------------------------------------------------------------------------
  // Возвращает название оплаты для базы данных
  //---------------------------------------------------------------------------
  virtual std::string GetBillName (void);

  //----------------------------------------------------------------------
  // Подготовить таг S_tagCheque1
  //----------------------------------------------------------------------
  int prepareTags (
    std::string                 idService,
    std::string                 idBuyer,

    S_tagCheque1                &tag,
    TStdStringList	            &billTags
    );

  //----------------------------------------------------------------------
  // Подготовить таг S_tagCheque1 для сервисов с IdCompany
  //----------------------------------------------------------------------
  int prepareTags (
    std::string                 idService,
    std::string                 idCompany,
    std::string                 idBuyer,

    S_tagCheque1                &tag,
    TStdStringList	            &billTags
    );

  //----------------------------------------------------------------------
  // Подготовить таг S_tagCheque2
  //----------------------------------------------------------------------
  int prepareTags (
    std::string                 idService,
    std::string                 idSubService,
    std::string                 idBuyer,
    std::string                 extReqNo,
    std::string                 idCompany,

    S_tagCheque2                &tag,
    TStdStringList	            &billTags
    );

  //----------------------------------------------------------------------
  // Подготовить таг S_tagCheque3
  //----------------------------------------------------------------------
  int prepareTags (
    std::string                 idService,
    std::string                 idSubService,
    std::string                 idBuyer,
    std::string                 extReqNo,
    std::string                 idCompany,

    S_tagCheque3                &tag,
    TStdStringList	            &billTags
    );

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
  int addIntegratorRequestTags (
    TStdStringList	            billTagsToAdd,
    S_INTEG_HEADER_REQ	        HDR,
    S_TAG_BODY                  tagBodyReq,
    S_PROVIDER_HEADER_RESP      provHDR,
    S_TAG_BODY                  tagProvBodyResp,

    S_TAG_BODY                  &tagBody,
    TStdStringList	            &billTags,
    eUniKioskStatus             &uniKioskStatus
    );

  //----------------------------------------------------------------------
  // Сохранить присланные в запросе таги в бд
  // Вход
  //    idComBill - номер транзакции по бд 
  //    HDR - заголовок запроса
  //    tag - данные тага
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  int storeTags (
    std::string                 idComBill,
    S_INTEG_HEADER_REQ	        HDR,
    TagCounter                  tag
    );

  //----------------------------------------------------------------------
  // Сохранить присланные в запросе таги в бд
  // Вход
  //    idComBill - номер транзакции по бд 
  //    HDR - заголовок запроса
  //    tag - данные тага
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  int storeTags (
    std::string                 idComBill,
    S_INTEG_HEADER_REQ	        HDR,
    S_tagPeriod                 tag
    );

  //----------------------------------------------------------------------
  // Сохранить присланные в запросе таги в бд
  // Вход
  //    idComBill - номер транзакции по бд 
  //    HDR - заголовок запроса
  //    tagList - данные тага
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  int storeTags (
    std::string                 idComBill,
    S_INTEG_HEADER_REQ	        HDR,
    TagInfoFromClient           tagList
    );

  //----------------------------------------------------------------------
  // Сохранить присланные в запросе таги в бд
  // Вход
  //    idComBill - номер транзакции по бд 
  //    HDR - заголовок запроса
  //    tag - данные тага
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
//   int storeTags (
//     std::string                 idComBill,
//     S_INTEG_HEADER_REQ	        HDR,
//     S_tagCheque3                tag
//     );

  //----------------------------------------------------------------------
  // Сохранить присланные в запросе таги в бд
  // Вход
  //    std::string                 idComBill,
  //    HDR - заголовок запроса
  //    tagBody - данные тагов
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  int storeIntegratorRequestTags (
    std::string                 idComBill,
    S_INTEG_HEADER_REQ	        HDR,
    S_TAG_BODY                  tagBody
    );

  //----------------------------------------------------------------------
  // Сохранить присланные в запросе таги в бд
  // Вход
  //    idComBill - ид транзакции
  //    conf - конфигурация для сервиса
  //    HDR - заголовок запроса
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  int storeTagsList (
    std::string                 idComBill,
    S_INTER_CONFIG		          conf,
    S_INTEG_HEADER_REQ	        HDR
    );

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
  int execInternalServRequest (
    std::string                 inIdService,
    std::string                 inIdSubService,
    TWebRequestList             requestList,

    eUniKioskStatus             &uniKioskStatus,
    TWebRequestList	            &responseList
    );

  //----------------------------------------------------------------------
  // Создать значение запроса по умолчанию
  //----------------------------------------------------------------------
  std::string CreateDefaultValue (
    std::string				  value
    );

  //----------------------------------------------------------------------
  // Запросить счет
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  int GetBillProvider (void);

  //----------------------------------------------------------------------
  // Оплатить счет
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  int PayBillProvider (void);

  //----------------------------------------------------------------------
  // Запрос состояния операции
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  int OperStatusProvider (void);

  //----------------------------------------------------------------------
  // Сделать сторно по счету
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  int StornoBillProvider (void);

  //----------------------------------------------------------------------
  // Запрос состояния операции ОДБ
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  int StatusODBProvider (void);

  //----------------------------------------------------------------------
  // Обработать полученный запрос счета
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  virtual int AcceptGetBill (void);
  
  //----------------------------------------------------------------------
  // Обработать полученный запрос оплаты счета по молотильному интерфейсу
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  virtual int AcceptPayBillMill (void);

  //----------------------------------------------------------------------
  // Обработать полученный запрос статуса транзакции
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  virtual int AcceptServiceOperStatusReq (void);

  //----------------------------------------------------------------------
  // Отменить указанную операцию в бд
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  virtual int AcceptStornoReq (void);

  //----------------------------------------------------------------------
  // Обработать полученный запрос статуса счета в ОБД
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  virtual int AcceptStatusODB (void);

  //----------------------------------------------
  // Обробка запитів на командний порт
  //----------------------------------------------
  virtual int processingCmdPortGen (int inPort, const std::string Request, std::string& Response);

  //-------------------------------------------------------------------------
  // Получить и обработать сверка транзакций списком (135/136)
  // Вход
  //  нет
  // Выход 
  //	результат функции - результат выполнения
  //-------------------------------------------------------------------------
  //   int AcceptServiceOperList (void);

  //----------------------------------------------
  // Обробка запитів на командний порт
  //----------------------------------------------
  // virtual int processingCmdPortGen(int inPort, const std::string Request, std::string & Response);

  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

  //----------------------------------------------------------------------
  // Підготувати таг S_tagErrInfo
  //----------------------------------------------------------------------
  int prepareTags(
    std::string idService,
    std::string idBuyer,
    std::string MsgError,

    S_tagErrInfo& tag,
    TStdStringList& billTags
  );

  //----------------------------------------------------------------------
  // Обробник повідомлення у відповідь для формування тага S_tagErrInfo
  //----------------------------------------------------------------------
  int TagERRHandler(
    std::string idService,
    std::string idBuyer,
    std::string MsgError,

    S_tagErrInfo& tag,
    TStdStringList& billTags
  );

  /// <summary>
  /// Метод відсилки СМС з кодом 
  /// </summary>
  /// <param name="inIdComBill">Ідентифікатор операції</param>
  /// <param name="inTelNumber">Номер телефону</param>
  /// <param name="inCode">Коду/повідомлення</param>
  /// <param name="outSendAuthType">Тип аутентифікації - 0 - sms, 1 - phone call</param>
  /// <param name="outUniKioskStatus">Статус виконання - код відповіді на запит</param>
  /// <param name="outSmsAmnt">Сума за смс</param>
  /// <param name="inSmsText">Смс текст до коду</param>
  /// <returns>Результат виконання методу</returns>
  int SendSMSHandler(std::string inIdComBill, std::string inTelNumber, std::string inCode, int inSendAuthType, std::string inSmsAmnt, std::string inSmsText, eUniKioskStatus&	outUniKioskStatus);

  /// <summary>
  /// Перевірити IdClient на наявність номеру телефону або коду, який передає клієнт у цьому полі
  /// </summary>
  /// <param name="inIdClient">Ідентифікатор клієнта, який отримали у запиті</param>
  /// <param name="outTelNumber">Отриманий з IdClient номер телефону клієнта</param>
  /// <param name="outClientCode">Отриманий з IdClient код, який клієнт отримав з sms</param>
  /// <returns>Результат виконання методу</returns>
  int CheckIdClientforSmsParams(std::string& inIdClient, std::string& outTelNumber, std::string& outClientCode);

  /// <summary>
  /// Метод відсилки СМС з кодом та обробники помилок
  /// </summary>
  /// <param name="inIdComBill">Ідентифікатор операції</param>
  /// <param name="inTelNumber">Номер телефону</param>
  /// <param name="inCode">Коду/повідомлення</param>
  /// <returns>Результат виконання методу</returns>
  int SendSMS(std::string inIdComBill, std::string inTelNumber, std::string inCode);

  /// <summary>
  /// Метод відсилки коду за допомогою телефоного дзвінка та обробники помилок
  /// </summary>
  /// <param name="inIdComBill">Ідентифікатор операції</param>
  /// <param name="inTelNumber">Номер телефону</param>
  /// <param name="inCode">Коду/повідомлення</param>
  /// <param name="inIpPort">IP адреса та порт у форматі IP:PORT</param>
  /// <returns>Результат виконання методу</returns>
  int SendPhoneCall(std::string inIdComBill, std::string inTelNumber, std::string inCode, std::string inIpPort);

  /// <summary>
  /// Метод відсилки запиту M85 для фіксації даних про надсилання смс клієнтові (для списання з Агента плати за смс)
  /// </summary>
  /// <param name="inIdComBill">Ідентифікатор операції</param>
  /// <param name="inTelNumber">Номер телефону</param>
  /// <param name="inCode">Коду/повідомлення</param>
  /// <returns>Результат виконання методу</returns>
  int SendM85Handler(std::string inIdComBill, std::string inTelNumber, std::string inCode, std::string inSmsAmnt);

  
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
  int TagNameHandle(
    std::string	 inIdComBill,
    std::string	 inIdBuyer,
    std::string	 inIdService,
    std::string	 inAmount,
    TPosDateTime inPayDate,
    const std::string tagName, 
    TStdStringList&   inBillTags,  
    S_TAG_BODY&      outBODYReq,
    S_TAG_BODY&      outBODYResp,
    eUniKioskStatus& outUniKioskStatus
    );


private:


};

// Автоматический указатель
typedef std::auto_ptr<MillExtAcceptor>        autoMillExtAcceptor;


#endif // __MILL_EXT_ACCEPTOR_H__
