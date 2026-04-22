#ifndef __INTEGRATOR_DATABASE_H__
#define __INTEGRATOR_DATABASE_H__

// Определение типов для парсинга запросов по ГИВЦу
#include "UniKiosk/UniKioskGivsParserBase.h"

// Описание типов парсера запросов 
#include "UniKiosk/IntegratorRequestParser.h"

// Работа с базой данных
#include "Database/TradeDataBase.h"


//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Этот клас предназначен для работы с базой данных акциптированных
// ваучеров и проданного товара
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
class TGivsDatabase : public TTradeDatabase 
{
public:
	//----------------------------------------------------------------
	// Конструктор 
	//	Вход
	// 			DatabaseHost - Адресс сервера базы данных
	// 			DatabasePort - Порт
	// 			DatabaseName - Название базы данных на сервере
	// 			DatabaseLogin - Логин
	// 			DatabasePassword - Пароль
	//----------------------------------------------------------------
	TGivsDatabase::TGivsDatabase(
		std::string	aDatabaseHost,
		int					aDatabasePort,
		std::string	aDatabaseName,
		std::string	aDatabaseLogin,
		std::string	aDatabasePassword
		);

	//----------------------------------------------------------------
	// Деструктор
	//----------------------------------------------------------------
	virtual ~TGivsDatabase();

  //----------------------------------------------------------------
  // Возвращает номер сервиса для указанного ваучера
  // Вход
  //		inIdBuyer - номер байера
  //		inExtReqNo - номер по молотилке
  // Выход
  //		outIdService - номер сервиса
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int getIdServiceByMill (	
																	   std::string inIdBuyer,
																	   std::string inExtReqNo,

                                     std::string &outIdService
																	   );

  //----------------------------------------------------------------
  // Возвращаем текст для чека
  // Вход
  //		InIdService - номер сервиса
  //    InIdSubService - код услуги
  //    InIdBuyer - номер покупателя 
  //    inExtReqNo - номер по молотилке
  //    inIdCompany - код компании 0 - если неизвестно
  // Выход
  //    OutChequeText - текст чека
  //     outSender - отправитель
  //     outMFOSender - МФО отправителя
  //     outReceiver = получатель
  //     outZKPOReceiver - ЗКПО получателя
  //     outBankReceiver - банк получателя
  //     outMFOReceiver - МФО получателя
  //     outAccReceiver - счет получателя
  //     outPaymentText - описание платежа
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
//   int GetServiceChequeExt (	
//     std::string								InIdService,
//     std::string								InIdSubService,
//     std::string								InIdBuyer,
//     std::string								inExtReqNo,
//     std::string               inIdCompany,
// 
//   //   std::string				        &OutChequeText,
//     std::string				        &outSender,
//     std::string				        &outMFOSender,
//     std::string				        &outReceiver,
//     std::string				        &outZKPOReceiver,
//     std::string				        &outBankReceiver,
//     std::string				        &outMFOReceiver,
//     std::string				        &outAccReceiver,
//     std::string				        &outPaymentText
//     );

  //----------------------------------------------------------------
  // Возвращаем текст для чека
  // Вход
  //		InIdService - номер сервиса
  //    InIdCompany - код компании
  //    InIdBuyer - номер покупателя 
  // Выход
  //    OutChequeText - текст чека
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
//   int GetServiceChequeIdCompany (	
//     std::string								InIdService,
//     std::string								inIdCompany,
//     std::string								InIdBuyer,
// 
//     std::string				        &OutChequeText
//     );

  //----------------------------------------------------------------
  // Запрос счета - stage 1
  // Вход
  //   inIdService - номер сервиса
  //   inIdSubService - код услуги
  //   inIdClient - Идентификатор клиента
  //   inIdTerm - номер терминала
  //   inKeyValue - значения ключа подписи
  // Выход
  //   UniKioskStatus - статус универсального киоска
  //   outIdBuyer - номер байера
  //   outChequeText - текст чека
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int IntegProcess111Stage1 (	
    std::string					inIdService,
    std::string					inIdSubService,
    std::string					inIdClient,
    std::string					inIdTerm,
    std::string					inKeyValue,

    eUniKioskStatus     &UniKioskStatus,
    std::string					&outIdBuyer
//     std::string					&outChequeText
  );
  //----------------------------------------------------------------
  int IntegProcess111Stage1(
    std::string					inIdService,
    std::string					inIdSubService,
    std::string					inIdClient,
    std::string					inIdTerm,
    std::string					inKeyValue,

    eUniKioskStatus& UniKioskStatus,
    std::string&     outIdBuyer,
    std::string&     outMsgError
  );

  //----------------------------------------------------------------
  // Запит фіксації даних тага AW, коли присутні AL or AP на 111-у запиті - stage 0
  // Вход
  //   inTagType   - тип тагу (можливі варіанти AP, AL)
  //   inAWperson  - ПІБ платника. Дані тагу AW. Якщо таг не отримано, то передати '*' 
  //   inAWaddress - Адреса платника.
  //   inAWother   - інша інформація про платника
  //   inIdService - номер послуги
  //   inIdBuyer   - Iдентификатор Агента
  //   inIdClient  - Iдентификатор клієнта
  //   inIdTerm    - Iдентификатор терминалу
  //   inAmount    - numeric(12,2)
  // Выход
  //   UniKioskStatus    - статус универсального киоска
  //   результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int IntegProcess111Stage0_v1 (	
    std::string	inTagType,
    std::string	inAWperson,
    std::string	inAWaddress,
    std::string	inAWother,
    std::string	inIdService,

    std::string	inIdBuyer,
    std::string	inIdClient,
    std::string	inIdTerm,
    std::string	inAmount,

    eUniKioskStatus& UniKioskStatus
  );
  //----------------------------------------------------------------
  int IntegProcess111Stage0_v1(
    std::string	inTagType,
    std::string	inAWperson,
    std::string	inAWaddress,
    std::string	inAWother,
    std::string	inIdService,

    std::string	inIdBuyer,
    std::string	inIdClient,
    std::string	inIdTerm,
    std::string	inAmount,

    eUniKioskStatus& UniKioskStatus,
    std::string& outMsgError
  );

  //----------------------------------------------------------------
  // Запрос счета - stage 1
  // Вход
  //   inIdService - номер сервиса
  //   inIdSubService - код услуги
  //   inIdClient - Идентификатор клиента
  //   inIdTerm - номер терминала
  //   inKeyValue - значения ключа подписи
  //   inProviderUniKioskStatus - статус универсального киоска полученный от провайдера
  // Выход
  //   UniKioskStatus - статус универсального киоска
  //   outChequeText - текст чека
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int IntegProcess111Stage2 (	
    std::string					inIdService,
    std::string					inIdSubService,
    std::string					inIdClient,
    std::string					inIdTerm,
    std::string					inKeyValue,
    eUniKioskStatus     inProviderUniKioskStatus,

    eUniKioskStatus     &UniKioskStatus
//     std::string					&outChequeText
    );
  //----------------------------------------------------------------
  int IntegProcess111Stage2(
    std::string					inIdService,
    std::string					inIdSubService,
    std::string					inIdClient,
    std::string					inIdTerm,
    std::string					inKeyValue,
    eUniKioskStatus     inProviderUniKioskStatus,

    eUniKioskStatus& UniKioskStatus,
    std::string& outMsgError
  );

  //----------------------------------------------------------------
  // Оплата счета - stage 1
  // Вход
  //   inIdComBill - номер транзакции в БД
  //   inIdMillRq - номер транзакции по таблице молотилки
  //   inIdTransaction - номер транзакции по таблице транзакций
  //   inExtReqNo - номер сертификата
  //   inIdService - номер сервиса
  //   inIdSubService - Код подуслуги
  //   inIdClient - Идентификатор клиента
  //   inAmountPayed - оплаченная сумма
  //   inAmountCommis - размер комиссионных
  //   inIdTerm - номер терминала
  //   inKeyValue - значения ключа подписи
  // Выход
  //   UniKioskStatus - статус универсального киоска
  //   UniBillStatus - статус операции в БД
  //   outIdComBill - уникальный номер транзакции в БД
  //   outIdBuyer - номер покупателя
  //   outChequeText - текст чека
  //   outIdCompany - код компании
  //   isMillOvertaking - есть ли догонка по милу
  //   isProviderOvertaking - есть ли догонка по провайдеру
  //   isSuccessLocked - успешно проведена блокировка счета в БД или нет
  //   isAlwaysSendToProvider - отправлять ли повторный запрос оплаты на предметный сервер
  //   outPayDateTime - дата оплаты, если транзакция была оплачена ранее
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int IntegProcess113Stage1 (
    std::string					inIdComBill,
    std::string					inIdMillRq,
    std::string					inIdTransaction,
    std::string					inExtReqNo,
    std::string					inIdService,
    std::string					inIdSubService,
    std::string					inIdClient,
    std::string					inAmountPayed,
    std::string					inAmountCommis,
    std::string					inIdTerm,
    std::string					inKeyValue,

    eUniKioskStatus     &UniKioskStatus,
    eUniBillStatus      &UniBillStatus,
    std::string					&outIdComBill,
    std::string					&outIdBuyer,
    std::string					&outChequeText,
    std::string					&outIdCompany,
    bool                &isMillOvertaking,
    bool                &isProviderOvertaking,
    bool                &isSuccessLocked,
    bool                &isAlwaysSendToProvider,
    TPosDateTime        &outPayDateTime
  );

  //----------------------------------------------------------------
  int IntegProcess113Stage1(
    std::string					inIdComBill,
    std::string					inIdMillRq,
    std::string					inIdTransaction,
    std::string					inExtReqNo,
    std::string					inIdService,
    std::string					inIdSubService,
    std::string					inIdClient,
    std::string					inAmountPayed,
    std::string					inAmountCommis,
    std::string					inIdTerm,
    std::string					inKeyValue,

    eUniKioskStatus& UniKioskStatus,
    eUniBillStatus& UniBillStatus,
    std::string& outIdComBill,
    std::string& outIdBuyer,
    std::string& outChequeText,
    std::string& outIdCompany,
    bool& isMillOvertaking,
    bool& isProviderOvertaking,
    bool& isSuccessLocked,
    bool& isAlwaysSendToProvider,
    TPosDateTime& outPayDateTime,
    std::string& outMsgError
  );

  /// <summary>
  /// Метод для обробки СП IntegProcess113Stage1_v6 для задачі
  /// http://192.168.71.10/issues/48226
  /// </summary>
  /// <param name="inIdComBill"></param>
  /// <param name="inIdMillRq"></param>
  /// <param name="inIdTransaction"></param>
  /// <param name="inExtReqNo"></param>
  /// <param name="inIdService"></param>
  /// <param name="inIdSubService"></param>
  /// <param name="inIdClient"></param>
  /// <param name="inAmountPayed"></param>
  /// <param name="inAmountCommis"></param>
  /// <param name="inIdTerm"></param>
  /// <param name="inKeyValue"></param>
  /// <param name="UniKioskStatus"></param>
  /// <param name="UniBillStatus"></param>
  /// <param name="outMsgError"></param>
  /// <param name="outIdComBill"></param>
  /// <param name="outIdBuyer"></param>
  /// <param name="outIdCompany"></param>
  /// <param name="isMillOvertaking"></param>
  /// <param name="isProviderOvertaking"></param>
  /// <param name="isSuccessLocked"></param>
  /// <param name="isAlwaysSendToProvider"></param>
  /// <param name="outPayDateTime"></param>
  /// <returns>Результат виконання запиту</returns>
  int IntegProcess113Stage1 (
    std::string	inIdComBill,
    std::string	inIdMillRq,
    std::string	inIdTransaction,
    std::string	inExtReqNo,
    std::string	inIdService,
    std::string	inIdSubService,
    std::string	inIdClient,
    std::string	inAmountPayed,
    std::string	inAmountCommis,
    std::string	inIdTerm,
    std::string	inKeyValue,

    eUniKioskStatus& UniKioskStatus,
    std::string&     outMsgError,
    eUniBillStatus&  UniBillStatus,    
    std::string&     outIdComBill,
    std::string&     outIdBuyer,    // std::string& outChequeText,
    std::string&     outIdCompany,
    bool&            isMillOvertaking,
    bool&            isProviderOvertaking,
    bool&            isSuccessLocked,
    bool&            isAlwaysSendToProvider,
    TPosDateTime&    outPayDateTime    
  );

  //----------------------------------------------------------------
  // Контроль лімітов - stage 1, Add 0
  // Вход
  //   inIdComBill - номер транзакции в БД
  //   inIdService - номер сервиса
  //   inIdBuyer - номер покупателя
  //   inAmountPayed - оплаченная сумма
  //   inIdTerm - номер терминала
  // Выход
  //   UniKioskStatus - статус универсального киоска
  //   UniBillStatus - статус операции в БД
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int IntegProcess113Stage1Add0_v1 (
    std::string					inIdComBill,    
    std::string					inIdService,
    std::string					inIdBuyer,
    std::string					inAmountPayed,
    std::string					inIdTerm,

    eUniKioskStatus     &UniKioskStatus,
    eUniBillStatus      &UniBillStatus
  );

  //----------------------------------------------------------------
  /// <summary>
  /// Перевірити, чи потрібно надсилати смс для цього сервісу
  /// </summary>
  /// <param name="inIdComBill">Ідентифікатор транзакції</param>
  /// <param name="inIdService">Ідентифікатор сервісу</param>
  /// <param name="inIdBuyer">Адентифікатор Агента</param>
  /// <param name="inAmountPayed">Сума операції</param>
  /// <param name="inIdTerm">Ідентифікатор терміналу</param>
  /// <param name="inIdClient">Ідентифікатор клієнта без коду смс або номеру телефона</param>
  /// <param name="UniKioskStatus">Статус у відповідь на запит</param>
  /// <param name="UniBillStatus">Статус рахунку</param>
  /// <param name="outMsgError">Повідомлення про помилку</param>  
  /// <param name="outSendAuthType">Тип аутентифікації - 0 - sms, 1 - phone call</param> // <param name="isNeedM85">Ознака, чи потрібно надсилати М85 запит до Сіріус - true - так, false - ні</param>
  /// <param name="outSmsAmnt">Сума за смс</param>
  /// <param name="outSmsText">Смс текст до коду</param>
  /// <returns>Результат виконання методу</returns>
  int IntegProcess113Stage2Add(
    std::string	inIdComBill,
    std::string	inIdService,
    std::string	inIdBuyer,
    std::string	inAmountPayed,
    std::string	inIdTerm,
    std::string	inIdClient,

    eUniKioskStatus& UniKioskStatus,
    eUniBillStatus&  UniBillStatus,
    std::string&     outMsgError,
    int& outSendAuthType,
    std::string& outSmsAmnt,
    std::string& outSmsText
  );

  //----------------------------------------------------------------
  // Контроль лімітов - stage 1, Add 0, v2
  // Вход
  //   inIdComBill - номер транзакции в БД
  //   inIdService - номер сервиса
  //   inIdBuyer - номер покупателя
  //   inAmountPayed - оплаченная сумма
  //   inIdTerm - номер терминала
  //   inIdClient - идентификатор клиента
  // Выход
  //   UniKioskStatus - статус универсального киоска
  //   UniBillStatus - статус операции в БД
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int IntegProcess113Stage1Add0_v2 (
    std::string					inIdComBill,    
    std::string					inIdService,
    std::string					inIdBuyer,
    std::string					inAmountPayed,
    std::string					inIdTerm,
    std::string					inIdClient,

    eUniKioskStatus     &UniKioskStatus,
    eUniBillStatus      &UniBillStatus
  );

  //----------------------------------------------------------------
  int IntegProcess113Stage1Add0_v2(
    std::string					inIdComBill,
    std::string					inIdService,
    std::string					inIdBuyer,
    std::string					inAmountPayed,
    std::string					inIdTerm,
    std::string					inIdClient,

    eUniKioskStatus& UniKioskStatus,
    eUniBillStatus&  UniBillStatus,
    std::string&     outMsgError
  );

  //----------------------------------------------------------------
  // Оплата счета - stage 2
  // Вход
  //   inIdComBill - номер транзакции в БД
  //   inIdMillRq - номер транзакции по таблице молотилки
  //   inIdTransaction - номер транзакции по таблице транзакций
  //   inCertificateNumber - номер сертификата
  //   inIdService - номер сервиса
  //   inIdSubService - Код подуслуги
  //   inIdClient - Идентификатор клиента
  //   inName - название транзакции
  //   inAmountPayed - оплаченная сумма
  //   inAmountCommis - размер комиссионных
  //   inIdCompany - код компании
  //   inIdTerm - номер терминала
  //   inKeyValue - значения ключа подписи
  //   inReqDateTime - Дата время формирования сообщения
  //   inMillOperationId - номер операции по молотилке
  //   inMillAcceptTime - врема акцепта по молотилке
  //   inMillAcceptStatus - статус акцепта по молотилке
  //   inProviderAcceptStatus - результат оплаты на сервере провайдера
  //   inProviderExtNo - номер транзакции по провайдеру
  //   inUpdateMillAcceptTime - обновлять время акцептации по милу или нет
  //   inUniKioskStatus - статус универсального киоска
  // Выход
  //   outUniKioskStatus - статус универсального киоска
  //   outPayDateTime - дата и время оплаты
  //   outReturnVoucher - возвращать - 1 или не возвращать ваучер - 0
  //   outSendToOvertaking - отправлять запрос в догонку или нет
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int IntegProcess113Stage2 (	
    std::string					inIdComBill,
    std::string					inIdMillRq,
    std::string					inIdTransaction,
    std::string					inExtReqNo,
    std::string					inIdService,
    std::string					inIdSubService,
    std::string					inIdClient,
    std::string					inName,
    std::string					inAmountPayed,
    std::string					inAmountCommis,
    std::string					inIdCompany,
    std::string					inIdTerm,
    std::string					inKeyValue,
    TPosDateTime        inPayDateTime,
    std::string					inMillOperationId,
    TPosDateTime        inMillAcceptTime,
    eMillAcceptStatus   inMillAcceptStatus,
    eProviderAcceptStatus inProviderAcceptStatus,
    std::string					inProviderExtNo,
    int                 inUpdateMillAcceptTime,
    eUniKioskStatus     inUniKioskStatus,

    eUniKioskStatus     &outUniKioskStatus,
    TPosDateTime        &outPayDateTime,
    bool                &outReturnVoucher,
    bool                &outSendToOvertaking
    );
  //----------------------------------------------------------------
  int IntegProcess113Stage2(
    std::string					inIdComBill,
    std::string					inIdMillRq,
    std::string					inIdTransaction,
    std::string					inExtReqNo,
    std::string					inIdService,
    std::string					inIdSubService,
    std::string					inIdClient,
    std::string					inName,
    std::string					inAmountPayed,
    std::string					inAmountCommis,
    std::string					inIdCompany,
    std::string					inIdTerm,
    std::string					inKeyValue,
    TPosDateTime        inPayDateTime,
    std::string					inMillOperationId,
    TPosDateTime        inMillAcceptTime,
    eMillAcceptStatus   inMillAcceptStatus,
    eProviderAcceptStatus inProviderAcceptStatus,
    std::string					inProviderExtNo,
    int                 inUpdateMillAcceptTime,
    eUniKioskStatus     inUniKioskStatus,

    eUniKioskStatus& outUniKioskStatus,
    TPosDateTime& outPayDateTime,
    bool& outReturnVoucher,
    bool& outSendToOvertaking,
    std::string& outMsgError
  );

  //----------------------------------------------------------------
  // Вычитываем адрес куда подключаться к внутреннему серверу
  // Вход
  //     inIdService - номер сервиса
  //     inIdSubService - номер услуги
  // Выход
  //     outAddress - адрес
  //     outPort - порт
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
//   int GetInternalServAddress (
//     std::string         inIdService,
//     std::string         inIdSubService,
// 
//     std::string         &outAddress,
//     int                 &outPort
//     );

  //----------------------------------------------------------------
  // Вычитать данные по операции
  // Вход
  // 		inIdBuyer - Номер покупателя
  //    inCertificateNumber - номер сертификата
  //		
  // Выход
  //    outIdComBill - уникальный номер транзакции в БД
  //    outTransactionStatus - статус транзакции
  //    outMillRqStatus - статус по молотилке
  //    outAmountPayed - сумма транзакции
  //    outRepeatMode - посылать запрос на предметный или нет
  //    outStReturn - режим сторно транзакции
  //    outPayDateTime - дата оплаты транзакции
  //    outFulFilledDateTime - дата проведения транзакции на предметном сервере
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int GetIntegTransactionData (	
    std::string					inIdBuyer,
    std::string					inCertificateNumber,

    std::string					&outIdComBill,
    eTransactionStatus  &outTransactionStatus,
    eMillAcceptStatus   &outMillRqStatus,
    std::string					&outAmountPayed,
    std::string					&outRepeatMode,
    std::string					&outStReturn,
    TPosDateTime        &outPayDateTime,
    TPosDateTime        &outFulFilledDateTime
    );
  //----------------------------------------------------------------
  int GetIntegTransactionData(
    std::string					inIdBuyer,
    std::string					inCertificateNumber,

    std::string& outIdComBill,
    eTransactionStatus& outTransactionStatus,
    eMillAcceptStatus& outMillRqStatus,
    std::string& outAmountPayed,
    std::string& outRepeatMode,
    std::string& outStReturn,
    TPosDateTime& outPayDateTime,
    TPosDateTime& outFulFilledDateTime,
    std::string& outMsgError
  );

  //----------------------------------------------------------------
  // Вычитать данные по операции
  // Вход
  // 		inIdBuyer - Номер покупателя
  //    inCertificateNumber - номер сертификата
  //		
  // Выход
  //    outIdComBill - уникальный номер транзакции в БД
  //    outTransactionStatus - статус транзакции
  //    outMillRqStatus - статус по молотилке
  //    outAmountPayed - сумма транзакции
  //    outRepeatMode - посылать запрос на предметный или нет
  //    outStReturn - режим сторно транзакции
  //    outPayDateTime - дата оплаты транзакции
  //    outFulFilledDateTime - дата проведения транзакции на предметном сервере
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int GetIntegTransactionData(
    std::string					inIdBuyer,
    std::string					inCertificateNumber,
    std::string					inIdService,

    std::string&        outIdComBill,
    eTransactionStatus& outTransactionStatus,
    eMillAcceptStatus&  outMillRqStatus,
    std::string&        outAmountPayed,
    std::string&        outRepeatMode,
    std::string&        outStReturn,
    TPosDateTime&       outPayDateTime,
    TPosDateTime&       outFulFilledDateTime,
    eUniKioskStatus&		outUniKioskStatus
  );

  //----------------------------------------------------------------
  int GetIntegTransactionData(
    std::string					inIdBuyer,
    std::string					inCertificateNumber,
    std::string					inIdService,

    std::string& outIdComBill,
    eTransactionStatus& outTransactionStatus,
    eMillAcceptStatus& outMillRqStatus,
    std::string& outAmountPayed,
    std::string& outRepeatMode,
    std::string& outStReturn,
    TPosDateTime& outPayDateTime,
    TPosDateTime& outFulFilledDateTime,
    eUniKioskStatus& outUniKioskStatus,
    std::string& outMsgError
  );

  //----------------------------------------------------------------
  // Отменить операцию в базе данных
  // Вход
  // 		inIdBuyer - Номер покупателя
  //    inCertificateNumber - номер сертификата
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
  //    outIsReturnMoney - возвращать деньги по молотилке или нет
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int StornoIntegratorBill (	
    std::string					inIdBuyer,
    std::string					inCertificateNumber,

    std::string					&outIdComBill, 
    eUniKioskStatus			&outStornoStatus, 
    std::string					&outStornoAmount,
    bool                &outIsReturnMoney
  );
  //----------------------------------------------------------------
  int StornoIntegratorBill(
    std::string					inIdBuyer,
    std::string					inCertificateNumber,

    std::string& outIdComBill,
    eUniKioskStatus& outStornoStatus,
    std::string& outStornoAmount,
    bool& outIsReturnMoney,
    std::string& outMsgError
  );

  //----------------------------------------------------------------
  // Сохранить счетчики
  // Вход
  //     inIdComBill - номер счета в бд
  //     inidCounter - номер счетчика
  //     inIdService - номер сервиса
  //     inIdClient - номер клиента
  //     inNameCounter - название счетчика
  //     inPrevCounter - предыдущие показания
  //     inCurrCounter - текущие показания
  //     inUsedCounter - использовано
  // Выход
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int StoreCounters (
    std::string	        inIdComBill,
    int                 inidCounter,
    std::string	        inIdService,
    std::string	        inIdClient,
    std::string	        inNameCounter,
    std::string	        inPrevCounter,
    std::string	        inCurrCounter,
    std::string	        inUsedCounter
    );

  //----------------------------------------------------------------
  // Сохранить период
  // Вход
  //     inIdComBill - номер счета в бд
  //     periodID - номер периода
  //     periodMode - режим периода
  //     periodBegin - начало периода
  //     periodEnd - конец периода
  // Выход
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int StorePeriod (
    std::string	        inIdComBill,
    std::string					inPeriodID,
    std::string					inPeriodMode,
    TPosDateTime			  inPeriodBegin,
    TPosDateTime        inPeriodEnd
    );

  //----------------------------------------------------------------
  // Сохранить информацию от клиента
  // Вход
  //     inIdComBill - номер счета в бд
  //     tag - данные тага
  // Выход
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int StoreInfoFromClient (
    std::string	        inIdComBill,
    TagInfoFromClient	tagList
    );

  //----------------------------------------------------------------
  // Сохранить список тагов
  // Вход
  //     inIdComBill - номер счета в бд
  //     idService - номер сервиса
  //     isChequeOwner - чек выдает предметный сервер
  //     isTagCounter - есть таг счетчика
  //     isTagPeriod - есть таг периода
  //     isTagInfoFromClient - есть таг информации от клиента
  //     isTagPaySubSrvFix - есть таг оплаты по постоянным услугам
  //     isTagPaySubSrvCount - есть таг оплаты услуги со счетчиками
  //     isTagPayDept - есть таг оплаты долга
  // Выход
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int StorePayTagList (
    std::string                 idComBill,
    std::string                 idService,
    bool							          isChequeOwner,
    bool							          isTagCounter,
    bool							          isTagPeriod,
    bool							          isTagInfoFromClient,
    bool							          isTagPaySubSrvFix,
    bool							          isTagPaySubSrvCount,
    bool							          isTagPayDept
    );

  //----------------------------------------------------------------
  // Прочитать конфигурацию сервиса
  // Вход
  //     idService - номер сервиса
  // Выход
  //      conf - настройки обработки для сервиса
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
//   int GetIntegServiceConfig (
//     std::string                 idService,
// 
//     S_INTER_CONFIG              &conf
//     );

  //----------------------------------------------------------------
  // Прочитать таги для запроса счета
  // Вход
  //     idService - номер сервиса
  //     idBuyer - номер байера
  // Выход
  //     billTagsForBill - список тагов для запроса счета
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
//   int GetServiceTagsBill (
//     std::string                 idService,
//     std::string                 idBuyer,
// 
//     TStdStringList	            &billTagsForBill
//     );

  //----------------------------------------------------------------
  // Прочитать таги для оплаты счета
  // Вход
  //     idService - номер сервиса
  //     idBuyer - номер байера
  // Выход
  //     billTagsForPay - список тагов для оплаты счета
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
//   int GetServiceTagsPay (
//     std::string                 idService,
//     std::string                 idBuyer,
// 
//     TStdStringList	            &billTagsForPay
//     );

  //----------------------------------------------------------------
  // Поставить статус транзакции в бд
  // Вход
  //    inIdComBill - номер транзакции в бд
  //		transactionStatus - статус транзакции
  // Выход
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int SetTransactionsCommStatus (	
    std::string					inIdComBill,
    eTransactionStatus  transactionStatus
    );

  //----------------------------------------------------------------
  // Запрос статуса транзакции
  // Вход
  //   inIdComBill - номер транзакции в бд
  // Выход
  //   UniKioskStatus - статус универсального киоска
  //   outIdComBill - номер транзакции в бд
  //   outIdService - номер сервиса
  //   outIdSubService - код услуги
  //   outIdClient - номер клиента
  //   outAmount - сумма
  //   outAmountCommis - комиссия
  //   outDateOperation - дата операции
  //   outIdCompany - код компании
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int IntegratorGetOperationState (	
    std::string					inIdComBill,

    eUniKioskStatus     &outUniKioskStatus,
    std::string					&outIdComBill,
    std::string					&outIdService,
    std::string					&outIdSubService,
    std::string					&outIdClient,
    std::string					&outAmount,
    std::string					&outAmountCommis,
    TPosDateTime        &outDateOperation,
    std::string					&outIdCompany
  );
  //----------------------------------------------------------------
  int IntegratorGetOperationState(
    std::string					inIdComBill,

    eUniKioskStatus& outUniKioskStatus,
    std::string& outIdComBill,
    std::string& outIdService,
    std::string& outIdSubService,
    std::string& outIdClient,
    std::string& outAmount,
    std::string& outAmountCommis,
    TPosDateTime& outDateOperation,
    std::string& outIdCompany,
    std::string& outMsgError
  );

  //----------------------------------------------------------------
  // Вернуть транзакции списком
  // Вход
  //		inPeriodBegin - начало периода
  //		inPeriodBegin - конец периода
  //    listServices - список сервисов
  // Выход
  //		operations - перечень операций
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
//   int GetTransactionList (	
//     TPosDateTime        inPeriodBegin,
//     TPosDateTime        inPeriodEnd,
//     TagListServices     listServices,
// 
//     TagOperations       &operations
//     );

  //----------------------------------------------------------------
  // Запрос статуса транзакции
  // Вход
  //  inAmountPayed - оплаченная сумма
  //  inAmountCommis - сумма комиссионных
  //  inIdBuyer - номер байера
  //  inExtReqNo - номер по молотилке
  //  inMillOperationId - номер операции по молотилке
  //  inMillAcceptStatus - результат акцептации по молотилке
  // Выход
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int AcceptMillIntegrator (	
      std::string					inAmountPayed,
      std::string					inAmountCommis,
      std::string					inIdBuyer,
      std::string					inExtReqNo,
      std::string					inMillOperationId,
      eMillAcceptStatus   inMillAcceptStatus
  );
  //----------------------------------------------------------------
  int AcceptMillIntegrator(
    std::string					inAmountPayed,
    std::string					inAmountCommis,
    std::string					inIdBuyer,
    std::string					inExtReqNo,
    std::string					inMillOperationId,
    eMillAcceptStatus   inMillAcceptStatus,

    std::string& outMsgError
  );


  //----------------------------------------------------------------
  // Возвращает размер лимитов для байера
  // Вход
  //		inKeyValue - идентификатор ключа
  // Выход
  //    outIdBuyer - номер байера
  //    outLimAccount - размер лимита на бейера
  //		outStLimit - статус использования лимита 1 - проверять лимит 0 - не проверять
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int getBuyerLimit (	
    std::string inKeyValue,

    std::string &outIdBuyer,
    std::string &outLimAccount,
    std::string &outStLimit
    );

  //----------------------------------------------------------------
	// Вернуть KMType - тип размер RSA ключа
	// Вход
	//		InIdBuyer - идентификатор Агента
	// Выход
	//		OutKMType - тип размер RSA ключа
	//		результат функции - результат выполнения операции
	//----------------------------------------------------------------
	int GetKMType (	
		std::string InIdBuyer,

		eKMType	   &OutKMType
	);

  //----------------------------------------------------------------
  // Поставить статус транзакции в бд
  // Вход
  //    inIdComBill - номер транзакции в бд
  //		inDateOp    - Облікова дата операції
  // Выход
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int SetTransactionsCommDateOp (	
    std::string	 inIdComBill,
    TPosDateTime inDateOp
    );

  //----------------------------------------------------------------
  // Поставить статус транзакции в бд
  // Вход
  //    inIdComBill - номер транзакции в бд
  //		transactionStatus - статус транзакции
  // Выход
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int ag_IntegSetTransactionCommStatus (	
    std::string					inIdComBill,
    TPosDateTime        inDateOp,
    eTransactionStatus  transactionStatus
  );

  //----------------------------------------------------------------
  // Додати запис до таблиці payowner. Якщо запис присутній, то нічого не робимо
  // Вход
  //    inIdComBill - номер транзакции в бд
  //    inIdbuyer 
  //    inIdservice
  //    inPaydate 
  //    inPperson
  //    inAddress, 
  //    inOther
  // Выход
  //		результат функции - результат выполнения операции
  //----------------------------------------------------------------
  int ag_AddToPayOwner (	
    std::string	 inIdComBill,
    std::string	 inIdBuyer,
    std::string	 inIdService,
    TPosDateTime inPayDate,
    std::string	 inPersone,
    std::string	 inAddress,
    std::string	 inOther
    );
  //----------------------------------------------------------------
  int ag_AddToPayOwner (	
    std::string	 inIdComBill,
    std::string	 inIdBuyer,
    std::string	 inIdService,
    std::string	 inAmount,
    TPosDateTime inPayDate,
    std::string	 inPersone,
    std::string	 inAddress,
    std::string	 inOther,
    int          inPayType,
    std::string	 inIdPayFund
    );
  //----------------------------------------------------------------
  int ag_AddToPayOwner(
    std::string	 inIdComBill,
    std::string	 inIdBuyer,
    std::string	 inIdService,
    std::string	 inAmount,
    TPosDateTime inPayDate,
    std::string	 inPersone,
    std::string	 inAddress,
    std::string	 inOther,
    int          inPayType,
    std::string	 inIdPayFund,
    
    std::string&	 outMsgError
  );

  //----------------------------------------------------------------
  // Вичитати інформацію про банківський платіж
  // Вход
  //    inIdComBill - Унікальний номер транзакції в системі Банку  або “*”
  //    inExtReqNo  - нікальний код операції в системі Партнера. Формується Партнером. або “*” 
  //    inIdbuyer   - Id Агента
  //    inIdservice - Id послуги
  // Выход
  //		результат функции - результат выполнения операции
  //    outIpDataList - список
  //----------------------------------------------------------------
  int ag_GetTagIPValues(
    std::string inIdComBill,
    std::string inExtReqNo,
    std::string inIdbuyer,
    std::string inIdService,
    
    TIpDataList& outIpDataList
  );

  /// <summary>
  /// Отримати код смс та номер телефону для певного статусу. Якщо дані не отримані, то надати помилку вибірки.
  /// </summary>
  /// <param name="inIdComBill">Ідентифікатор операції</param>
  /// <param name="inStatus">Статус коду: 1 - записано телефон для відправки СМС, 2 - записан код для відправки в смс, 3 - смс відправлена, 9 - відміна. код від партнера не приймати, 10 - отриман код в операції. Все Ок, співпало. </param>
  /// <param name="outCodeSend">Отриманий з БД код для надсилання клієнту</param>
  /// <param name="outPhone">Отриманий з БД номер телефону</param>
  /// <returns>Результат виконання методу</returns>
  int GetSmsParamsByStatus(std::string inIdComBill, int inStatus, std::string& outCodeSend, std::string& outPhone);

  /// <summary>
  /// Отримати статус, код смс та номер телефону за ідентифікатором операції IdComBill. Якщо дані не отримані, то надати помилку вибірки.
  /// </summary>
  /// <param name="inIdComBill">Ідентифікатор операції</param>
  /// <param name="outStatus">Статус коду: 1 - записано телефон для відправки СМС, 2 - записан код для відправки в смс, 3 - смс відправлена, 9 - відміна. код від партнера не приймати, 10 - отриман код в операції. Все Ок, співпало. </param>
  /// <param name="outCodeSend">Отриманий з БД код для надсилання клієнту</param>
  /// <param name="outPhone">Отриманий з БД номер телефону</param>
  /// <param name="outIpPort">IP та PORT для обміну з сервером надсилання дзвінків клієнтові</param>
  /// <returns>Результат виконання методу</returns>
  int GetSmsParams(std::string inIdComBill, int& outStatus, std::string& outCodeSend, std::string& outPhone, std::string& outIpPort);

  /// <summary>
  /// Оновити статус смс у SMS_CONTROL
  /// </summary>
  /// <param name="inIdComBill">Ідентифікатор операції</param>
  /// <param name="inStatus">Статус коду: 1 - записано телефон для відправки СМС, 2 - записан код для відправки в смс, 3 - смс відправлена, 9 - відміна. код від партнера не приймати, 10 - отриман код в операції. Все Ок, співпало. </param>
  /// <returns>Результат виконання методу</returns>
  int UpdateSmsParamsByStatus(std::string inIdComBill, int inStatus);

  /// <summary>
  /// Додати до таблиці payowner_pf дані тага AWP або AWF
  /// </summary>
  /// <param name="inIdComBill">Ідентифікатор операції. Має значення 0 на валідації</param>
  /// <param name="inIdBuyer">Ідентифікатор Агента</param>
  /// <param name="inIdService">Ідентифікатор сервісу</param>
  /// <param name="inAmount">Сума операції</param>
  /// <param name="inPayDate">Дата сплати</param>
  /// <param name="inTagName">Назва тага AWP або AWF</param>
  /// <param name="intagAccOwnerPay">Дані Платника - особа яка виконує платіж/Платник фактичний - особа, грошові зобов'язання якої виконуються платником, шляхом здійснення переказу з рахунку платника.</param>
  /// <param name="outMsgError">Дані про помилку. Формується таг ERR.</param>
  /// <returns></returns>
  int Add_AWPAWF(
    std::string	 inIdComBill,
    std::string	 inIdBuyer,
    std::string	 inIdService,
    std::string	 inAmount,
    TPosDateTime inPayDate,
    std::string	 inTagName,
    S_tagAccOwnerPay intagAccOwnerPay,

    eUniKioskStatus& outUniKioskStatus,
    std::string&	   outMsgError
  );

};

// Автоматический указатель на класс с автоматическим освобождением блокировки
typedef	std::auto_ptr<TGivsDatabase>					autoGivsDatabase;

#endif // __UKR_TELE_DATABASE_H__
