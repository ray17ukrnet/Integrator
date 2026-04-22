#ifndef __MILL_EXT_ACCEPTOR_H__
#define __MILL_EXT_ACCEPTOR_H__

// Парсинг запросов по универсальному ГИВЦу MILL
#include "UniKiosk/IntegratorParserBase.h"

//*****************************************************************************
// Класс парсера запросов для ГИВЦа без блокировок
//*****************************************************************************
class MillExtAcceptor : public IntegratorParserBase
{

public:

  //---------------------------------------------------------------------------
  // Конструктор
  //---------------------------------------------------------------------------
  MillExtAcceptor (
    );

  //---------------------------------------------------------------------------
  // Деструктор
  //---------------------------------------------------------------------------
  virtual ~MillExtAcceptor ();

protected:

  // Текущий период счета
  std::string                 Period;

  //---------------------------------------------------------------------------
  // Возвращает название оплаты для базы данных
  //---------------------------------------------------------------------------
  virtual std::string GetBillName (void);
  /*
  //---------------------------------------------------------------------------
  // Вернуть текущий период счета
  //---------------------------------------------------------------------------
  virtual std::string GetBillPeriod (void);

  //---------------------------------------------------------------------------
  // Вернуть начальный период для записи счета
  //---------------------------------------------------------------------------
  TPosDateTime getPeriodBegin (void);

  //---------------------------------------------------------------------------
  // Вернуть конечный период для записи счета
  //---------------------------------------------------------------------------
  TPosDateTime getPeriodEnd (void);

  //---------------------------------------------------------------------------
  // Вернуть номер для акцепта по молотилке
  //---------------------------------------------------------------------------
  virtual std::string GetMillVoucher (std::string IdRec);

  //---------------------------------------------------------------------------
  // Оплатить счет в базе данных
  // Вход
  //		MillReturnGuard - класс для автоматического возврата ваучеров
  //    Bills - перечень счетов 
  //    givsServicePay - счет для оплаты сервиса
  // Выход
  //	результат функции - результат выполнения операции
  //---------------------------------------------------------------------------
  virtual int PayService (
    autoMillReturnGuard         &MillReturnGuard,
    TMakeevkaBillList           Bills,
    S_GIVS_SERVICE_PAY_EXT      givsServicePay
    );

  //---------------------------------------------------------------------------
  // Оплатить счетчик в базе данных
  // Вход
  //		MillReturnGuard - класс для автоматического возврата ваучеров
  //    Bills - перечень счетов 
  //    givsCounterPay - счет для оплаты счетчика
  // Выход
  //	результат функции - результат выполнения операции
  //---------------------------------------------------------------------------
  virtual int PayCounter (
    autoMillReturnGuard         &MillReturnGuard,
    TMakeevkaBillList           Bills,
    S_GIVS_COUNTER_PAY_EXT      givsCounterPay
    );

  //---------------------------------------------------------------------------
  // Определить сервис по которому оплачивать деньги сверх выставленного счета
  // Вход
  //    Bills - перечень счетов 
  // Выход
  //    billRec - счет по которому оплачивать переплату
  //	результат функции - результат выполнения операции
  //---------------------------------------------------------------------------
  virtual int GetOverPayService (
//    TMakeevkaBillList           Bills,

    S_MAKEEVKA_BILL_REC         &billRec
    );

  //----------------------------------------------------------------------
  // Вернуть текущую версию телеграммы
  // Вход
  //		TelType - Тип запроса
  // Выход
  //    TelVersion - версия телеграммы
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  virtual int GetTelVersion (
    eUniKioskClientRequest	      TelType,
    eUniKioskClientRequestVersion &TelVersion
    );
  */

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
  int execInternalServRequest (
    std::string                 inIdService,
    std::string                 inIdSubService,
    TWebRequestList             requestList,

    TWebRequestList	            &responseList
    );

  //----------------------------------------------------------------------
  // Создать значение запроса по умолчанию
  //----------------------------------------------------------------------
  std::string MillExtAcceptor::CreateDefaultValue (
    std::string				  value
    );

  //----------------------------------------------------------------------
  // Запросить счет
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  int GetBillProvider (
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
                                        );

  //----------------------------------------------------------------------
  // Оплатить счет
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  int PayBillProvider (
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
                                        );

  //----------------------------------------------------------------------
  // Сделать сторно по счету
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
//   virtual int StornoBillProvider (
//     std::string					inIdService,
//     std::string					inIdSubService,
//     std::string					inIdClient,
//     std::string					inIdComBill,
//     std::string					inAmountPayed,
//     std::string					inIdTerminal,
//     std::string					inIdBuyer,
// 
//     eUniKioskStatus			&uniKioskStatus,
//     std::string					&outProviderMessage
//                                         );

  //----------------------------------------------------------------------
  // Обработать полученный запрос счета
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  virtual int AcceptGetBill (void);

  //----------------------------------------------------------------------
  // Обработать полученный запрос оплаты счета по ваучерному интерфейсу
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
//   virtual int AcceptPayBillDang (void);

  //----------------------------------------------------------------------
  // Обработать полученный запрос оплаты счета по молотильному интерфейсу
  // Вход
  //		
  // Выход
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  virtual int AcceptPayBillMill (void);

  //----------------------------------------------------------------------
  // Отменить указанную операцию в бд
  // Вход
  // 		inIdBuyer - Номер покупателя
  // 		inIdService - номер сервиса
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
  //    isReturnMoney - вовзвращать деньги или нет
  //    outIdService - номер сервиса реальный
  //	результат функции - результат выполнения операции
  //----------------------------------------------------------------------
  int Storno (
    std::string					inIdBuyer,
    std::string					inIdService,
    std::string					inCertificateNumber,

    std::string					&outIdComBill, 
    eUniKioskStatus			&outStornoStatus,
    std::string					&outStornoAmount,
    bool                &isReturnMoney
    );

  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&


private:


};

// Автоматический указатель
typedef std::auto_ptr<MillExtAcceptor>        autoMillExtAcceptor;


#endif // __MILL_EXT_ACCEPTOR_H__
