#include "integrator_pch.h"

#include "IntegratorDatabase.h"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Этот клас предназначен для работы с базой данных акциптированных
// ваучеров и проданного товара
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

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
														 ) 

														 : TTradeDatabase(
																aDatabaseHost, 
																aDatabasePort,
																aDatabaseName,
																aDatabaseLogin,
																aDatabasePassword
																)
{
}

//----------------------------------------------------------------
// Деструктор
//----------------------------------------------------------------
TGivsDatabase::~TGivsDatabase()
{
}

//----------------------------------------------------------------
// Возвращает номер сервиса для указанного ваучера
// Вход
//		inIdBuyer - номер байера
//		inExtReqNo - номер по молотилке
// Выход
//		outIdService - номер сервиса
//		результат функции - результат выполнения операции
//----------------------------------------------------------------
int TGivsDatabase::getIdServiceByMill (	
																	 std::string inIdBuyer,
																	 std::string inExtReqNo,

                                   std::string &outIdService
																	 )
{
	TID										ClassID = tidDATABASE;
	std::string						ProcedureName = "TGivsDatabase::getIdServiceByMill : ";
	int										Result = OK;

	std::string						SQLProcedureName = "getIdServiceByMill";
	msstream							SQL;
	TSQLResults						SQLResults;
// 	std::string						StValue;
	//	int										intValue;

// 	Inform(MS 
// 		<< SQLProcedureName
// 		<< " in parametr"
// 		<< " inIdBuyer = " << inIdBuyer
// 		<< " inExtReqNo = " << inExtReqNo
// 		, ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
		<< " inIdBuyer = " << inIdBuyer
		<< " inExtReqNo = " << inExtReqNo
    );

	// Формируем текст запроса
	SQL << CLEAR;
	SQL << "select t.Service from TransactionsComm t, MillTx m where t.IdComBill = m.IdComBill "
		<< " and m.IdBuyer = " << inIdBuyer
		<< " and m.ExtReqNo = " << ToSQLString(inExtReqNo)
		;

	// Выполняем запрос
	Result = ExecSQLRequest (	
		SQL.str(),
		SQLResults
		);

	CHECK_ERROR_RETURN(Result);

	// Если отсутстуют данные
	if (SQLResults.size() == 0) {
		outIdService = "-1";
	}
	else {
		// Вычитываем данные запроса
		Result = GetField (	
			SQLResults, 0, 0, outIdService);

		CHECK_ERROR_RETURN(Result);
	}

// 	Inform(MS 
// 		<< SQLProcedureName
// 		<< " output parametr"
// 		<< " outIdService = " << outIdService
// 		, ClassID, mtMessage); 

  LogOutputParametr (SQLProcedureName, MS
		<< " outIdService = " << outIdService
    );

	return Result;
}

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
int TGivsDatabase::IntegProcess111Stage1 (	
  std::string					inIdService,
  std::string					inIdSubService,
  std::string					inIdClient,
  std::string					inIdTerm,
  std::string					inKeyValue,

  eUniKioskStatus     &UniKioskStatus,
  std::string					&outIdBuyer
//   std::string					&outChequeText
                                    )
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::IntegProcess111Stage1 : ";
  int										Result = OK;

  std::string						SQLProcedureName = "IntegProcess111Stage1";
  msstream							SQL;
  int										EstimatedResultCount = 4;
  TSQLResults						SQLResults;
  int										Col;
  std::string						StValue;
  std::string						stCounterFlag;
  std::string						newIdComBill;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " inIdService = " << inIdService
//     << " inIdSubService = " << inIdSubService
//     << " inIdClient = " << inIdClient
//     << " inIdTerm = " << inIdTerm
//     << " inKeyValue = " << inKeyValue
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
    << " inIdService = " << inIdService
    << " inIdSubService = " << inIdSubService
    << " inIdClient = " << inIdClient
    << " inIdTerm = " << inIdTerm
    << " inKeyValue = " << inKeyValue
    );

// CREATE PROCEDURE IntegProcess111Stage1
// (
//  IN inIdService     INT,
//  IN inIdSubService  INT,
//  IN inIdClient      VARCHAR(20),
//  IN inIdTerm        VARCHAR(50),
//  IN inKeyValue      VARCHAR(12)
//  )

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "call "
    << SQLProcedureName
    << "("
    << " " << inIdService
    << "," << inIdSubService
    << "," << ToSQLString(inIdClient)
    << "," << ToSQLString(inIdTerm)
    << "," << ToSQLString(inKeyValue)
    << ")";

  Result = ExecStoredProcedure (	
    SQL.str(),
    EstimatedResultCount,
    SQLResults
    );

  CHECK_ERROR_RETURN(Result);

//   SELECT exec_result
//        , error_msg
// 
//        , UniKioskStatus
//        , outIdBuyer
//   ;

	// Вычитываем данные запроса
	Col = 2;

  Result = GetField (SQLResults, 
    0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

  Result = GetField (SQLResults, 
    0, Col++, outIdBuyer);
  CHECK_ERROR_RETURN(Result);
  
//   Result = GetField (SQLResults, 
//     0, Col++, outChequeText);
//   CHECK_ERROR_RETURN(Result);

//   Inform(MS 
//     << SQLProcedureName 
//     << " output parametr"
//     << " UniKioskStatus = " << UniKioskStatus
//     << " outIdBuyer = " << outIdBuyer
// //     << " outChequeText = " << outChequeText
//     , ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS
    << " UniKioskStatus = " << UniKioskStatus
    << " outIdBuyer = " << outIdBuyer
//     << " outChequeText = " << outChequeText
    );

  return Result;
}
//----------------------------------------------------------------
int TGivsDatabase::IntegProcess111Stage1(
  std::string	inIdService,
  std::string	inIdSubService,
  std::string	inIdClient,
  std::string	inIdTerm,
  std::string inKeyValue,

  eUniKioskStatus& UniKioskStatus,
  std::string&     outIdBuyer,
  std::string&     outMsgError
)
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::IntegProcess111Stage1 : ";
  int										Result = OK;

  std::string						SQLProcedureName = "IntegProcess111Stage1";
  msstream							SQL;
  int										EstimatedResultCount = 4;
  TSQLResults						SQLResults;
  int										Col;
  std::string						StValue;
  std::string						stCounterFlag;
  std::string						newIdComBill;

  //   Inform(MS 
  //     << SQLProcedureName
  //     << " in parametr"
  //     << " inIdService = " << inIdService
  //     << " inIdSubService = " << inIdSubService
  //     << " inIdClient = " << inIdClient
  //     << " inIdTerm = " << inIdTerm
  //     << " inKeyValue = " << inKeyValue
  //     , ClassID, mtMessage);

  LogInputParametr(SQLProcedureName, MS
    << " inIdService = " << inIdService
    << " inIdSubService = " << inIdSubService
    << " inIdClient = " << inIdClient
    << " inIdTerm = " << inIdTerm
    << " inKeyValue = " << inKeyValue
  );

  // CREATE PROCEDURE IntegProcess111Stage1
  // (
  //  IN inIdService     INT,
  //  IN inIdSubService  INT,
  //  IN inIdClient      VARCHAR(20),
  //  IN inIdTerm        VARCHAR(50),
  //  IN inKeyValue      VARCHAR(12)
  //  )

    // Формируем текст запроса
  SQL << CLEAR;
  SQL << "CALL "
    << SQLProcedureName
    << "("
    << " " << inIdService
    << "," << inIdSubService
    << "," << ToSQLString(inIdClient)
    << "," << ToSQLString(inIdTerm)
    << "," << ToSQLString(inKeyValue)
    << "); ";

  Result = ExecStoredProcedure(
    SQL.str(),
    EstimatedResultCount,
    SQLResults
  );

  CHECK_ERROR_RETURN(Result);

  //   SELECT exec_result
  //        , error_msg
  // 
  //        , UniKioskStatus
  //        , outIdBuyer
  //   ;

    // Вычитываем данные запроса
  Col = 1;

  Result = GetField(SQLResults,  0, Col++, outMsgError);
  CHECK_ERROR_RETURN(Result);


  Result = GetField(SQLResults,  0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

  Result = GetField(SQLResults, 0, Col++, outIdBuyer);
  CHECK_ERROR_RETURN(Result);

  //   Result = GetField (SQLResults, 
  //     0, Col++, outChequeText);
  //   CHECK_ERROR_RETURN(Result);

  //   Inform(MS 
  //     << SQLProcedureName 
  //     << " output parametr"
  //     << " UniKioskStatus = " << UniKioskStatus
  //     << " outIdBuyer = " << outIdBuyer
  // //     << " outChequeText = " << outChequeText
  //     , ClassID, mtMessage);

  LogOutputParametr(SQLProcedureName, MS
    << " UniKioskStatus = " << UniKioskStatus
    << " outIdBuyer = "     << outIdBuyer
    << " outMsgError = "    << outMsgError
  );

  return Result;
}

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
int TGivsDatabase::IntegProcess111Stage0_v1 (	
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
)
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::IntegProcess111Stage0_v1 : ";
  int										Result = OK;

  std::string						SQLProcedureName = "IntegProcess111Stage0_v1";
  msstream							SQL;
  int										EstimatedResultCount = 3;
  TSQLResults						SQLResults;
  int										Col;
  std::string						StValue;
  std::string						stCounterFlag;
  std::string						newIdComBill;

  LogInputParametr (SQLProcedureName, MS
    << ": TagType = "    << ToSQLString(inTagType)
    << ", AWperson = "   << (inAWperson.empty()  ? ToSQLString("*") : ToSQLString(inAWperson))
    << ", AWaddress = "  << (inAWaddress.empty() ? ToSQLString("*") : ToSQLString(inAWaddress))
    << ", AWother = "    << (inAWother.empty()   ? ToSQLString("*") : ToSQLString(inAWother))
    << ", inIdSrv = "    << inIdService
    << ", inIdBr = "     << inIdBuyer
    << ", inIdClient = " << ToSQLString(inIdClient)
    << ", inIdTerm = "   << ToSQLString(inIdTerm)
    << ", inAmount = "   << inAmount
    );
    
  //IN tagtype varchar(5),
  //IN AWperson varchar(128),
  //IN AWaddress varchar(128),
  //IN AWother   varchar(255),
  //IN inIdSrv   int,
  //IN inIdBr    int,
  //IN inIdClient varchar(128),
  //IN inIdTern   varchar(40),
  //IN inAmount   numeric(12,2)

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "call "
    << SQLProcedureName
    << "( "
            << ToSQLString(inTagType)
    << ", " << (inAWperson.empty()  ? ToSQLString("*") : ToSQLString(inAWperson))
    << ", " << (inAWaddress.empty() ? ToSQLString("*") : ToSQLString(inAWaddress))
    << ", " << (inAWother.empty()   ? ToSQLString("*") : ToSQLString(inAWother))
    << ", " << inIdService
    << ", " << inIdBuyer
    << ", " << ToSQLString(inIdClient)
    << ", " << ToSQLString(inIdTerm)
    << ", " << inAmount
    << " )";

  Result = ExecStoredProcedure ( SQL.str(), EstimatedResultCount, SQLResults );

  CHECK_ERROR_RETURN(Result);

//   SELECT exec_result
//        , UniKioskStatus
//        , error_msg
//   ;

	// Вычитываем данные запроса
	Col = 1;
  // UniKioskStatus
  Result = GetField (SQLResults,  0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniKioskStatus = (eUniKioskStatus)StringToInt(StValue);
  // error_msg
  Result = GetField (SQLResults,  0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);
  

//   Inform(MS 
//     << SQLProcedureName 
//     << " output parametr"
//     << " UniKioskStatus = " << UniKioskStatus
//     << " outIdBuyer = " << outIdBuyer
// //     << " outChequeText = " << outChequeText
//     , ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS
    << ": UniKioskStatus = " << UniKioskStatus
    << ", error_msg = " << ((StValue.empty())? "NULL": StValue )
  );

  return Result;
}
  //----------------------------------------------------------------
  int TGivsDatabase::IntegProcess111Stage0_v1(
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
  )
  {
    TID										ClassID = tidDATABASE;
    std::string						ProcedureName = "TGivsDatabase::IntegProcess111Stage0_v1 : ";
    int										Result = OK;

    std::string						SQLProcedureName = "IntegProcess111Stage0_v1";
    msstream							SQL;
    int										EstimatedResultCount = 3;
    TSQLResults						SQLResults;
    int										Col;
    std::string						StValue;
    std::string						stCounterFlag;
    std::string						newIdComBill;

    LogInputParametr(SQLProcedureName, MS
      << ": TagType = " << ToSQLString(inTagType)
      << ", AWperson = " << (inAWperson.empty() ? ToSQLString("*") : ToSQLString(inAWperson))
      << ", AWaddress = " << (inAWaddress.empty() ? ToSQLString("*") : ToSQLString(inAWaddress))
      << ", AWother = " << (inAWother.empty() ? ToSQLString("*") : ToSQLString(inAWother))
      << ", inIdSrv = " << inIdService
      << ", inIdBr = " << inIdBuyer
      << ", inIdClient = " << ToSQLString(inIdClient)
      << ", inIdTerm = " << ToSQLString(inIdTerm)
      << ", inAmount = " << inAmount
    );

    //IN tagtype varchar(5),
    //IN AWperson varchar(128),
    //IN AWaddress varchar(128),
    //IN AWother   varchar(255),
    //IN inIdSrv   int,
    //IN inIdBr    int,
    //IN inIdClient varchar(128),
    //IN inIdTern   varchar(40),
    //IN inAmount   numeric(12,2)

    // Формируем текст запроса
    SQL << CLEAR;
    SQL << "CALL "
      << SQLProcedureName
      << "( "
      << ToSQLString(inTagType)
      << ", " << (inAWperson.empty() ? ToSQLString("*") : ToSQLString(inAWperson))
      << ", " << (inAWaddress.empty() ? ToSQLString("*") : ToSQLString(inAWaddress))
      << ", " << (inAWother.empty() ? ToSQLString("*") : ToSQLString(inAWother))
      << ", " << inIdService
      << ", " << inIdBuyer
      << ", " << ToSQLString(inIdClient)
      << ", " << ToSQLString(inIdTerm)
      << ", " << inAmount
      << " ); ";

    Result = ExecStoredProcedure(SQL.str(), EstimatedResultCount, SQLResults);

    CHECK_ERROR_RETURN(Result);

    //   SELECT exec_result
    //        , UniKioskStatus
    //        , error_msg
    //   ;

      // Вычитываем данные запроса
    Col = 1;
    // UniKioskStatus
    Result = GetField(SQLResults, 0, Col++, StValue);
    CHECK_ERROR_RETURN(Result);

    UniKioskStatus = (eUniKioskStatus)StringToInt(StValue);
    // error_msg
    Result = GetField(SQLResults, 0, Col++, StValue);
    CHECK_ERROR_RETURN(Result);

    outMsgError = StValue;


    //   Inform(MS 
    //     << SQLProcedureName 
    //     << " output parametr"
    //     << " UniKioskStatus = " << UniKioskStatus
    //     << " outIdBuyer = " << outIdBuyer
    // //     << " outChequeText = " << outChequeText
    //     , ClassID, mtMessage);

    LogOutputParametr(SQLProcedureName, MS
      << ": UniKioskStatus = " << UniKioskStatus
      << ", error_msg = " << ((StValue.empty()) ? "NULL" : StValue)
    );

    return Result;
  }

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
int TGivsDatabase::IntegProcess111Stage2 (	
  std::string					inIdService,
  std::string					inIdSubService,
  std::string					inIdClient,
  std::string					inIdTerm,
  std::string					inKeyValue,
  eUniKioskStatus     inProviderUniKioskStatus,

  eUniKioskStatus     &UniKioskStatus
//   std::string					&outChequeText
                                    )
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::IntegProcess111Stage2 : ";
  int										Result = OK;

  std::string						SQLProcedureName = "IntegProcess111Stage2";
  msstream							SQL;
  int										EstimatedResultCount = 3;
  TSQLResults						SQLResults;
  int										Col;
  std::string						StValue;
  std::string						stCounterFlag;
  std::string						newIdComBill;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " inIdService = " << inIdService
//     << " inIdSubService = " << inIdSubService
//     << " inIdClient = " << inIdClient
//     << " inIdTerm = " << inIdTerm
//     << " inKeyValue = " << inKeyValue
//     << " inProviderUniKioskStatus = " << inProviderUniKioskStatus
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
    << " inIdService = " << inIdService
    << " inIdSubService = " << inIdSubService
    << " inIdClient = " << inIdClient
    << " inIdTerm = " << inIdTerm
    << " inKeyValue = " << inKeyValue
    << " inProviderUniKioskStatus = " << inProviderUniKioskStatus
    );

// CREATE PROCEDURE IntegProcess111Stage1
// (
//  IN inIdService     INT,
//  IN inIdSubService  INT,
//  IN inIdClient      VARCHAR(20),
//  IN inIdTerm        VARCHAR(50),
//  IN inKeyValue      VARCHAR(12),
//     IN inProviderUnikioskStatus int
//  )

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "call "
    << SQLProcedureName
    << "("
    << " " << inIdService
    << "," << inIdSubService
    << "," << ToSQLString(inIdClient)
    << "," << ToSQLString(inIdTerm)
    << "," << ToSQLString(inKeyValue)
    << "," << inProviderUniKioskStatus
    << ")";

  Result = ExecStoredProcedure (	
    SQL.str(),
    EstimatedResultCount,
    SQLResults
    );

  CHECK_ERROR_RETURN(Result);

//   SELECT exec_result
//        , error_msg
// 
//        , UniKioskStatus
//        , outChequeText
//   ;

	// Вычитываем данные запроса
	Col = 2;

  Result = GetField (SQLResults, 
    0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

//   Result = GetField (SQLResults, 
//     0, Col++, outChequeText);
//   CHECK_ERROR_RETURN(Result);

//   Inform(MS 
//     << SQLProcedureName 
//     << " output parametr"
//     << " UniKioskStatus = " << UniKioskStatus
// //     << " outChequeText = " << outChequeText
//     , ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS
    << " UniKioskStatus = " << UniKioskStatus
//     << " outChequeText = " << outChequeText
    );

  return Result;
}
//----------------------------------------------------------------
int TGivsDatabase::IntegProcess111Stage2 (	
    std::string					inIdService,
    std::string					inIdSubService,
    std::string					inIdClient,
    std::string					inIdTerm,
    std::string					inKeyValue,
    eUniKioskStatus     inProviderUniKioskStatus,

    eUniKioskStatus     &UniKioskStatus,
    std::string& outMsgError
  )
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::IntegProcess111Stage2 : ";
  int										Result = OK;

  std::string						SQLProcedureName = "IntegProcess111Stage2";
  msstream							SQL;
  int										EstimatedResultCount = 3;
  TSQLResults						SQLResults;
  int										Col;
  std::string						StValue;
  std::string						stCounterFlag;
  std::string						newIdComBill;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " inIdService = " << inIdService
//     << " inIdSubService = " << inIdSubService
//     << " inIdClient = " << inIdClient
//     << " inIdTerm = " << inIdTerm
//     << " inKeyValue = " << inKeyValue
//     << " inProviderUniKioskStatus = " << inProviderUniKioskStatus
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
    << " inIdService = " << inIdService
    << " inIdSubService = " << inIdSubService
    << " inIdClient = " << inIdClient
    << " inIdTerm = " << inIdTerm
    << " inKeyValue = " << inKeyValue
    << " inProviderUniKioskStatus = " << inProviderUniKioskStatus
    );

// CREATE PROCEDURE IntegProcess111Stage1
// (
//  IN inIdService     INT,
//  IN inIdSubService  INT,
//  IN inIdClient      VARCHAR(20),
//  IN inIdTerm        VARCHAR(50),
//  IN inKeyValue      VARCHAR(12),
//     IN inProviderUnikioskStatus int
//  )

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "CALL "
    << SQLProcedureName
    << "("
    << " " << inIdService
    << "," << inIdSubService
    << "," << ToSQLString(inIdClient)
    << "," << ToSQLString(inIdTerm)
    << "," << ToSQLString(inKeyValue)
    << "," << inProviderUniKioskStatus
    << "); ";

  Result = ExecStoredProcedure (	
    SQL.str(),
    EstimatedResultCount,
    SQLResults
  );

  CHECK_ERROR_RETURN(Result);

//   SELECT exec_result
//        , error_msg
// 
//        , UniKioskStatus
//        , outChequeText
//   ;

	// Вычитываем данные запроса
	Col = 1;

  Result = GetField(SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  outMsgError = StValue;

  Result = GetField (SQLResults,  0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

//   Result = GetField (SQLResults, 
//     0, Col++, outChequeText);
//   CHECK_ERROR_RETURN(Result);

//   Inform(MS 
//     << SQLProcedureName 
//     << " output parametr"
//     << " UniKioskStatus = " << UniKioskStatus
// //     << " outChequeText = " << outChequeText
//     , ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS << " UniKioskStatus = " << UniKioskStatus << ", outMsgError = " << outMsgError );

  return Result;
}

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
int TGivsDatabase::IntegProcess113Stage1 (
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
)
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TUniBillDatabase::IntegProcess113Stage1 : ";
  int										Result = OK;

  std::string						SQLProcedureName = "IntegProcess113Stage1_v5";
  msstream							SQL;
  int										EstimatedResultCount = 13;
  TSQLResults						SQLResults;
  int										Col;
  std::string						StValue;
  std::string						stCounterFlag;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " inIdComBill = " << inIdComBill
//     << " inIdMillRq = " << inIdMillRq
//     << " inIdTransaction = " << inIdTransaction
//     << " inExtReqNo = " << inExtReqNo
//     << " inIdService = " << inIdService
//     << " inIdSubService = " << inIdSubService
//     << " inIdClient = " << inIdClient
//     << " inAmountPayed = " << inAmountPayed
//     << " inAmountCommis = " << inAmountCommis
//     << " inIdTerm = " << inIdTerm
//     << " inKeyValue = " << inKeyValue
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
    << " inIdComBill = " << inIdComBill
    << " inIdMillRq = " << inIdMillRq
    << " inIdTransaction = " << inIdTransaction
    << " inExtReqNo = " << inExtReqNo
    << " inIdService = " << inIdService
    << " inIdSubService = " << inIdSubService
    << " inIdClient = " << inIdClient
    << " inAmountPayed = " << inAmountPayed
    << " inAmountCommis = " << inAmountCommis
    << " inIdTerm = " << inIdTerm
    << " inKeyValue = " << inKeyValue
    );

// CREATE PROCEDURE IntegProcess113Stage1_v5
// (IN inIdComBill bigint,
// IN inIdMillRq bigint,
// IN inIdTransaction bigint,
// IN inExtReqNo varchar(40),
// IN inIdService int,
// IN inIdSubService int,
// IN inIdClient varchar(50),
// IN inAmountPayed numeric(10, 2),
// IN inAmountCommis numeric(10, 2),
// IN inIdTerm varchar(50),
// IN inKeyValue varchar(12))
// 
// -- version 5.0
// -- from 29.05.2015

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "call "
    << SQLProcedureName
    << "("
    << " " << inIdComBill
    << "," << inIdMillRq
    << "," << inIdTransaction
    << "," << ToSQLString(inExtReqNo)
    << "," << inIdService
    << "," << inIdSubService
    << "," << ToSQLString(inIdClient)
    << "," << inAmountPayed
    << "," << inAmountCommis
    << "," << ToSQLString(inIdTerm)
    << "," << ToSQLString(inKeyValue)
    << ")";

  Result = ExecStoredProcedure (	
    SQL.str(),
    EstimatedResultCount,
    SQLResults
    );

  CHECK_ERROR_RETURN(Result);

//   SELECT
//     exec_result,
//     error_msg,
// 
//     UniKioskStatus,
//     UniBillStatus,
//     outIdComBill,
//     outIdBuyer,
//     outChequeText,
//     outIdCompany,
//     isMillOvertaking,
//     isProviderOvertaking,
//     isSuccessLocked,
//     isAlwaysSendToProvider,
//     outPayDateTime
//   ;

	// Вычитываем данные запроса
	Col = 2;

  Result = GetField (SQLResults, 
    0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

  Result = GetField (SQLResults, 
    0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniBillStatus = (eUniBillStatus)StringToInt(StValue);

  Result = GetField (SQLResults, 
    0, Col++, outIdComBill);
  CHECK_ERROR_RETURN(Result);

  Result = GetField (SQLResults, 
    0, Col++, outIdBuyer);
  CHECK_ERROR_RETURN(Result);

  Result = GetField (SQLResults, 
    0, Col++, outChequeText);
  CHECK_ERROR_RETURN(Result);
  
  Result = GetField (SQLResults, 
    0, Col++, outIdCompany);
  CHECK_ERROR_RETURN(Result);

  Result = GetField (SQLResults, 
    0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  if (StValue == "1") {
    isMillOvertaking = true;
  } else {
    isMillOvertaking = false;
  }
  
  Result = GetField (SQLResults, 
    0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  if (StValue == "1") {
    isProviderOvertaking = true;
  } else {
    isProviderOvertaking = false;
  }

  Result = GetField (SQLResults, 
    0, Col++, isSuccessLocked);
  CHECK_ERROR_RETURN(Result);

  Result = GetField (SQLResults, 
    0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  if (StValue == "1") {
    isAlwaysSendToProvider = true;
  } else {
    isAlwaysSendToProvider = false;
  }

  Result = GetField (SQLResults, 
    0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  Result = MySQLDateTime_To_TPosDateTime(StValue, outPayDateTime);
  CHECK_ERROR_RETURN(Result);

//   Inform(MS 
//     << SQLProcedureName 
//     << " output parametr"
//     << " UniKioskStatus = " << UniKioskStatus
//     << " UniBillStatus = " << UniBillStatus
//     << " outIdComBill = " << outIdComBill
//     << " outIdBuyer = " << outIdBuyer
//     << " outChequeText = " << outChequeText
//     << " outIdCompany = " << outIdCompany
//     << " isMillOvertaking = " << isMillOvertaking
//     << " isProviderOvertaking = " << isProviderOvertaking
//     << " isSuccessLocked = " << isSuccessLocked
//     << " isAlwaysSendToProvider = " << isAlwaysSendToProvider
//     << " outPayDateTime = " << TPosDateTime_To_DateTimeString(outPayDateTime)
//     , ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS
    << " UniKioskStatus = " << UniKioskStatus
    << " UniBillStatus = " << UniBillStatus
    << " outIdComBill = " << outIdComBill
    << " outIdBuyer = " << outIdBuyer
    << " outChequeText = " << outChequeText
    << " outIdCompany = " << outIdCompany
    << " isMillOvertaking = " << isMillOvertaking
    << " isProviderOvertaking = " << isProviderOvertaking
    << " isSuccessLocked = " << isSuccessLocked
    << " isAlwaysSendToProvider = " << isAlwaysSendToProvider
    << " outPayDateTime = " << TPosDateTime_To_DateTimeString(outPayDateTime)
    );

  return Result;
}

//----------------------------------------------------------------
int TGivsDatabase::IntegProcess113Stage1(
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
)
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TUniBillDatabase::IntegProcess113Stage1 : ";
  int										Result = OK;

  std::string						SQLProcedureName = "IntegProcess113Stage1_v5";
  msstream							SQL;
  int										EstimatedResultCount = 13;
  TSQLResults						SQLResults;
  int										Col;
  std::string						StValue;
  std::string						stCounterFlag;

  //   Inform(MS 
  //     << SQLProcedureName
  //     << " in parametr"
  //     << " inIdComBill = " << inIdComBill
  //     << " inIdMillRq = " << inIdMillRq
  //     << " inIdTransaction = " << inIdTransaction
  //     << " inExtReqNo = " << inExtReqNo
  //     << " inIdService = " << inIdService
  //     << " inIdSubService = " << inIdSubService
  //     << " inIdClient = " << inIdClient
  //     << " inAmountPayed = " << inAmountPayed
  //     << " inAmountCommis = " << inAmountCommis
  //     << " inIdTerm = " << inIdTerm
  //     << " inKeyValue = " << inKeyValue
  //     , ClassID, mtMessage);

  LogInputParametr(SQLProcedureName, MS
    << " inIdComBill = " << inIdComBill
    << " inIdMillRq = " << inIdMillRq
    << " inIdTransaction = " << inIdTransaction
    << " inExtReqNo = " << inExtReqNo
    << " inIdService = " << inIdService
    << " inIdSubService = " << inIdSubService
    << " inIdClient = " << inIdClient
    << " inAmountPayed = " << inAmountPayed
    << " inAmountCommis = " << inAmountCommis
    << " inIdTerm = " << inIdTerm
    << " inKeyValue = " << inKeyValue
  );

  // CREATE PROCEDURE IntegProcess113Stage1_v5
  // (IN inIdComBill bigint,
  // IN inIdMillRq bigint,
  // IN inIdTransaction bigint,
  // IN inExtReqNo varchar(40),
  // IN inIdService int,
  // IN inIdSubService int,
  // IN inIdClient varchar(50),
  // IN inAmountPayed numeric(10, 2),
  // IN inAmountCommis numeric(10, 2),
  // IN inIdTerm varchar(50),
  // IN inKeyValue varchar(12))
  // 
  // -- version 5.0
  // -- from 29.05.2015

    // Формируем текст запроса
  SQL << CLEAR;
  SQL << "CALL "
    << SQLProcedureName
    << "("
    << " " << inIdComBill
    << "," << inIdMillRq
    << "," << inIdTransaction
    << "," << ToSQLString(inExtReqNo)
    << "," << inIdService
    << "," << inIdSubService
    << "," << ToSQLString(inIdClient)
    << "," << inAmountPayed
    << "," << inAmountCommis
    << "," << ToSQLString(inIdTerm)
    << "," << ToSQLString(inKeyValue)
    << "); ";

  Result = ExecStoredProcedure(  SQL.str(), EstimatedResultCount,  SQLResults );
  CHECK_ERROR_RETURN(Result);

  //   SELECT
  //     exec_result,
  //     error_msg,
  // 
  //     UniKioskStatus,
  //     UniBillStatus,
  //     outIdComBill,
  //     outIdBuyer,
  //     outChequeText,
  //     outIdCompany,
  //     isMillOvertaking,
  //     isProviderOvertaking,
  //     isSuccessLocked,
  //     isAlwaysSendToProvider,
  //     outPayDateTime
  //   ;

    // Вычитываем данные запроса
  Col = 1;

  Result = GetField(SQLResults, 0, Col++, outMsgError);
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

  Result = GetField(SQLResults,  0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniBillStatus = (eUniBillStatus)StringToInt(StValue);

  Result = GetField(SQLResults,  0, Col++, outIdComBill);
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults,  0, Col++, outIdBuyer);
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults,  0, Col++, outChequeText);
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults,  0, Col++, outIdCompany);
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults,  0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  if (StValue == "1") {
    isMillOvertaking = true;
  }
  else {
    isMillOvertaking = false;
  }

  Result = GetField(SQLResults,  0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  if (StValue == "1") {
    isProviderOvertaking = true;
  }
  else {
    isProviderOvertaking = false;
  }

  Result = GetField(SQLResults,  0, Col++, isSuccessLocked);
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults,  0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  if (StValue == "1") {
    isAlwaysSendToProvider = true;
  }
  else {
    isAlwaysSendToProvider = false;
  }

  Result = GetField(SQLResults,  0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  Result = MySQLDateTime_To_TPosDateTime(StValue, outPayDateTime);
  CHECK_ERROR_RETURN(Result);

  //   Inform(MS 
  //     << SQLProcedureName 
  //     << " output parametr"
  //     << " UniKioskStatus = " << UniKioskStatus
  //     << " UniBillStatus = " << UniBillStatus
  //     << " outIdComBill = " << outIdComBill
  //     << " outIdBuyer = " << outIdBuyer
  //     << " outChequeText = " << outChequeText
  //     << " outIdCompany = " << outIdCompany
  //     << " isMillOvertaking = " << isMillOvertaking
  //     << " isProviderOvertaking = " << isProviderOvertaking
  //     << " isSuccessLocked = " << isSuccessLocked
  //     << " isAlwaysSendToProvider = " << isAlwaysSendToProvider
  //     << " outPayDateTime = " << TPosDateTime_To_DateTimeString(outPayDateTime)
  //     , ClassID, mtMessage);

  LogOutputParametr(SQLProcedureName, MS
    << " UniKioskStatus = " << UniKioskStatus
    << " UniBillStatus = " << UniBillStatus
    << " outIdComBill = " << outIdComBill
    << " outIdBuyer = " << outIdBuyer
    << " outChequeText = " << outChequeText
    << " outIdCompany = " << outIdCompany
    << " isMillOvertaking = " << isMillOvertaking
    << " isProviderOvertaking = " << isProviderOvertaking
    << " isSuccessLocked = " << isSuccessLocked
    << " isAlwaysSendToProvider = " << isAlwaysSendToProvider
    << " outPayDateTime = " << TPosDateTime_To_DateTimeString(outPayDateTime)
    << " outMsgError = " << outMsgError
  );

  return Result;
}


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
int TGivsDatabase::IntegProcess113Stage1(
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
    std::string& outIdComBill,
    std::string& outIdBuyer,    // std::string& outChequeText,
    std::string& outIdCompany,
    bool& isMillOvertaking,
    bool& isProviderOvertaking,
    bool& isSuccessLocked,
    bool& isAlwaysSendToProvider,
    TPosDateTime& outPayDateTime
)
{
  int	Result  = OK;
  TID	ClassID = tidDATABASE;
  std::string	ProcedureName = "TUniBillDatabase::IntegProcess113Stage1 : "; 
  std::string	SQLProcedureName = "IntegProcess113Stage1_v7";
  
  int					Col = 0;
  std::string	StValue;
  std::string	stCounterFlag;
  msstream		SQL;
  int					EstimatedResultCount = 12;
  TSQLResults	SQLResults;

  LogInputParametr(SQLProcedureName, MS
    << ": inIdComBill = "     << inIdComBill
    << ", inIdMillRq = "      << inIdMillRq
    << ", inIdTransaction = " << inIdTransaction
    << ", inExtReqNo = "      << inExtReqNo
    << ", inIdService = "     << inIdService
    << ", inIdSubService = "  << inIdSubService
    << ", inIdClient = "      << inIdClient
    << ", inAmountPayed = "   << inAmountPayed
    << ", inAmountCommis = "  << inAmountCommis
    << ", inIdTerm = "        << inIdTerm
    << " inKeyValue = "       << inKeyValue
  );

  //IntegProcess113Stage1_v6(
  //  IN inIdComBill bigint,
  //  IN inIdMillRq bigint,
  //  IN inIdTransaction bigint,
  //  IN inExtReqNo varchar(40),
  //  IN inIdService int,
  //  IN inIdSubService int,
  //  IN inIdClient varchar(128),
  //  IN inAmountPayed numeric(10, 2),
  //  IN inAmountCommis numeric(10, 2),
  //  IN inIdTerm varchar(50),
  //  IN inKeyValue varchar(12))

    // Формируем текст запроса
  SQL << CLEAR;
  SQL << "CALL " << SQLProcedureName
      << "( " << inIdComBill
      << ", " << inIdMillRq
      << ", " << inIdTransaction
      << ", " << ToSQLString(inExtReqNo)
      << ", " << inIdService
      << ", " << inIdSubService
      << ", " << ToSQLString(inIdClient)
      << ", " << inAmountPayed
      << ", " << inAmountCommis
      << ", " << ToSQLString(inIdTerm)
      << ", " << ToSQLString(inKeyValue)
      << " ); ";

  Result = ExecStoredProcedure(SQL.str(), EstimatedResultCount, SQLResults);
  CHECK_ERROR_RETURN(Result);

  //SELECT SP_result, OUT_result, OUT_msg, OUTidComBill, outIdBuyer,
  //  UniBillStatus,
  //  outIdCompany,
  //  isMillOvertaking,
  //  isProviderOvertaking,
  //  isSuccessLocked,
  //  isAlwaysSendToProvider,
  //  outPayDateTime
  //;

    // Вычитываем данные запроса
  Col = 1;
  // OUT_result, 
  Result = GetField(SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniKioskStatus = (eUniKioskStatus)StringToInt(StValue);
  // OUT_msg, 
  Result = GetField(SQLResults, 0, Col++, outMsgError);
  CHECK_ERROR_RETURN(Result);
  // OUTidComBill, 
  Result = GetField(SQLResults, 0, Col++, outIdComBill);
  CHECK_ERROR_RETURN(Result);
  // outIdBuyer
  Result = GetField(SQLResults, 0, Col++, outIdBuyer);
  CHECK_ERROR_RETURN(Result);

  //  UniBillStatus,
  Result = GetField(SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniBillStatus = (eUniBillStatus)StringToInt(StValue);
  //  outIdCompany,
  Result = GetField(SQLResults, 0, Col++, outIdCompany);
  CHECK_ERROR_RETURN(Result);
  //  isMillOvertaking
  Result = GetField(SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  if (StValue == "1") {
    isMillOvertaking = true;
  }
  else {
    isMillOvertaking = false;
  }
  //  isProviderOvertaking,
  Result = GetField(SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  if (StValue == "1") {
    isProviderOvertaking = true;
  }
  else {
    isProviderOvertaking = false;
  }
  //  isSuccessLocked,
  Result = GetField(SQLResults, 0, Col++, isSuccessLocked);
  CHECK_ERROR_RETURN(Result);
  //  isAlwaysSendToProvider,

  Result = GetField(SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  if (StValue == "1") {
    isAlwaysSendToProvider = true;
  }
  else {
    isAlwaysSendToProvider = false;
  }
  //  outPayDateTime
  Result = GetField(SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  Result = MySQLDateTime_To_TPosDateTime(StValue, outPayDateTime);
  CHECK_ERROR_RETURN(Result);
 
  LogOutputParametr(SQLProcedureName, MS
    << ": UniKioskStatus = "         << UniKioskStatus
    << ", UniBillStatus = "          << UniBillStatus
    << ", outIdComBill = "           << outIdComBill
    << ", outIdBuyer = "             << outIdBuyer
    // << " outChequeText = " << outChequeText
    << ", outIdCompany = "           << outIdCompany
    << ", isMillOvertaking = "       << isMillOvertaking
    << ", isProviderOvertaking = "   << isProviderOvertaking
    << ", isSuccessLocked = "        << isSuccessLocked
    << ", isAlwaysSendToProvider = " << isAlwaysSendToProvider
    << ", outPayDateTime = "         << TPosDateTime_To_DateTimeString(outPayDateTime)
    << ", outMsgError = "            << outMsgError
  );

  return Result;
}

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
  int TGivsDatabase::IntegProcess113Stage1Add0_v1 (
    std::string					inIdComBill,    
    std::string					inIdService,
    std::string					inIdBuyer,
    std::string					inAmountPayed,
    std::string					inIdTerm,

    eUniKioskStatus     &UniKioskStatus,
    eUniBillStatus      &UniBillStatus
  )
  {
    TID										ClassID = tidDATABASE;
    std::string						ProcedureName = "TUniBillDatabase::IntegProcess113Stage1Add0_v1 : ";
    int										Result = OK;

    std::string						SQLProcedureName = "IntegProcess113Stage1Add0_v1";
    msstream							SQL;
    int										EstimatedResultCount = 4;
    TSQLResults						SQLResults;
    int										Col;
    std::string						StValue;
    std::string						ErrorMsg;


    LogInputParametr (SQLProcedureName, MS
      << ": inIdComBill = "   << inIdComBill
      << ", inIdService = "   << inIdService
      << ", inIdBuyer = "     << inIdBuyer
      << ", inAmountPayed = " << inAmountPayed
      << ", inIdTerm = "      << ToSQLString(inIdTerm)
      << ". "
      );

    // CREATE PROCEDURE `IntegProcess113Stage1Add0_v1`(
    //IN inIdComBill bigint,
    //IN inIdService int,
    //IN inIdBuyer int,
    //IN inAmountPayed numeric(10, 2),
    //IN inIdTerm varchar(50))
    // 
    // Формируем текст запроса
    SQL << CLEAR;
    SQL << "CALL "
      << SQLProcedureName
      << "( "
      << " " << inIdComBill
      << "," << inIdService
      << "," << inIdBuyer
      << "," << inAmountPayed
      << "," << ToSQLString(inIdTerm)
      << " )";

    Result = ExecStoredProcedure (	
      SQL.str(),
      EstimatedResultCount,
      SQLResults
      );

    CHECK_ERROR_RETURN(Result);

   //SELECT
   //   exec_result,
   //   error_msg,

   //   UniKioskStatus,
   //   UniBillStatus
   // ;

	// Вычитываем данные запроса
	Col = 1;

  // error_msg,
  Result = GetField (SQLResults, 0, Col++, ErrorMsg);
  CHECK_ERROR_RETURN(Result);

  // UniKioskStatus,
  Result = GetField (SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

  // UniBillStatus
  Result = GetField (SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniBillStatus = (eUniBillStatus)StringToInt(StValue);

    LogOutputParametr (SQLProcedureName, MS
    << ": UniKioskStatus = " << UniKioskStatus
    << ", UniBillStatus = "  << UniBillStatus
    << ", error_msg = "      << ErrorMsg
    << ". "
    );

  return Result;
}

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
int TGivsDatabase::IntegProcess113Stage1Add0_v2 (
    std::string					inIdComBill,    
    std::string					inIdService,
    std::string					inIdBuyer,
    std::string					inAmountPayed,
    std::string					inIdTerm,
    std::string					inIdClient,

    eUniKioskStatus     &UniKioskStatus,
    eUniBillStatus      &UniBillStatus
  )
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TUniBillDatabase::IntegProcess113Stage1Add0_v2 : ";
  int										Result = OK;

  std::string						SQLProcedureName = "IntegProcess113Stage1Add0_v2";
  msstream							SQL;
  int										EstimatedResultCount = 4;
  TSQLResults						SQLResults;
  int										Col;
  std::string						StValue;
  std::string						ErrorMsg;

  // Если IdClient не заполнен , то в процедуру передавать "*"  (звездочку)
  // Довжину поля IdClient обмежуємо до 128 символів.
  if (inIdClient.empty())
    inIdClient = "*";
  else
    inIdClient = inIdClient.substr(0,127);


  LogInputParametr (SQLProcedureName, MS
    << ": inIdComBill = "   << inIdComBill
    << ", inIdService = "   << inIdService
    << ", inIdBuyer = "     << inIdBuyer
    << ", inAmountPayed = " << inAmountPayed
    << ", inIdTerm = "      << ToSQLString(inIdTerm)
    << ", inIdClient = "    << ToSQLString(inIdClient)
    << ". "
    );

  // CREATE PROCEDURE `IntegProcess113Stage1Add0_v1`(
  //IN inIdComBill bigint,
  //IN inIdService int,
  //IN inIdBuyer int,
  //IN inAmountPayed numeric(10, 2),
  //IN inIdTerm varchar(50))
  // 
  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "CALL "
    << SQLProcedureName
    << "( "
    << " " << inIdComBill
    << "," << inIdService
    << "," << inIdBuyer
    << "," << inAmountPayed
    << "," << ToSQLString(inIdTerm)
    << "," << ToSQLString(inIdClient)
    << " )";

  Result = ExecStoredProcedure (	
    SQL.str(),
    EstimatedResultCount,
    SQLResults
    );

  CHECK_ERROR_RETURN(Result);

 //SELECT
 //   exec_result,
 //   error_msg,

 //   UniKioskStatus,
 //   UniBillStatus
 // ;

	// Вычитываем данные запроса
	Col = 1;

  // error_msg,
  Result = GetField (SQLResults, 0, Col++, ErrorMsg);
  CHECK_ERROR_RETURN(Result);

  // UniKioskStatus,
  Result = GetField (SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

  // UniBillStatus
  Result = GetField (SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniBillStatus = (eUniBillStatus)StringToInt(StValue);

    LogOutputParametr (SQLProcedureName, MS
    << ": UniKioskStatus = " << UniKioskStatus
    << ", UniBillStatus = "  << UniBillStatus
    << ", error_msg = "      << ErrorMsg
    << ". "
    );

  return Result;
}
//----------------------------------------------------------------
int TGivsDatabase::IntegProcess113Stage1Add0_v2(
      std::string					inIdComBill,
      std::string					inIdService,
      std::string					inIdBuyer,
      std::string					inAmountPayed,
      std::string					inIdTerm,
      std::string					inIdClient,

      eUniKioskStatus& UniKioskStatus,
      eUniBillStatus& UniBillStatus,
      std::string& outMsgError
)
    {
      TID										ClassID = tidDATABASE;
      std::string						ProcedureName = "TUniBillDatabase::IntegProcess113Stage1Add0_v2 : ";
      int										Result = OK;

      std::string						SQLProcedureName = "IntegProcess113Stage1Add0_v2";
      msstream							SQL;
      int										EstimatedResultCount = 4;
      TSQLResults						SQLResults;
      int										Col;
      std::string						StValue;
      std::string						ErrorMsg;

      // Если IdClient не заполнен , то в процедуру передавать "*"  (звездочку)
      // Довжину поля IdClient обмежуємо до 128 символів.
      if (inIdClient.empty())
        inIdClient = "*";
      else
        inIdClient = inIdClient.substr(0, 127);


      LogInputParametr(SQLProcedureName, MS
        << ": inIdComBill = " << inIdComBill
        << ", inIdService = " << inIdService
        << ", inIdBuyer = " << inIdBuyer
        << ", inAmountPayed = " << inAmountPayed
        << ", inIdTerm = " << ToSQLString(inIdTerm)
        << ", inIdClient = " << ToSQLString(inIdClient)
        << ". "
      );

      // CREATE PROCEDURE `IntegProcess113Stage1Add0_v1`(
      //IN inIdComBill bigint,
      //IN inIdService int,
      //IN inIdBuyer int,
      //IN inAmountPayed numeric(10, 2),
      //IN inIdTerm varchar(50))
      // 
      // Формируем текст запроса
      SQL << CLEAR;
      SQL << "CALL "
        << SQLProcedureName
        << "( "
        << " " << inIdComBill
        << "," << inIdService
        << "," << inIdBuyer
        << "," << inAmountPayed
        << "," << ToSQLString(inIdTerm)
        << "," << ToSQLString(inIdClient)
        << " ); ";

      Result = ExecStoredProcedure( SQL.str(), EstimatedResultCount, SQLResults );
      CHECK_ERROR_RETURN(Result);

      //SELECT
      //   exec_result,
      //   error_msg,

      //   UniKioskStatus,
      //   UniBillStatus
      // ;

       // Вычитываем данные запроса
      Col = 1;

      // error_msg,
      Result = GetField(SQLResults, 0, Col++, ErrorMsg);
      CHECK_ERROR_RETURN(Result);

      outMsgError = ErrorMsg;

      // UniKioskStatus,
      Result = GetField(SQLResults, 0, Col++, StValue);
      CHECK_ERROR_RETURN(Result);

      UniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

      // UniBillStatus
      Result = GetField(SQLResults, 0, Col++, StValue);
      CHECK_ERROR_RETURN(Result);

      UniBillStatus = (eUniBillStatus)StringToInt(StValue);

      LogOutputParametr(SQLProcedureName, MS
        << ": UniKioskStatus = " << UniKioskStatus
        << ", UniBillStatus = " << UniBillStatus
        << ", error_msg = " << ErrorMsg
        << ". "
      );

      return Result;
    }
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
int TGivsDatabase::IntegProcess113Stage2 (	
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
  )
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TUniBillDatabase::IntegProcess113Stage2 : ";
  int										Result = OK;
  
#ifdef USE_OLD_MILL_REQUEST
  std::string						SQLProcedureName = "IntegProcess113Stage2_v4";
#else // USE_OLD_MILL_REQUEST
  /// std::string	SQLProcedureName = "IntegProcess113Stage2_v5";
  /// std::string	SQLProcedureName = "IntegProcess113Stage2_v6";
  std::string			SQLProcedureName = "IntegProcess113Stage2_v7";
#endif // USE_OLD_MILL_REQUEST

  msstream							SQL;
  int										EstimatedResultCount = 6;
  TSQLResults						SQLResults;
  int										Col;
  std::string						StValue;
//   std::string						stCounterFlag;
  std::string						stReturnVoucher;

  outReturnVoucher = false;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " inIdComBill = " << inIdComBill
//     << " inIdMillRq = " << inIdMillRq
//     << " inIdTransaction = " << inIdTransaction
//     << " inExtReqNo = " << inExtReqNo
//     << " inIdService = " << inIdService
//     << " inIdSubService = " << inIdSubService
//     << " inIdClient = " << inIdClient
//     << " inName = " << inName
//     << " inAmountPayed = " << inAmountPayed
//     << " inAmountCommis = " << inAmountCommis
//     << " inIdCompany = " << inIdCompany
//     << " inIdTerm = " << inIdTerm
//     << " inKeyValue = " << inKeyValue
//     << " inPayDateTime = " << TPosDateTime_To_DateTimeString(inPayDateTime)
//     << " inMillOperationId = " << inMillOperationId
//     << " inMillAcceptTime = " << TPosDateTime_To_DateTimeString(inMillAcceptTime)
//     << " inMillAcceptStatus = " << inMillAcceptStatus
//     << " inProviderAcceptStatus = " << inProviderAcceptStatus
//     << " inProviderExtNo = " << inProviderExtNo
//     << " inUpdateMillAcceptTime = " << inUpdateMillAcceptTime
//     << " inUniKioskStatus = " << inUniKioskStatus
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
    << " inIdComBill = " << inIdComBill
    << " inIdMillRq = " << inIdMillRq
    << " inIdTransaction = " << inIdTransaction
    << " inExtReqNo = " << inExtReqNo
    << " inIdService = " << inIdService
    << " inIdSubService = " << inIdSubService
    << " inIdClient = " << inIdClient
    << " inName = " << inName
    << " inAmountPayed = " << inAmountPayed
    << " inAmountCommis = " << inAmountCommis
    << " inIdCompany = " << inIdCompany
    << " inIdTerm = " << inIdTerm
    << " inKeyValue = " << inKeyValue
    << " inPayDateTime = " << TPosDateTime_To_DateTimeString(inPayDateTime)
    << " inMillOperationId = " << inMillOperationId
    << " inMillAcceptTime = " << TPosDateTime_To_DateTimeString(inMillAcceptTime)
    << " inMillAcceptStatus = " << inMillAcceptStatus
    << " inProviderAcceptStatus = " << inProviderAcceptStatus
    << " inProviderExtNo = " << inProviderExtNo
    << " inUpdateMillAcceptTime = " << inUpdateMillAcceptTime
    << " inUniKioskStatus = " << inUniKioskStatus
    );

// CREATE PROCEDURE IntegProcess113Stage2_v4
// (IN inIdComBill bigint,
// IN inIdMillRq bigint,
// IN inIdTransaction bigint,
// IN inExtReqNo varchar(40),
// IN inIdService int,
// IN inIdSubService int,
// IN inIdClient varchar(20),
// in inName  varchar(50),
// IN inAmountPayed numeric(10, 2),
// IN inAmountCommis numeric(10, 2),
// IN inIdCompany int,
// IN inIdTerm varchar(50),
// IN inKeyValue varchar(12),
// IN inPayDateTime datetime,
// IN inMillOperationId bigint,
// IN inMillAcceptTime datetime,
// IN inMillAcceptStatus tinyint,
// IN inProviderAcceptStatus tinyint,
// IN inProviderExtNo bigint,
// IN inUpdateMillAcceptTime int,
// IN inUniKioskStatus int
// )

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "call "
    << SQLProcedureName
    << "("
    << " " << inIdComBill
    << "," << inIdMillRq
    << "," << inIdTransaction
    << "," << ToSQLString(inExtReqNo)
    << "," << inIdService
    << "," << inIdSubService
    << "," << ToSQLString(inIdClient)
    << "," << ToSQLString(inName)
    << "," << inAmountPayed
    << "," << inAmountCommis
    << "," << inIdCompany
    << "," << ToSQLString(inIdTerm)
    << "," << ToSQLString(inKeyValue)
    << "," << ToSQLString(TPosDateTime_To_MySQLDateTime(inPayDateTime))
    << "," << inMillOperationId
    << "," << ToSQLString(TPosDateTime_To_MySQLDateTime(inMillAcceptTime))
    << "," << inMillAcceptStatus
    << "," << inProviderAcceptStatus
    << "," << inProviderExtNo
    << "," << inUpdateMillAcceptTime
    << "," << inUniKioskStatus
    << ")";

  Result = ExecStoredProcedure (	
    SQL.str(),
    EstimatedResultCount,
    SQLResults
    );

  CHECK_ERROR_RETURN(Result);

//   SELECT
//     exec_result,
//     error_msg,
//     UniKioskStatus,
//     outPayDateTime,
//     outReturnVoucher,
//     outSendToOvertaking
//   ;

	// Вычитываем данные запроса
	Col = 2;

  Result = GetField (SQLResults, 
    0, Col, StValue);++Col;
  CHECK_ERROR_RETURN(Result);

  outUniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

  Result = GetField (SQLResults, 
    0, Col, StValue);++Col;
  CHECK_ERROR_RETURN(Result);

  Result = MySQLDateTime_To_TPosDateTime(StValue, outPayDateTime);
  CHECK_ERROR_RETURN(Result);

  Result = GetField (SQLResults, 
    0, Col, stReturnVoucher);++Col;
  CHECK_ERROR_RETURN(Result);

  if (stReturnVoucher == "1") {
    outReturnVoucher = true;
  } else {
    outReturnVoucher = false;
  }
  
  Result = GetField (SQLResults, 
    0, Col, outSendToOvertaking);++Col;
  CHECK_ERROR_RETURN(Result);

//   Inform(MS 
//     << SQLProcedureName 
//     << " output parametr"
//     << " outUniKioskStatus = " << outUniKioskStatus
//     << " outPayDateTime = " << TPosDateTime_To_DateTimeString(outPayDateTime)
//     << " outReturnVoucher = " << stReturnVoucher
//     << " outSendToOvertaking = " << outSendToOvertaking
//     , ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS
    << " outUniKioskStatus = " << outUniKioskStatus
    << " outPayDateTime = " << TPosDateTime_To_DateTimeString(outPayDateTime)
    << " outReturnVoucher = " << stReturnVoucher
    << " outSendToOvertaking = " << outSendToOvertaking
    );

  return Result;
}
//----------------------------------------------------------------
int TGivsDatabase::IntegProcess113Stage2(
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
)
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TUniBillDatabase::IntegProcess113Stage2 : ";
  int										Result = OK;

#ifdef USE_OLD_MILL_REQUEST
  std::string						SQLProcedureName = "IntegProcess113Stage2_v4";
#else // USE_OLD_MILL_REQUEST
  /// std::string	SQLProcedureName = "IntegProcess113Stage2_v5";
  /// std::string	SQLProcedureName = "IntegProcess113Stage2_v6";
  std::string			SQLProcedureName = "IntegProcess113Stage2_v7";
#endif // USE_OLD_MILL_REQUEST

  msstream							SQL;
  int										EstimatedResultCount = 6;
  TSQLResults						SQLResults;
  int										Col;
  std::string						StValue;
  //   std::string						stCounterFlag;
  std::string						stReturnVoucher;

  outReturnVoucher = false;

  //   Inform(MS 
  //     << SQLProcedureName
  //     << " in parametr"
  //     << " inIdComBill = " << inIdComBill
  //     << " inIdMillRq = " << inIdMillRq
  //     << " inIdTransaction = " << inIdTransaction
  //     << " inExtReqNo = " << inExtReqNo
  //     << " inIdService = " << inIdService
  //     << " inIdSubService = " << inIdSubService
  //     << " inIdClient = " << inIdClient
  //     << " inName = " << inName
  //     << " inAmountPayed = " << inAmountPayed
  //     << " inAmountCommis = " << inAmountCommis
  //     << " inIdCompany = " << inIdCompany
  //     << " inIdTerm = " << inIdTerm
  //     << " inKeyValue = " << inKeyValue
  //     << " inPayDateTime = " << TPosDateTime_To_DateTimeString(inPayDateTime)
  //     << " inMillOperationId = " << inMillOperationId
  //     << " inMillAcceptTime = " << TPosDateTime_To_DateTimeString(inMillAcceptTime)
  //     << " inMillAcceptStatus = " << inMillAcceptStatus
  //     << " inProviderAcceptStatus = " << inProviderAcceptStatus
  //     << " inProviderExtNo = " << inProviderExtNo
  //     << " inUpdateMillAcceptTime = " << inUpdateMillAcceptTime
  //     << " inUniKioskStatus = " << inUniKioskStatus
  //     , ClassID, mtMessage);

  LogInputParametr(SQLProcedureName, MS
    << " inIdComBill = " << inIdComBill
    << " inIdMillRq = " << inIdMillRq
    << " inIdTransaction = " << inIdTransaction
    << " inExtReqNo = " << inExtReqNo
    << " inIdService = " << inIdService
    << " inIdSubService = " << inIdSubService
    << " inIdClient = " << inIdClient
    << " inName = " << inName
    << " inAmountPayed = " << inAmountPayed
    << " inAmountCommis = " << inAmountCommis
    << " inIdCompany = " << inIdCompany
    << " inIdTerm = " << inIdTerm
    << " inKeyValue = " << inKeyValue
    << " inPayDateTime = " << TPosDateTime_To_DateTimeString(inPayDateTime)
    << " inMillOperationId = " << inMillOperationId
    << " inMillAcceptTime = " << TPosDateTime_To_DateTimeString(inMillAcceptTime)
    << " inMillAcceptStatus = " << inMillAcceptStatus
    << " inProviderAcceptStatus = " << inProviderAcceptStatus
    << " inProviderExtNo = " << inProviderExtNo
    << " inUpdateMillAcceptTime = " << inUpdateMillAcceptTime
    << " inUniKioskStatus = " << inUniKioskStatus
  );

  // CREATE PROCEDURE IntegProcess113Stage2_v4
  // (IN inIdComBill bigint,
  // IN inIdMillRq bigint,
  // IN inIdTransaction bigint,
  // IN inExtReqNo varchar(40),
  // IN inIdService int,
  // IN inIdSubService int,
  // IN inIdClient varchar(20),
  // in inName  varchar(50),
  // IN inAmountPayed numeric(10, 2),
  // IN inAmountCommis numeric(10, 2),
  // IN inIdCompany int,
  // IN inIdTerm varchar(50),
  // IN inKeyValue varchar(12),
  // IN inPayDateTime datetime,
  // IN inMillOperationId bigint,
  // IN inMillAcceptTime datetime,
  // IN inMillAcceptStatus tinyint,
  // IN inProviderAcceptStatus tinyint,
  // IN inProviderExtNo bigint,
  // IN inUpdateMillAcceptTime int,
  // IN inUniKioskStatus int
  // )

    // Формируем текст запроса
  SQL << CLEAR;
  SQL << "CALL "
    << SQLProcedureName
    << "("
    << " " << inIdComBill
    << "," << inIdMillRq
    << "," << inIdTransaction
    << "," << ToSQLString(inExtReqNo)
    << "," << inIdService
    << "," << inIdSubService
    << "," << ToSQLString(inIdClient)
    << "," << ToSQLString(inName)
    << "," << inAmountPayed
    << "," << inAmountCommis
    << "," << inIdCompany
    << "," << ToSQLString(inIdTerm)
    << "," << ToSQLString(inKeyValue)
    << "," << ToSQLString(TPosDateTime_To_MySQLDateTime(inPayDateTime))
    << "," << inMillOperationId
    << "," << ToSQLString(TPosDateTime_To_MySQLDateTime(inMillAcceptTime))
    << "," << inMillAcceptStatus
    << "," << inProviderAcceptStatus
    << "," << inProviderExtNo
    << "," << inUpdateMillAcceptTime
    << "," << inUniKioskStatus
    << "); ";

  Result = ExecStoredProcedure( SQL.str(), EstimatedResultCount, SQLResults );
  CHECK_ERROR_RETURN(Result);

  //   SELECT
  //     exec_result,
  //     error_msg,
  //     UniKioskStatus,
  //     outPayDateTime,
  //     outReturnVoucher,
  //     outSendToOvertaking
  //   ;

    // Вычитываем данные запроса
  Col = 1;

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  outMsgError = StValue;

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  outUniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = MySQLDateTime_To_TPosDateTime(StValue, outPayDateTime);
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, stReturnVoucher); ++Col;
  CHECK_ERROR_RETURN(Result);

  if (stReturnVoucher == "1") {
    outReturnVoucher = true;
  }
  else {
    outReturnVoucher = false;
  }

  Result = GetField(SQLResults, 0, Col, outSendToOvertaking); ++Col;
  CHECK_ERROR_RETURN(Result);

  //   Inform(MS 
  //     << SQLProcedureName 
  //     << " output parametr"
  //     << " outUniKioskStatus = " << outUniKioskStatus
  //     << " outPayDateTime = " << TPosDateTime_To_DateTimeString(outPayDateTime)
  //     << " outReturnVoucher = " << stReturnVoucher
  //     << " outSendToOvertaking = " << outSendToOvertaking
  //     , ClassID, mtMessage);

  LogOutputParametr(SQLProcedureName, MS 
    << " outUniKioskStatus = "   << outUniKioskStatus
    << " outPayDateTime = "      << TPosDateTime_To_DateTimeString(outPayDateTime)
    << " outReturnVoucher = "    << stReturnVoucher
    << " outSendToOvertaking = " << outSendToOvertaking
    << " outMsgError = "         << outMsgError
  );

  return Result;
}
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
int TGivsDatabase::GetIntegTransactionData (	
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
  )
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::GetIntegTransactionData : ";
  int										Result = OK;

  std::string						SQLProcedureName = "GetIntegTransactionData_v3";
  msstream							SQL;
  int										EstimatedResultCount = 10;
  TSQLResults						SQLResults;
  int										Col;
  std::string						StValue;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " inIdBuyer = " << inIdBuyer
//     << " inCertificateNumber = " << inCertificateNumber
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
    << " inIdBuyer = " << inIdBuyer
    << " inCertificateNumber = " << inCertificateNumber
    );

// CREATE PROCEDURE `GetIntegTransactionData_v3`
// -- version 1.0
// -- from 26.02.2014
// (IN InIdBuyer           SMALLINT,
//  IN InCertificateNumber VARCHAR(40)
//  )

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "call "
    << SQLProcedureName
    << "("
    << " " << inIdBuyer
    << "," << ToSQLString(inCertificateNumber)
    << ")";

  Result = ExecStoredProcedure (	
    SQL.str(),
    EstimatedResultCount,
    SQLResults
    );

  CHECK_ERROR_RETURN(Result);

//   SELECT exec_result
//        , error_msg
//        , outIdComBill
//        , outTransactionStatus
//        , outMillRqStatus
//        , outAmountPayed
//        , outRepeatMode
//        , outStReturn
//        , outPayDateTime
//        , outFulFilledDateTime
//   ;

	// Вычитываем данные запроса
	Col = 2;

  Result = GetField (SQLResults, 
		0, Col, outIdComBill);++Col;
	CHECK_ERROR_RETURN(Result);

  Result = GetField (SQLResults, 
    0, Col, StValue);++Col;
  CHECK_ERROR_RETURN(Result);

  outTransactionStatus = (eTransactionStatus)StringToInt(StValue);

  Result = GetField (SQLResults, 
    0, Col, StValue);++Col;
  CHECK_ERROR_RETURN(Result);

  outMillRqStatus = (eMillAcceptStatus)StringToInt(StValue);
  
  Result = GetField (SQLResults, 
    0, Col, outAmountPayed);++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField (SQLResults, 
    0, Col, outRepeatMode);++Col;
  CHECK_ERROR_RETURN(Result);
  
  Result = GetField (SQLResults, 
    0, Col, outStReturn);++Col;
  CHECK_ERROR_RETURN(Result);
  
  Result = GetField (SQLResults, 
    0, Col, StValue);++Col;
  CHECK_ERROR_RETURN(Result);
  
  if (StValue == SQL_NULL) {
     DateString_To_TPosDateTime("2000.01.01", outPayDateTime);
  } else {
    Result = MySQLDateTime_To_TPosDateTime(StValue, outPayDateTime);
    CHECK_ERROR_RETURN(Result);
  }

  Result = GetField (SQLResults, 
    0, Col, StValue);++Col;
  CHECK_ERROR_RETURN(Result);
  
  if (StValue == SQL_NULL) {
     DateString_To_TPosDateTime("2000.01.01", outFulFilledDateTime);
  } else {
    Result = MySQLDateTime_To_TPosDateTime(StValue, outFulFilledDateTime);
    CHECK_ERROR_RETURN(Result);
  }

//   Inform(MS 
//     << SQLProcedureName 
//     << " output parametr"
//     << " outIdComBill = " << outIdComBill
//     << " outTransactionStatus = " << outTransactionStatus
//     << " outMillRqStatus = " << outMillRqStatus
//     << " outAmountPayed = " << outAmountPayed
//     << " outRepeatMode = " << outRepeatMode
//     << " outStReturn = " << outStReturn
//     << " outPayDateTime = " << outPayDateTime
//     , ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS
    << " outIdComBill = " << outIdComBill
    << " outTransactionStatus = " << outTransactionStatus
    << " outMillRqStatus = " << outMillRqStatus
    << " outAmountPayed = " << outAmountPayed
    << " outRepeatMode = " << outRepeatMode
    << " outStReturn = " << outStReturn
    << " outPayDateTime = " << outPayDateTime
    << " outFulFilledDateTime = " << outFulFilledDateTime
    );

  return Result;
}
//----------------------------------------------------------------
int TGivsDatabase::GetIntegTransactionData(
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
)
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::GetIntegTransactionData : ";
  int										Result = OK;

  std::string						SQLProcedureName = "GetIntegTransactionData_v3";
  msstream							SQL;
  int										EstimatedResultCount = 10;
  TSQLResults						SQLResults;
  int										Col;
  std::string						StValue;

  //   Inform(MS 
  //     << SQLProcedureName
  //     << " in parametr"
  //     << " inIdBuyer = " << inIdBuyer
  //     << " inCertificateNumber = " << inCertificateNumber
  //     , ClassID, mtMessage);

  LogInputParametr(SQLProcedureName, MS << ": inIdBuyer = " << inIdBuyer << ", inCertificateNumber = " << inCertificateNumber
  );

  // CREATE PROCEDURE `GetIntegTransactionData_v3`
  // -- version 1.0
  // -- from 26.02.2014
  // (IN InIdBuyer           SMALLINT,
  //  IN InCertificateNumber VARCHAR(40)
  //  )

    // Формируем текст запроса
  SQL << CLEAR;
  SQL << "CALL "
    << SQLProcedureName
    << "("
    << " " << inIdBuyer
    << "," << ToSQLString(inCertificateNumber)
    << "); ";

  Result = ExecStoredProcedure( SQL.str(), EstimatedResultCount, SQLResults );
  CHECK_ERROR_RETURN(Result);

  //   SELECT exec_result
  //        , error_msg
  //        , outIdComBill
  //        , outTransactionStatus
  //        , outMillRqStatus
  //        , outAmountPayed
  //        , outRepeatMode
  //        , outStReturn
  //        , outPayDateTime
  //        , outFulFilledDateTime
  //   ;

    // Вычитываем данные запроса
  Col = 1;

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  outMsgError = StValue;

  Result = GetField(SQLResults, 0, Col, outIdComBill); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  outTransactionStatus = (eTransactionStatus)StringToInt(StValue);

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  outMillRqStatus = (eMillAcceptStatus)StringToInt(StValue);

  Result = GetField(SQLResults, 0, Col, outAmountPayed); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, outRepeatMode); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, outStReturn); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  if (StValue == SQL_NULL) {
    DateString_To_TPosDateTime("2000.01.01", outPayDateTime);
  }
  else {
    Result = MySQLDateTime_To_TPosDateTime(StValue, outPayDateTime);
    CHECK_ERROR_RETURN(Result);
  }

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  if (StValue == SQL_NULL) {
    DateString_To_TPosDateTime("2000.01.01", outFulFilledDateTime);
  }
  else {
    Result = MySQLDateTime_To_TPosDateTime(StValue, outFulFilledDateTime);
    CHECK_ERROR_RETURN(Result);
  }

  //   Inform(MS 
  //     << SQLProcedureName 
  //     << " output parametr"
  //     << " outIdComBill = " << outIdComBill
  //     << " outTransactionStatus = " << outTransactionStatus
  //     << " outMillRqStatus = " << outMillRqStatus
  //     << " outAmountPayed = " << outAmountPayed
  //     << " outRepeatMode = " << outRepeatMode
  //     << " outStReturn = " << outStReturn
  //     << " outPayDateTime = " << outPayDateTime
  //     , ClassID, mtMessage);

  LogOutputParametr(SQLProcedureName, MS
    << " outIdComBill = " << outIdComBill
    << " outTransactionStatus = " << outTransactionStatus
    << " outMillRqStatus = " << outMillRqStatus
    << " outAmountPayed = " << outAmountPayed
    << " outRepeatMode = " << outRepeatMode
    << " outStReturn = " << outStReturn
    << " outPayDateTime = " << outPayDateTime
    << " outFulFilledDateTime = " << outFulFilledDateTime
    << " outMsgError = " << outMsgError
  );

  return Result;
}
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
int TGivsDatabase::GetIntegTransactionData(
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
  eUniKioskStatus&    outUniKioskStatus
)
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::GetIntegTransactionData : ";
  int										Result = OK;

  std::string						SQLProcedureName = "GetIntegTransactionData_v4";
  msstream							SQL;
  int										EstimatedResultCount = 11;
  TSQLResults						SQLResults;
  int										Col;
  std::string						StValue, strMsg;

  LogInputParametr(SQLProcedureName, MS << " inIdBuyer = " << inIdBuyer
    << ", inCertificateNumber = " << inCertificateNumber   << ", inIdService = " << inIdService
  );

  // CREATE PROCEDURE `GetIntegTransactionData_v3`
  // -- version 1.0
  // -- from 26.02.2014
  // (IN InIdBuyer           SMALLINT,
  //  IN InCertificateNumber VARCHAR(40)
  //  )

    // Формируем текст запроса
  SQL << CLEAR;
  SQL << "call "
    << SQLProcedureName
    << "( " << inIdBuyer
    << ","  << ToSQLString(inCertificateNumber)
    << ","  << inIdService
    << ")";

  Result = ExecStoredProcedure(
    SQL.str(),
    EstimatedResultCount,
    SQLResults
  );

  CHECK_ERROR_RETURN(Result);

  //SELECT exec_result
  //  , error_msg
  //  , outIdComBill
  //  , outTransactionStatus
  //  , outMillRqStatus
  //  , outAmountPayed
  //  , outRepeatMode
  //  , outStReturn
  //  , outPayDateTime
  //  , outFulFilledDateTime
  //  , outUniKioskStatus
  //;

    // Вычитываем данные запроса
  Col = 1; // 

  Result = GetField(SQLResults, 0, Col, strMsg); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, outIdComBill); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  outTransactionStatus = (eTransactionStatus)StringToInt(StValue);

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  outMillRqStatus = (eMillAcceptStatus)StringToInt(StValue);

  Result = GetField(SQLResults, 0, Col, outAmountPayed); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, outRepeatMode); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, outStReturn); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  if (StValue == SQL_NULL) {
    DateString_To_TPosDateTime("2000.01.01", outPayDateTime);
  }
  else {
    Result = MySQLDateTime_To_TPosDateTime(StValue, outPayDateTime);
    CHECK_ERROR_RETURN(Result);
  }

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  if (StValue == SQL_NULL) {
    DateString_To_TPosDateTime("2000.01.01", outFulFilledDateTime);
  }
  else {
    Result = MySQLDateTime_To_TPosDateTime(StValue, outFulFilledDateTime);
    CHECK_ERROR_RETURN(Result);
  }

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  outUniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

  LogOutputParametr(SQLProcedureName, MS
    << " outError_msg = "          << strMsg
    << ", outIdComBill = "         << outIdComBill
    << ", outTransactionStatus = " << outTransactionStatus
    << ", outMillRqStatus = "      << outMillRqStatus
    << ", outAmountPayed = "       << outAmountPayed
    << ", outRepeatMode = "        << outRepeatMode
    << ", outStReturn = "          << outStReturn
    << ", outPayDateTime = "       << outPayDateTime
    << ", outFulFilledDateTime = " << outFulFilledDateTime
    << ", outUniKioskStatus = "    << outUniKioskStatus
  );

  return Result;
}
//----------------------------------------------------------------
int TGivsDatabase::GetIntegTransactionData(
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
)
{
  TID					ClassID = tidDATABASE;
  std::string	ProcedureName = "TGivsDatabase::GetIntegTransactionData : ";
  int					Result = OK;

  std::string	SQLProcedureName = "GetIntegTransactionData_v4";
  msstream		SQL;
  int					EstimatedResultCount = 11;
  TSQLResults	SQLResults;
  int					Col = 0;
  std::string	StValue, strMsg;

  LogInputParametr(SQLProcedureName, MS 
    << ": inIdBuyer = " << inIdBuyer
    << ", inCertificateNumber = " << inCertificateNumber 
    << ", inIdService = " << inIdService
  );

  // CREATE PROCEDURE `GetIntegTransactionData_v3`
  // -- version 1.0
  // -- from 26.02.2014
  // (IN InIdBuyer           SMALLINT,
  //  IN InCertificateNumber VARCHAR(40)
  //  )

    // Формируем текст запроса
  SQL << CLEAR;
  SQL << "CALL "
    << SQLProcedureName
    << "( " << inIdBuyer
    << "," << ToSQLString(inCertificateNumber)
    << "," << inIdService
    << "); ";

  Result = ExecStoredProcedure( SQL.str(), EstimatedResultCount, SQLResults );
  CHECK_ERROR_RETURN(Result);

  //SELECT exec_result
  //  , error_msg
  //  , outIdComBill
  //  , outTransactionStatus
  //  , outMillRqStatus
  //  , outAmountPayed
  //  , outRepeatMode
  //  , outStReturn
  //  , outPayDateTime
  //  , outFulFilledDateTime
  //  , outUniKioskStatus
  //;

  // Вычитываем данные запроса
  Col = 1; // 

  Result = GetField(SQLResults, 0, Col, strMsg); ++Col;
  CHECK_ERROR_RETURN(Result);

  outMsgError = strMsg;

  Result = GetField(SQLResults, 0, Col, outIdComBill); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  outTransactionStatus = (eTransactionStatus)StringToInt(StValue);

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  outMillRqStatus = (eMillAcceptStatus)StringToInt(StValue);

  Result = GetField(SQLResults, 0, Col, outAmountPayed); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, outRepeatMode); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, outStReturn); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  if (StValue == SQL_NULL) {
    DateString_To_TPosDateTime("2000.01.01", outPayDateTime);
  }
  else {
    Result = MySQLDateTime_To_TPosDateTime(StValue, outPayDateTime);
    CHECK_ERROR_RETURN(Result);
  }

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  if (StValue == SQL_NULL) {
    DateString_To_TPosDateTime("2000.01.01", outFulFilledDateTime);
  }
  else {
    Result = MySQLDateTime_To_TPosDateTime(StValue, outFulFilledDateTime);
    CHECK_ERROR_RETURN(Result);
  }

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  outUniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

  LogOutputParametr(SQLProcedureName, MS
    << " outError_msg = " << strMsg
    << ", outIdComBill = " << outIdComBill
    << ", outTransactionStatus = " << outTransactionStatus
    << ", outMillRqStatus = " << outMillRqStatus
    << ", outAmountPayed = " << outAmountPayed
    << ", outRepeatMode = " << outRepeatMode
    << ", outStReturn = " << outStReturn
    << ", outPayDateTime = " << outPayDateTime
    << ", outFulFilledDateTime = " << outFulFilledDateTime
    << ", outUniKioskStatus = " << outUniKioskStatus
  );

  return Result;
}
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
int TGivsDatabase::StornoIntegratorBill (	
  std::string					inIdBuyer,
  std::string					inCertificateNumber,

  std::string					&outIdComBill, 
  eUniKioskStatus			&outStornoStatus, 
  std::string					&outStornoAmount,
  bool                &outIsReturnMoney
                                    )
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::StornoIntegratorBill : ";
  int										Result = OK;

  std::string						SQLProcedureName = "StornoIntegratorBill";
  msstream							SQL;
  int										EstimatedResultCount = 6;
  TSQLResults						SQLResults;
  int										Col;
  std::string						StValue;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " inIdBuyer = " << inIdBuyer
//     << " inCertificateNumber = " << inCertificateNumber
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
    << " inIdBuyer = " << inIdBuyer
    << " inCertificateNumber = " << inCertificateNumber
    );

// CREATE PROCEDURE StornoUniBillMill_v2
// -- version 2.0
// -- from 24.02.2012
// ( 
//   IN InIdBuyer SMALLINT, 
//   IN InCertificateNumber VARCHAR(40)
//   )

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "call "
    << SQLProcedureName
    << "("
    << " " << inIdBuyer
    << "," << ToSQLString(inCertificateNumber)
    << ")";

  Result = ExecStoredProcedure (	
    SQL.str(),
    EstimatedResultCount,
    SQLResults
    );

  CHECK_ERROR_RETURN(Result);

//   SELECT exec_result
//        , error_msg
//        , CurIdComBill
//        , StornoStatus
//        , StornoAmount
//        , isReturnMoney
//   ;

	// Вычитываем данные запроса
	Col = 2;

  Result = GetField (SQLResults, 
		0, Col, outIdComBill);++Col;
	CHECK_ERROR_RETURN(Result);

  Result = GetField (SQLResults, 
    0, Col, StValue);++Col;
  CHECK_ERROR_RETURN(Result);

  outStornoStatus = (eUniKioskStatus)StringToInt(StValue);

  Result = GetField (SQLResults, 
    0, Col, outStornoAmount);++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField (SQLResults, 
    0, Col, StValue);++Col;
  CHECK_ERROR_RETURN(Result);

  if (StValue == "1") {
    outIsReturnMoney = true;
  } else {
    outIsReturnMoney = false;
  }

//   Inform(MS 
//     << SQLProcedureName 
//     << " output parametr"
//     << " outIdComBill = " << outIdComBill
//     << " outStornoStatus = " << outStornoStatus
//     << " outStornoAmount = " << outStornoAmount
//     << " outIsReturnMoney = " << outIsReturnMoney
//     , ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS
    << " outIdComBill = " << outIdComBill
    << " outStornoStatus = " << outStornoStatus
    << " outStornoAmount = " << outStornoAmount
    << " outIsReturnMoney = " << outIsReturnMoney
    );

  return Result;
}
//----------------------------------------------------------------
int TGivsDatabase::StornoIntegratorBill(
  std::string					inIdBuyer,
  std::string					inCertificateNumber,

  std::string& outIdComBill,
  eUniKioskStatus& outStornoStatus,
  std::string& outStornoAmount,
  bool& outIsReturnMoney,
  std::string& outMsgError
)
{
  TID					ClassID = tidDATABASE;
  std::string	ProcedureName = "TGivsDatabase::StornoIntegratorBill : ";
  int					Result = OK;

  std::string	SQLProcedureName = "StornoIntegratorBill";
  msstream		SQL;
  int					EstimatedResultCount = 6;
  TSQLResults	SQLResults;
  int					Col = 0;
  std::string	StValue;

  //   Inform(MS 
  //     << SQLProcedureName
  //     << " in parametr"
  //     << " inIdBuyer = " << inIdBuyer
  //     << " inCertificateNumber = " << inCertificateNumber
  //     , ClassID, mtMessage);

  LogInputParametr(SQLProcedureName, MS << ": inIdBuyer = " << inIdBuyer << " inCertificateNumber = " << inCertificateNumber );

  // CREATE PROCEDURE StornoUniBillMill_v2
  // -- version 2.0
  // -- from 24.02.2012
  // ( 
  //   IN InIdBuyer SMALLINT, 
  //   IN InCertificateNumber VARCHAR(40)
  //   )

    // Формируем текст запроса
  SQL << CLEAR;
  SQL << "CALL " << SQLProcedureName << "( " << inIdBuyer << ", " << ToSQLString(inCertificateNumber) << " ); ";

  Result = ExecStoredProcedure( SQL.str(), EstimatedResultCount, SQLResults );
  CHECK_ERROR_RETURN(Result);

  //   SELECT exec_result
  //        , error_msg
  //        , CurIdComBill
  //        , StornoStatus
  //        , StornoAmount
  //        , isReturnMoney
  //   ;

  // Вычитываем данные запроса
  Col = 1;

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  outMsgError = StValue;

  Result = GetField(SQLResults, 0, Col, outIdComBill); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  outStornoStatus = (eUniKioskStatus)StringToInt(StValue);

  Result = GetField(SQLResults, 0, Col, outStornoAmount); ++Col;
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col, StValue); ++Col;
  CHECK_ERROR_RETURN(Result);

  if (StValue == "1") {
    outIsReturnMoney = true;
  }
  else {
    outIsReturnMoney = false;
  }

  //   Inform(MS 
  //     << SQLProcedureName 
  //     << " output parametr"
  //     << " outIdComBill = " << outIdComBill
  //     << " outStornoStatus = " << outStornoStatus
  //     << " outStornoAmount = " << outStornoAmount
  //     << " outIsReturnMoney = " << outIsReturnMoney
  //     , ClassID, mtMessage);

  LogOutputParametr(SQLProcedureName, MS
    << " outIdComBill = " << outIdComBill
    << " outStornoStatus = " << outStornoStatus
    << " outStornoAmount = " << outStornoAmount
    << " outIsReturnMoney = " << outIsReturnMoney
    << " outMsgError = " << outMsgError
  );

  return Result;
}

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
int TGivsDatabase::StoreCounters (
  std::string	        inIdComBill,
  int                 inidCounter,
  std::string	        inIdService,
  std::string	        inIdClient,
  std::string	        inNameCounter,
  std::string	        inPrevCounter,
  std::string	        inCurrCounter,
  std::string	        inUsedCounter
  )
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::StoreCounters : ";
  int										Result = OK;
  std::string						SQLProcedureName = "StoreCounters";

  BaseMsstream					SQL;
  TSQLResults						SQLResults;
  //  S_PAYED_SUM           payedSum;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " IdComBill = " << inIdComBill
//     << " idCounter = " << inidCounter
//     << " IdService = " << inIdService
//     << " IdClient = " << inIdClient
//     << " NameCounter = " << inNameCounter
//     << " PrevCounter = " << inPrevCounter
//     << " CurrCounter = " << inCurrCounter
//     << " UsedCounter = " << inUsedCounter
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
    << " IdComBill = " << inIdComBill
    << " idCounter = " << inidCounter
    << " IdService = " << inIdService
    << " IdClient = " << inIdClient
    << " NameCounter = " << inNameCounter
    << " PrevCounter = " << inPrevCounter
    << " CurrCounter = " << inCurrCounter
    << " UsedCounter = " << inUsedCounter
    );

  if (inPrevCounter == "") {
    inPrevCounter = "0";
  }
  else {
    inPrevCounter = ControlCounterValue12dot4(inPrevCounter);
  }

  if (inCurrCounter == "") {
    inCurrCounter = "0";
  }
  else {
    inCurrCounter = ControlCounterValue12dot4(inCurrCounter);
  }

  if (inUsedCounter == "") {
    inUsedCounter = "0";
  }
  else {
    if (inPrevCounter == "0" && inCurrCounter == "0") {
      inUsedCounter = "0";
    }
    else {
      inUsedCounter = ControlCounterValue12dot4(inUsedCounter);
    }    
  }

// DROP TABLE IF EXISTS `tradedb3_1`.`countersoffline`;
// CREATE TABLE  `tradedb3_1`.`countersoffline` (
//   `IdComBill` bigint(20) NOT NULL,
//   `idCounter` int(11) NOT NULL,
//   `IdService` int(11) NOT NULL,
//   `IdClient` varchar(50) NOT NULL,
//   `NameCounter` varchar(45) NOT NULL,
//   `PrevCounter` decimal(12,4) NOT NULL,
//   `CurrCounter` decimal(12,4) NOT NULL,
//   `UsedCounter` decimal(12,4) NOT NULL,
//   PRIMARY KEY (`IdComBill`)
// ) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

  // Формируем текст запроса 
  SQL << CLEAR;
  SQL << "delete from icounters where "
    << " IdComBill = " << inIdComBill
    << " and idCounter = " << inidCounter
    ;

  // Выполняем запрос
  Result = ExecSQLRequest (	
    SQL.str()
    );

  CHECK_ERROR_RETURN(Result);

  // Формируем текст запроса 
  SQL << CLEAR;
  SQL << "insert into icounters (IdComBill, IdCounter, IdService, IdClient, NameCounter, PrevCounter, CurrCounter, UsedCounter, Payed ) "
    << " values("
    << " " << inIdComBill
    << ", " << inidCounter
    << ", " << inIdService
    << ", " << ToSQLString(inIdClient)
    << ", " << ToSQLString(inNameCounter)
    << ", " << inPrevCounter
    << ", " << inCurrCounter
    << ", " << inUsedCounter
    << ", now()" 
    << "); "
    ;

  // Выполняем запрос
  Result = ExecSQLRequest (	
    SQL.str()
    );

  CHECK_ERROR_RETURN(Result);

//   Inform(MS 
//     << SQLProcedureName
//     << " output parametr"
//     //    << " OutIdService = " << OutIdService
//     ,ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS
    //    << " OutIdService = " << OutIdService
    );

  return Result;
}

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
int TGivsDatabase::StorePeriod (
  std::string	        inIdComBill,
  std::string					inPeriodID,
  std::string					inPeriodMode,
  TPosDateTime			  inPeriodBegin,
  TPosDateTime        inPeriodEnd
  )
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::StorePeriod : ";
  int										Result = OK;
  std::string						SQLProcedureName = "StorePeriod";

  BaseMsstream					SQL;
  TSQLResults						SQLResults;
  //  S_PAYED_SUM           payedSum;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " inIdComBill = " << inIdComBill
//     << " inPeriodID = " << inPeriodID
//     << " inPeriodMode = " << inPeriodMode
//     << " inPeriodBegin = " << inPeriodBegin
//     << " inPeriodEnd = " << inPeriodEnd
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
    << " inIdComBill = " << inIdComBill
    << " inPeriodID = " << inPeriodID
    << " inPeriodMode = " << inPeriodMode
    << " inPeriodBegin = " << inPeriodBegin
    << " inPeriodEnd = " << inPeriodEnd
    );

// Таг периода оплаты
// struct S_tagPeriod {
//   std::string					periodID;         // N Идентификатор периода (№ п/п). Уникальный в пределах тага и сообщения.
//   std::string					periodMode;       // N Режим тага 
//   TPosDateTime			  periodBegin;      // A Дата в формате dd.mm.yyyy  - начало периода , за который выполняется оплата
//   TPosDateTime        periodEnd;        // A Дата в формате dd.mm.yyyy  - окончание периода , за который выполняется оплата
// };

// CREATE TABLE  `tradedb3_1`.`iperiodpay` (
//   `IdComBill` bigint(20) NOT NULL,
//   `PeriodBegin` int(11) NOT NULL,
//   `PeriodEnd` int(11) NOT NULL,
//   `PeriodMode` int(11) NOT NULL
// ) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

  // Формируем текст запроса 
  SQL << CLEAR;
  SQL << "delete from iperiodpay where "
    << " IdComBill = " << inIdComBill
//     << " and idCounter = " << inidCounter
    ;

  // Выполняем запрос
  Result = ExecSQLRequest (	
    SQL.str()
    );

  CHECK_ERROR_RETURN(Result);

  // Формируем текст запроса 
  SQL << CLEAR;
  SQL << "insert into iperiodpay (IdComBill, PeriodMode, PeriodBegin, PeriodEnd ) "
    << " values("
    << " " << inIdComBill
    << ", " << inPeriodMode
    << ", " << ToSQLString(TPosDateTime_To_DateTimeString(inPeriodBegin))
    << ", " << ToSQLString(TPosDateTime_To_DateTimeString(inPeriodEnd))
    << ")"
    ;

  // Выполняем запрос
  Result = ExecSQLRequest (	
    SQL.str()
    );

  CHECK_ERROR_RETURN(Result);

//   Inform(MS 
//     << SQLProcedureName
//     << " output parametr"
//     //    << " OutIdService = " << OutIdService
//     ,ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS
    //    << " OutIdService = " << OutIdService
    );

  return Result;
}

//----------------------------------------------------------------
// Сохранить информацию от клиента
// Вход
//     inIdComBill - номер счета в бд
//     tag - данные тага
// Выход
//		результат функции - результат выполнения операции
//----------------------------------------------------------------
int TGivsDatabase::StoreInfoFromClient (
  std::string	      inIdComBill,
  TagInfoFromClient	tagList
  )
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::StoreInfoFromClient : ";
  int										Result = OK;
  std::string						SQLProcedureName = "StoreInfoFromClient";

  BaseMsstream					SQL;
  TSQLResults						SQLResults;
  //  S_PAYED_SUM           payedSum;
  S_tagInfoFromClientData tag;

  // Если таг пустой то выходим
  if (tagList.size() == 0) {
    return Result;
  }

  tag = tagList.begin()->second;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " inIdComBill = " << inIdComBill
//     << " mode = " << tag.mode
//     << " InfoName = " << tag.InfoName
//     << " InfoValue = " << tag.InfoValue
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
    << " inIdComBill = " << inIdComBill
    << " mode = " << tag.mode
    << " InfoName = " << tag.InfoName
    << " InfoValue = " << tag.InfoValue
    );

// Информация от клиента
// struct S_tagInfoFromClient {
//   std::string					mode;             // N
//   std::string					InfoName;         // A Название информационного поля (что спрашивать у клиента)
//   std::string					InfoValue;        // A Данные, введенные клиентом. ( В запросе 112
// };

// CREATE TABLE  `tradedb3_1`.`itagicpay` (
//   `IdComBill` bigint(20) NOT NULL,
//   `icmode` tinyint(4) DEFAULT NULL,
//   `InfoName` varchar(64) DEFAULT NULL,
//   `InfoValue` varchar(64) DEFAULT NULL,
//   PRIMARY `indIdComBill` (`IdComBill`)
// ) ENGINE=InnoDB DEFAULT CHARSET=cp1251 COMMENT='табл. для сохранения информ., полученной от клиента';

  // Формируем текст запроса 
  SQL << CLEAR;
  SQL << "delete from itagicpay where "
    << " IdComBill = " << inIdComBill
//     << " and idCounter = " << inidCounter
    ;

  // Выполняем запрос
  Result = ExecSQLRequest (	
    SQL.str()
    );

  CHECK_ERROR_RETURN(Result);

  // Формируем текст запроса 
  SQL << CLEAR;
  SQL << "insert into iperiodpay (IdComBill, icmode, InfoName, InfoValue ) "
    << " values("
    << " " << inIdComBill
    << ", " << tag.mode
    << ", " << tag.InfoName
    << ", " << tag.InfoValue
    << ")"
    ;

  // Выполняем запрос
  Result = ExecSQLRequest (	
    SQL.str()
    );

  CHECK_ERROR_RETURN(Result);

//   Inform(MS 
//     << SQLProcedureName
//     << " output parametr"
//     //    << " OutIdService = " << OutIdService
//     ,ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS
    //    << " OutIdService = " << OutIdService
    );

  return Result;
}

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
int TGivsDatabase::StorePayTagList (
  std::string                 idComBill,
  std::string                 idService,
  bool							          isChequeOwner,
  bool							          isTagCounter,
  bool							          isTagPeriod,
  bool							          isTagInfoFromClient,
  bool							          isTagPaySubSrvFix,
  bool							          isTagPaySubSrvCount,
  bool							          isTagPayDept
  )
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::StorePayTagList : ";
  int										Result = OK;
  std::string						SQLProcedureName = "StorePayTagList";

  BaseMsstream					SQL;
  TSQLResults						SQLResults;
  //  S_PAYED_SUM           payedSum;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " idComBill = " << idComBill
//     << " idService = " << idService
//     << " isChequeOwner = " << isChequeOwner
//     << " isTagCounter = " << isTagCounter
//     << " isTagPeriod = " << isTagPeriod
//     << " isTagInfoFromClient = " << isTagInfoFromClient
//     << " isTagPaySubSrvFix = " << isTagPaySubSrvFix
//     << " isTagPaySubSrvCount = " << isTagPaySubSrvCount
//     << " isTagPayDept = " << isTagPayDept 
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
    << " idComBill = " << idComBill
    << " idService = " << idService
    << " isChequeOwner = " << isChequeOwner
    << " isTagCounter = " << isTagCounter
    << " isTagPeriod = " << isTagPeriod
    << " isTagInfoFromClient = " << isTagInfoFromClient
    << " isTagPaySubSrvFix = " << isTagPaySubSrvFix
    << " isTagPaySubSrvCount = " << isTagPaySubSrvCount
    << " isTagPayDept = " << isTagPayDept 
    );

// CREATE TABLE  `tradedb3_1`.`itransactionscomm` (
//   `IdComBill` bigint(20) NOT NULL DEFAULT '0',
//   `IdService` int(11) NOT NULL,
//   `tagCounter` tinyint(4) DEFAULT '0' COMMENT '0 -счетчиков нет, 1 -счетчики есть',
//   `tagPeriod` tinyint(4) DEFAULT '0' COMMENT '0 - периодов нет, 1 - период есть',
//   `tagInfoFromClient` tinyint(4) DEFAULT '0' COMMENT '0 - тага инф.от клиента нет, 1 - есть',
//   `tagPaySubSrvFix` tinyint(4) DEFAULT '0' COMMENT '0 - нет тага PF, 1 есть',
//   `tagPaySubSrvCount` tinyint(4) DEFAULT '0' COMMENT '0 - нет тага PC, 1 - есть',
//   `tagPayDept` tinyint(4) DEFAULT '0' COMMENT '0 - нет тага PD, 1 - есть',
//   `ChequeOwner` tinyint(4) DEFAULT '0' COMMENT '0 - чек формирует ТрэйдСерв., 1 - чек получаем от Поставщика',
//   PRIMARY KEY (`IdComBill`) USING BTREE
// ) ENGINE=InnoDB DEFAULT CHARSET=cp1251;

  // Формируем текст запроса 
  SQL << CLEAR;
  SQL << "delete from itransactionscomm where "
    << " IdComBill = " << idComBill
//     << " and idCounter = " << inidCounter
    ;

  // Выполняем запрос
  Result = ExecSQLRequest (	
    SQL.str()
    );

  CHECK_ERROR_RETURN(Result);

  // Формируем текст запроса 
  SQL << CLEAR;
  SQL << "insert into itransactionscomm (IdComBill, IdService, ChequeOwner, tagCounter, tagPeriod, tagInfoFromClient, "
    << " tagPaySubSrvFix, tagPaySubSrvCount, tagPayDept) "
    << " values("
    << " " << idComBill
    << ", " << idService
    << ", " << isChequeOwner
    << ", " << isTagCounter
    << ", " << isTagPeriod
    << ", " << isTagInfoFromClient
    << ", " << isTagPaySubSrvFix
    << ", " << isTagPaySubSrvCount
    << ", " << isTagPayDept
    << ")"
    ;

  // Выполняем запрос
  Result = ExecSQLRequest (	
    SQL.str()
    );

  CHECK_ERROR_RETURN(Result);

//   Inform(MS 
//     << SQLProcedureName
//     << " output parametr"
//     //    << " OutIdService = " << OutIdService
//     ,ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS
    //    << " OutIdService = " << OutIdService
    );

  return Result;
}

//----------------------------------------------------------------
// Поставить статус транзакции в бд
// Вход
//    inIdComBill - номер транзакции в бд
//		transactionStatus - статус транзакции
// Выход
//		результат функции - результат выполнения операции
//----------------------------------------------------------------
int TGivsDatabase::SetTransactionsCommStatus (	
  std::string					inIdComBill,
  eTransactionStatus  transactionStatus
  )
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::SetTransactionsCommStatus : ";
  int										Result = OK;
  std::string						SQLProcedureName = "SetTransactionsCommStatus";

  msstream							SQL;
  TSQLResults						SQLResults;
  //  int										Col;
  //	int										intValue;
  std::string           stValue;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " inIdComBill = " << inIdComBill
//     << " transactionStatus = " << transactionStatus
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
    << " inIdComBill = " << inIdComBill
    << " transactionStatus = " << transactionStatus
    );

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "update TransactionsComm set "
    << " TransactionStatus = " << transactionStatus
    << " where IdComBill =  " << inIdComBill;

  // Выполняем запрос
  Result = ExecSQLRequest (	
    SQL.str()
    );

  CHECK_ERROR_RETURN(Result);

  //   // Вычитываем данные запроса
  //   Col = 0;
  //   Result = GetField (SQLResults, 
  //     0, Col, stValue);++Col;
  // 
  //   CHECK_ERROR_RETURN(Result);
  // 
  //   Result = MySQLDateTime_To_TPosDateTime(stValue, OutPayDateTime);
  // 
  //   CHECK_ERROR_RETURN(Result);

//   Inform(MS 
//     << SQLProcedureName
//     << " output parametr"
//     //    << " OutPayDateTime = " << OutPayDateTime
//     , ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS
    //    << " OutPayDateTime = " << OutPayDateTime
    );

  return Result;
}

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
int TGivsDatabase::IntegratorGetOperationState (	
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
)
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TUkrTeleDatabase::IntegratorGetOperationState : ";
  int										Result = OK;

  std::string						SQLProcedureName = "IntegratorGetOperationState_v2";
  msstream							SQL;
  int										EstimatedResultCount = 11;
  TSQLResults						SQLResults;
  int										Col;
  std::string						StValue;
  std::string						stCounterFlag;
  std::string						stReturnVoucher;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " inIdComBill = " << inIdComBill
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
    << " inIdComBill = " << inIdComBill
    );

// CREATE PROCEDURE IntegratorGetOperationState -- version 1.0
// -- from 19.03.2014
// (IN inIdComBill bigint
// )

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "call "
    << SQLProcedureName
    << "("
    << " " << inIdComBill
    << ")";

  Result = ExecStoredProcedure (	
    SQL.str(),
    EstimatedResultCount,
    SQLResults
    );

  CHECK_ERROR_RETURN(Result);

//   SELECT
//     exec_result,
//     error_msg,

//     outUniKioskStatus,
//     outIdComBill,
//     outIdService,
//     outIdSubService,
//     outIdClient,
//     outAmount,
//     outAmountCommis,
//     outDateOperation
//   ;


	// Вычитываем данные запроса
	Col = 2;

  Result = GetField (SQLResults, 
    0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  outUniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

  Result = GetField (SQLResults, 
    0, Col++, outIdComBill);
  CHECK_ERROR_RETURN(Result);

  Result = GetField (SQLResults, 
    0, Col++, outIdService);
  CHECK_ERROR_RETURN(Result);
        
  Result = GetField (SQLResults, 
    0, Col++, outIdSubService);
  CHECK_ERROR_RETURN(Result);
    
  Result = GetField (SQLResults, 
    0, Col++, outIdClient);
  CHECK_ERROR_RETURN(Result);
    
  Result = GetField (SQLResults, 
    0, Col++, outAmount);
  CHECK_ERROR_RETURN(Result);
    
  Result = GetField (SQLResults, 
    0, Col++, outAmountCommis);
  CHECK_ERROR_RETURN(Result);
    
  Result = GetField (SQLResults, 
    0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  if (StValue == "NULL") {
    Result = DateYYYY_MM_DD_dot_To_TPosDateTime("2000.01.01", outDateOperation);
    CHECK_ERROR_RETURN(Result);
  } else {
    Result = MySQLDateTime_To_TPosDateTime(StValue, outDateOperation);
    CHECK_ERROR_RETURN(Result);
  }
    
  Result = GetField (SQLResults, 
    0, Col++, outIdCompany);
  CHECK_ERROR_RETURN(Result);
  
//   Inform(MS 
//     << SQLProcedureName 
//     << " output parametr"
//     << " outUniKioskStatus = " << outUniKioskStatus
//     << " outIdComBill = " << outIdComBill
//     << " outIdService = " << outIdService
//     << " outIdSubService = " << outIdSubService
//     << " outIdClient = " << outIdClient
//     << " outAmount = " << outAmount
//     << " outAmountCommis = " << outAmountCommis
//     << " outDateOperation = " << TPosDateTime_To_DateTimeString(outDateOperation)
//     << " outIdCompany = " << outIdCompany
//     , ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS
    << " outUniKioskStatus = " << outUniKioskStatus
    << " outIdComBill = " << outIdComBill
    << " outIdService = " << outIdService
    << " outIdSubService = " << outIdSubService
    << " outIdClient = " << outIdClient
    << " outAmount = " << outAmount
    << " outAmountCommis = " << outAmountCommis
    << " outDateOperation = " << TPosDateTime_To_DateTimeString(outDateOperation)
    << " outIdCompany = " << outIdCompany
    );

  return Result;
}

//----------------------------------------------------------------
int TGivsDatabase::IntegratorGetOperationState(
  std::string inIdComBill,

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
)
{
  TID					ClassID = tidDATABASE;
  std::string	ProcedureName = "TUkrTeleDatabase::IntegratorGetOperationState : ";
  int					Result = OK;

  std::string	SQLProcedureName = "IntegratorGetOperationState_v2";
  msstream		SQL;
  int					EstimatedResultCount = 11;
  TSQLResults	SQLResults;
  int					Col = 0;
  std::string	StValue;
  std::string	stCounterFlag;
  std::string	stReturnVoucher;

  //   Inform(MS 
  //     << SQLProcedureName
  //     << " in parametr"
  //     << " inIdComBill = " << inIdComBill
  //     , ClassID, mtMessage);

  LogInputParametr(SQLProcedureName, MS << ": inIdComBill = " << inIdComBill );

  // CREATE PROCEDURE IntegratorGetOperationState -- version 1.0
  // -- from 19.03.2014
  // (IN inIdComBill bigint
  // )

    // Формируем текст запроса
  SQL << CLEAR;
  SQL << "CALL " << SQLProcedureName  << "( " << inIdComBill << " ); ";

  Result = ExecStoredProcedure( SQL.str(), EstimatedResultCount, SQLResults );
  CHECK_ERROR_RETURN(Result);

  //   SELECT
  //     exec_result,
  //     error_msg,
  //
  //     outUniKioskStatus,
  //     outIdComBill,
  //     outIdService,
  //     outIdSubService,
  //     outIdClient,
  //     outAmount,
  //     outAmountCommis,
  //     outDateOperation

  // Вычитываем данные запроса
  Col = 1;

  Result = GetField(SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  outMsgError = StValue;

  Result = GetField(SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  outUniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

  Result = GetField(SQLResults, 0, Col++, outIdComBill);
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col++, outIdService);
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col++, outIdSubService);
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col++, outIdClient);
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col++, outAmount);
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col++, outAmountCommis);
  CHECK_ERROR_RETURN(Result);

  Result = GetField(SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  if (StValue == "NULL") {
    Result = DateYYYY_MM_DD_dot_To_TPosDateTime("2000.01.01", outDateOperation);
    CHECK_ERROR_RETURN(Result);
  }
  else {
    Result = MySQLDateTime_To_TPosDateTime(StValue, outDateOperation);
    CHECK_ERROR_RETURN(Result);
  }

  Result = GetField(SQLResults, 0, Col++, outIdCompany);
  CHECK_ERROR_RETURN(Result);

  //   Inform(MS 
  //     << SQLProcedureName 
  //     << " output parametr"
  //     << " outUniKioskStatus = " << outUniKioskStatus
  //     << " outIdComBill = " << outIdComBill
  //     << " outIdService = " << outIdService
  //     << " outIdSubService = " << outIdSubService
  //     << " outIdClient = " << outIdClient
  //     << " outAmount = " << outAmount
  //     << " outAmountCommis = " << outAmountCommis
  //     << " outDateOperation = " << TPosDateTime_To_DateTimeString(outDateOperation)
  //     << " outIdCompany = " << outIdCompany
  //     , ClassID, mtMessage);

  LogOutputParametr(SQLProcedureName, MS 
    << ": outUniKioskStatus = " << outUniKioskStatus 
    << " outIdComBill = " << outIdComBill
    << " outIdService = " << outIdService
    << " outIdSubService = " << outIdSubService
    << " outIdClient = " << outIdClient
    << " outAmount = " << outAmount
    << " outAmountCommis = " << outAmountCommis
    << " outDateOperation = " << TPosDateTime_To_DateTimeString(outDateOperation)
    << " outIdCompany = " << outIdCompany
    << " outMsgError = " << outMsgError
  );

  return Result;
}
//----------------------------------------------------------------
// Отметить в бд результат акцептации по молотилке
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
int TGivsDatabase::AcceptMillIntegrator (	
    std::string					inAmountPayed,
    std::string					inAmountCommis,
    std::string					inIdBuyer,
    std::string					inExtReqNo,
    std::string					inMillOperationId,
    eMillAcceptStatus   inMillAcceptStatus
)
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TUkrTeleDatabase::AcceptMillIntegrator : ";
  int										Result = OK;

#ifdef USE_OLD_MILL_REQUEST
  std::string						SQLProcedureName = "AcceptMillIntegrator";
#else // USE_OLD_MILL_REQUEST
  std::string						SQLProcedureName = "AcceptMillIntegrator_v2";
#endif // USE_OLD_MILL_REQUEST

  msstream							SQL;
  int										EstimatedResultCount = 2;
  TSQLResults						SQLResults;
  int										Col = 0;
  std::string						StValue;
  std::string						stCounterFlag;
  std::string						stReturnVoucher;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " inAmountPayed = " << inAmountPayed
//     << " inAmountCommis = " << inAmountCommis
//     << " inIdBuyer = " << inIdBuyer
//     << " inExtReqNo = " << inExtReqNo
//     << " inMillOperationId = " << inMillOperationId
//     << " inMillAcceptStatus = " << inMillAcceptStatus
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
    << " inAmountPayed = " << inAmountPayed
    << " inAmountCommis = " << inAmountCommis
    << " inIdBuyer = " << inIdBuyer
    << " inExtReqNo = " << inExtReqNo
    << " inMillOperationId = " << inMillOperationId
    << " inMillAcceptStatus = " << inMillAcceptStatus
    );

// CREATE PROCEDURE `AcceptMillIntegrator` (IN inAmountPayed numeric(10, 2),
// IN inAmountCommis numeric(10, 2),
// IN inIdBuyer smallint,
// IN inExtReqNo varchar(40),
// IN inMillOperationId bigint,
// IN inMillAcceptStatus tinyint)
// -- version 1.0 
// -- from 06.06.2014

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "call "
    << SQLProcedureName
    << "("
    << " " << inAmountPayed
    << "," << inAmountCommis
    << "," << inIdBuyer
    << "," << ToSQLString(inExtReqNo)
    << "," << inMillOperationId
    << "," << inMillAcceptStatus
    << ")";

  Result = ExecStoredProcedure (	
    SQL.str(),
    EstimatedResultCount,
    SQLResults
    );

  CHECK_ERROR_RETURN(Result);

//   SELECT
//     exec_result,
//     error_msg,



	// Вычитываем данные запроса
// 	Col = 2;
// 
//   Result = GetField (SQLResults, 
//     0, Col++, StValue);
//   CHECK_ERROR_RETURN(Result);

    
//   Inform(MS 
//     << SQLProcedureName 
//     << " output parametr"
// //     << " outUniKioskStatus = " << outUniKioskStatus
//     , ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS
//     << " outUniKioskStatus = " << outUniKioskStatus
    );

  return Result;
}

//----------------------------------------------------------------
int TGivsDatabase::AcceptMillIntegrator (	
    std::string					inAmountPayed,
    std::string					inAmountCommis,
    std::string					inIdBuyer,
    std::string					inExtReqNo,
    std::string					inMillOperationId,
    eMillAcceptStatus   inMillAcceptStatus,

    std::string& outMsgError
)
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TUkrTeleDatabase::AcceptMillIntegrator : ";
  int										Result = OK;

#ifdef USE_OLD_MILL_REQUEST
  std::string						SQLProcedureName = "AcceptMillIntegrator";
#else // USE_OLD_MILL_REQUEST
  std::string						SQLProcedureName = "AcceptMillIntegrator_v2";
#endif // USE_OLD_MILL_REQUEST

  msstream							SQL;
  int										EstimatedResultCount = 2;
  TSQLResults						SQLResults;
  int										Col = 0;
  std::string						StValue;
  std::string						stCounterFlag;
  std::string						stReturnVoucher;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " inAmountPayed = " << inAmountPayed
//     << " inAmountCommis = " << inAmountCommis
//     << " inIdBuyer = " << inIdBuyer
//     << " inExtReqNo = " << inExtReqNo
//     << " inMillOperationId = " << inMillOperationId
//     << " inMillAcceptStatus = " << inMillAcceptStatus
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
    << " inAmountPayed = " << inAmountPayed
    << " inAmountCommis = " << inAmountCommis
    << " inIdBuyer = " << inIdBuyer
    << " inExtReqNo = " << inExtReqNo
    << " inMillOperationId = " << inMillOperationId
    << " inMillAcceptStatus = " << inMillAcceptStatus
    );

// CREATE PROCEDURE `AcceptMillIntegrator` (IN inAmountPayed numeric(10, 2),
// IN inAmountCommis numeric(10, 2),
// IN inIdBuyer smallint,
// IN inExtReqNo varchar(40),
// IN inMillOperationId bigint,
// IN inMillAcceptStatus tinyint)
// -- version 1.0 
// -- from 06.06.2014

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "CALL "
    << SQLProcedureName
    << "("
    << " " << inAmountPayed
    << "," << inAmountCommis
    << "," << inIdBuyer
    << "," << ToSQLString(inExtReqNo)
    << "," << inMillOperationId
    << "," << inMillAcceptStatus
    << "); ";

  Result = ExecStoredProcedure ( SQL.str(), EstimatedResultCount, SQLResults );
  CHECK_ERROR_RETURN(Result);

//   SELECT
//     exec_result,
//     error_msg,
	// Вычитываем данные запроса
 	Col = 1;
 
  Result = GetField (SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  outMsgError = StValue;

    
//   Inform(MS 
//     << SQLProcedureName 
//     << " output parametr"
// //     << " outUniKioskStatus = " << outUniKioskStatus
//     , ClassID, mtMessage);

  LogOutputParametr (SQLProcedureName, MS << ": outMsgError = " << outMsgError );

  return Result;
}

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
int TGivsDatabase::getBuyerLimit (	
  std::string inKeyValue,

  std::string &outIdBuyer,
  std::string &outLimAccount,
  std::string &outStLimit
  )
{
	TID										ClassID = tidDATABASE;
	std::string						ProcedureName = "TGivsDatabase::getBuyerLimit : ";
	int										Result = OK;

	std::string						SQLProcedureName = "getBuyerLimit";
	msstream							SQL;
	TSQLResults						SQLResults;

// 	Inform(MS 
// 		<< SQLProcedureName
// 		<< " in parametr"
// 		<< " inKeyValue = " << inKeyValue
// 		, ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS
		<< " inKeyValue = " << inKeyValue
    );

	// Формируем текст запроса
	SQL << CLEAR;
	SQL << "select idbuyer, LimAccount, StAccount from buyers where "
    << " IdBuyer = (select idbuyer from signkeys where "
		<< " KeyValue = " << inKeyValue
    << ")"
		;

	// Выполняем запрос
	Result = ExecSQLRequest (	
		SQL.str(),
		SQLResults
		);

	CHECK_ERROR_RETURN(Result);

	// Если отсутстуют данные
	if (SQLResults.size() == 0) {
		outLimAccount = "1000000.00";
    outStLimit = "1";

    Result = ERROR_UNKNOWN_BUYER;

	  Inform(MSML
		  << " Байер не прописан в таблице buyers, signkeys"
		  << " inKeyValue = " << inKeyValue
		  , ClassID, mtMessage); 

    return Result;
	}
	else {
		// Вычитываем данные запроса
	  int Col = 0;

    Result = GetField (SQLResults,0, Col++, outIdBuyer);
    CHECK_ERROR_RETURN(Result);

    Result = GetField (SQLResults,0, Col++, outLimAccount);
    CHECK_ERROR_RETURN(Result);

    Result = GetField (SQLResults,0, Col++, outStLimit);
    CHECK_ERROR_RETURN(Result);
	}

// 	Inform(MS 
// 		<< SQLProcedureName
// 		<< " output parametr"
//     << " outIdBuyer = " << outIdBuyer
// 		<< " outLimAccount = " << outLimAccount
//     << " outStLimit = " << outStLimit
// 		, ClassID, mtMessage); 

  LogOutputParametr (SQLProcedureName, MS
    << " outIdBuyer = " << outIdBuyer
		<< " outLimAccount = " << outLimAccount
    << " outStLimit = " << outStLimit
    );

	return Result;
}
//----------------------------------------------------------------
// Вернуть KMType - тип размер RSA ключа
// Вход
//		InIdBuyer - идентификатор Агента
// Выход
//		OutKMType - тип размер RSA ключа
//		результат функции - результат выполнения операции
//----------------------------------------------------------------
int TGivsDatabase::GetKMType ( std::string InIdBuyer, eKMType	&OutKMType )
{
	TID					ClassID = tidDATABASE;
	std::string	ProcedureName = "TGivsDatabase::GetKMType : ";
	int					Result = OK;
	std::string	SQLProcedureName = "GetKMType";

	BaseMsstream SQL;
	TSQLResults	 SQLResults;

  std::string strValue;

	Inform(MS << SQLProcedureName << " in parameter" << " InIdBuyer = " << InIdBuyer, ClassID, mtMessage);

	// Формируем текст запроса
	SQL << CLEAR;
	SQL << "select KMType from Buyers where IdBuyer = " << InIdBuyer;

	// Выполняем запрос
	Result = ExecSQLRequest(
		SQL.str(),
		SQLResults
	);

	CHECK_ERROR_RETURN(Result);

	// Если такой ваучер вообще отсутствует
	if (IsNullSQLResult(SQLResults)) {

		Result = ERROR_BUYER_TYPE;

		Inform(MSML << "Ошибка неизвестный тип Партнера или IdBuyer. InIdBuyer = " << InIdBuyer, ClassID, mtError, Result);

		return Result;
	}

	// Вычитываем данные запроса
	Result = GetField( SQLResults, 0, 0, strValue );
	CHECK_ERROR_RETURN(Result);

  if ( strValue.size() > 0 )
    OutKMType = (eKMType)StringToInt(strValue);
  else
    OutKMType = keyKmUnknown;


	Inform(MS << SQLProcedureName << " output parameter: OutBuyerType = " << strValue, ClassID, mtMessage);

	return Result;
}
//----------------------------------------------------------------
// Поставить статус транзакции в бд
// Вход
//    inIdComBill - номер транзакции в бд
//		inDateOp    - Облікова дата операції
// Выход
//		результат функции - результат выполнения операции
//----------------------------------------------------------------
int TGivsDatabase::SetTransactionsCommDateOp (	
    std::string	 inIdComBill,
    TPosDateTime inDateOp
  )
{
  int	Result  = OK;
  TID	ClassID = tidDATABASE;
  std::string	ProcedureName    = "TGivsDatabase::SetTransactionsCommDateOp : ";  
  std::string	SQLProcedureName = "SetTransactionsCommDateOp";

  msstream		SQL;
  TSQLResults SQLResults;
  //  int			Col;
  //	int			intValue;
  std::string stValue;

//   Inform(MS 
//     << SQLProcedureName
//     << " in parametr"
//     << " inIdComBill = " << inIdComBill
//     << " transactionStatus = " << transactionStatus
//     , ClassID, mtMessage);

  LogInputParametr (SQLProcedureName, MS << " inIdComBill = " << inIdComBill 
     << ", DateOp = " << ToSQLString(TPosDateTime_To_MySQLDateTime(inDateOp)) << ". "  );

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "update TransactionsComm set "
    << " DateOp = " << ToSQLString(TPosDateTime_To_MySQLDateTime(inDateOp))
    << " where IdComBill =  " << inIdComBill;

  // Выполняем запрос
  Result = ExecSQLRequest (	SQL.str() );
  CHECK_ERROR_RETURN(Result);

  LogOutputParametr (SQLProcedureName, MS
    //    << " OutPayDateTime = " << OutPayDateTime
    );

  return Result;
}
//----------------------------------------------------------------
// Поставить статус транзакции в бд
// Вход
//    inIdComBill - номер транзакции в бд
//		transactionStatus - статус транзакции
// Выход
//		результат функции - результат выполнения операции
//----------------------------------------------------------------
int TGivsDatabase::ag_IntegSetTransactionCommStatus (	
    std::string					inIdComBill,
    TPosDateTime        inDateOp,
    eTransactionStatus  transactionStatus
  )
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::ag_IntegSetTransactionsCommStatus : ";
  int										Result = OK;
  std::string						SQLProcedureName = "ag_IntegSetTransactionCommStatus";

  msstream							SQL;
  int										EstimatedResultCount = 2;
  TSQLResults						SQLResults;  

  int										ExecResult = 0;
  int										Col = 0;
  std::string           stValue;
  std::string						error_msg;
  // -----------------------
  LogInputParametr (SQLProcedureName, MS
    << " inIdComBill = "       << inIdComBill
    << " inDateOp = "          << ToSQLString(TPosDateTime_To_MySQLDateTime(inDateOp))
    << " transactionStatus = " << IntToString ( (int)transactionStatus )
    );
  // -----------------------
  // Формируем текст запроса
  // -----------------------
  SQL << CLEAR;
  SQL << "call " << SQLProcedureName
    << "("
    << " " << inIdComBill
    << "," << ToSQLString(TPosDateTime_To_MySQLDateTime(inDateOp))
    << "," << IntToString ( (int)transactionStatus )
    << ")";

  Result = ExecStoredProcedure (	
    SQL.str(),
    EstimatedResultCount,
    SQLResults
    );

  CHECK_ERROR_RETURN(Result);

  //--------------------------------
  //  SELECT exec_result, error_msg;
	//--------------------------------
	// Вычитываем данные запроса
	//--------------------------------
	Result = GetField (	SQLResults, 0, 1, error_msg);
  CHECK_ERROR_RETURN(Result);
	//--------------------------------
	//
	//--------------------------------
	Result = GetField (	SQLResults, 0, 0, ExecResult);
  CHECK_ERROR_RETURN(Result);
  //--------------------------------

  LogOutputParametr (SQLProcedureName, MS << " error_msg = " << error_msg << ". "  );

  return Result;
}

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
int TGivsDatabase::ag_AddToPayOwner (	
    std::string	 inIdComBill,
    std::string	 inIdBuyer,
    std::string	 inIdService,
    TPosDateTime inPayDate,
    std::string	 inPersone,
    std::string	 inAddress,
    std::string	 inOther
   )
{
   TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::ag_AddToPayOwner : ";
  int										Result = OK;
  std::string						SQLProcedureName = "ag_AddToPayOwner";

  msstream							SQL;
  int										EstimatedResultCount = 2;
  TSQLResults						SQLResults;  

  int										ExecResult = 0;
  int										Col = 0;
  std::string           stValue;
  std::string						error_msg;

  // -----------------------
  LogInputParametr (SQLProcedureName, MS
    << ": inIdComBill = "  << inIdComBill
    << ", inIdBuyer = "   << inIdBuyer
    << ", inIdService = " << inIdService    
    << ", inPayDate = "   << ToSQLString ( TPosDateTime_To_MySQLDateTime(inPayDate) )
    << ", inPersone = "   << inPersone
    << ", inAddress = "   << inAddress
    << ", inOther = "     << inOther
    );
  // -----------------------
  // Формируем текст запроса
  // -----------------------
  SQL << CLEAR;
  SQL << "call " << SQLProcedureName
    << "( " << inIdComBill
    << ", " << inIdBuyer
    << ", " << inIdService
    << ", " << ToSQLString(TPosDateTime_To_MySQLDateTime(inPayDate))
    << ", " << ToSQLString( inPersone )
    << ", " << ToSQLString( inAddress )
    << ", " << ToSQLString( inOther )
    << " )";

  Result = ExecStoredProcedure (	
    SQL.str(),
    EstimatedResultCount,
    SQLResults
    );

  CHECK_ERROR_RETURN(Result);
  //--------------------------------
  //  SELECT exec_result, error_msg;
	//--------------------------------
	// Вычитываем данные запроса
	//--------------------------------
	Result = GetField (	SQLResults, 0, 1, error_msg);
  CHECK_ERROR_RETURN(Result);
	//--------------------------------
	//
	//--------------------------------
	Result = GetField (	SQLResults, 0, 0, ExecResult);
  CHECK_ERROR_RETURN(Result);
  //--------------------------------

  LogOutputParametr (SQLProcedureName, MS << " error_msg = " << error_msg << ". "  );

  return Result;
}
//----------------------------------------------------------------
int TGivsDatabase::ag_AddToPayOwner (	
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
    )
{
   TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::ag_AddToPayOwner+ : ";
  int										Result = OK;
  std::string						SQLProcedureName = "ag_AddToPayOwner_v3";

  msstream							SQL;
  int										EstimatedResultCount = 3;
  TSQLResults						SQLResults;  

  int										ExecResult = 0;
  int										Col = 0;
  std::string           stValue;
  std::string						error_msg;

  // -----------------------
  LogInputParametr (SQLProcedureName, MS
    << ": inIdComBill = "  << inIdComBill
    << ", inIdBuyer = "   << inIdBuyer
    << ", inIdService = " << inIdService
    << ", inAmount = "    << inAmount
    << ", inPayDate = "   << ToSQLString ( TPosDateTime_To_MySQLDateTime(inPayDate) )
    << ", inPersone = "   << inPersone
    << ", inAddress = "   << inAddress
    << ", inOther = "     << inOther
    << ", inPayType = "   << IntToString( inPayType )
    << ", inIdPayFund = " << inIdPayFund
    );
  // -----------------------
  // Формируем текст запроса
  // -----------------------
  SQL << CLEAR;
  SQL << "call " << SQLProcedureName
    << "( " << inIdComBill
    << ", " << inIdBuyer
    << ", " << inIdService
    << ", " << inAmount
    << ", " << ToSQLString(TPosDateTime_To_MySQLDateTime(inPayDate))
    << ", " << ToSQLString( inPersone )
    << ", " << ToSQLString( inAddress )
    << ", " << ToSQLString( inOther )
    << ", " << IntToString( inPayType )
    << ", " << ToSQLString( inIdPayFund )
    << " )";

  Result = ExecStoredProcedure (	
    SQL.str(),
    EstimatedResultCount,
    SQLResults
    );

  CHECK_ERROR_RETURN(Result);
  //--------------------------------
  //  SELECT exec_result, error_msg;
	//--------------------------------
	// Вычитываем данные запроса
	//--------------------------------
	Result = GetField (	SQLResults, 0, 2, error_msg);
  CHECK_ERROR_RETURN(Result);
	//--------------------------------
	//
	//--------------------------------
	Result = GetField (	SQLResults, 0, 1, ExecResult);
  CHECK_ERROR_RETURN(Result);

  if ( ExecResult != 0 )
  {
    Result = ERROR_WRONG_AW_AL_RAG_DATA;
  }
  //--------------------------------

  LogOutputParametr (SQLProcedureName, MS << " error_msg = " << error_msg << ". "  );

  return Result;
}
//----------------------------------------------------------------
int TGivsDatabase::ag_AddToPayOwner(
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

  std::string& outMsgError
)
{
  TID										ClassID = tidDATABASE;
  std::string						ProcedureName = "TGivsDatabase::ag_AddToPayOwner+ : ";
  int										Result = OK;
  std::string						SQLProcedureName = "ag_AddToPayOwner_v3";

  msstream							SQL;
  int										EstimatedResultCount = 3;
  TSQLResults						SQLResults;

  int										ExecResult = 0;
  int										Col = 0;
  std::string           stValue;
  std::string						error_msg;

  // -----------------------
  LogInputParametr(SQLProcedureName, MS
    << ": inIdComBill = " << inIdComBill
    << ", inIdBuyer = " << inIdBuyer
    << ", inIdService = " << inIdService
    << ", inAmount = " << inAmount
    << ", inPayDate = " << ToSQLString(TPosDateTime_To_MySQLDateTime(inPayDate))
    << ", inPersone = " << inPersone
    << ", inAddress = " << inAddress
    << ", inOther = " << inOther
    << ", inPayType = " << IntToString(inPayType)
    << ", inIdPayFund = " << inIdPayFund
  );
  // -----------------------
  // Формируем текст запроса
  // -----------------------
  SQL << CLEAR;
  SQL << "CALL " << SQLProcedureName
    << "( " << inIdComBill
    << ", " << inIdBuyer
    << ", " << inIdService
    << ", " << inAmount
    << ", " << ToSQLString(TPosDateTime_To_MySQLDateTime(inPayDate))
    << ", " << ToSQLString(inPersone)
    << ", " << ToSQLString(inAddress)
    << ", " << ToSQLString(inOther)
    << ", " << IntToString(inPayType)
    << ", " << ToSQLString(inIdPayFund)
    << " ); ";

  Result = ExecStoredProcedure( SQL.str(), EstimatedResultCount, SQLResults );
  CHECK_ERROR_RETURN(Result);
  //--------------------------------
  //  SELECT exec_result, error_msg;
  //--------------------------------
  // Вычитываем данные запроса
  //--------------------------------
  Result = GetField(SQLResults, 0, 2, error_msg);
  CHECK_ERROR_RETURN(Result);

  outMsgError = error_msg;
  //--------------------------------
  //
  //--------------------------------
  Result = GetField(SQLResults, 0, 1, ExecResult);
  CHECK_ERROR_RETURN(Result);

  if (ExecResult != 0)
  {
    Result = ERROR_WRONG_AW_AL_RAG_DATA;
  }
  //--------------------------------

  LogOutputParametr(SQLProcedureName, MS << " error_msg = " << error_msg << ". ");

  return Result;
}
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
int TGivsDatabase::ag_GetTagIPValues(
  std::string inIdComBill,
  std::string inExtReqNo,
  std::string inIdbuyer,
  std::string inIdService,

  TIpDataList& outIpDataList
)
{
  int	Result  = OK;
  TID	ClassID = tidDATABASE;
  std::string	ProcedureName = "TGivsDatabase::ag_GetTagIPValues : ";
  
  std::string	SQLProcedureName = "ag_GetTagIPValues";

  msstream		SQL;
  int					EstimatedResultCount = 11;
  TSQLResults	SQLResults;

  int					ExecResult = 0;
  int					Col = 0;
  std::string stValue;
  std::string	error_msg;
  // -----------------------
  LogInputParametr(SQLProcedureName, MS
    << ": inIdComBill = " << inIdComBill
    << ", inExtReqNo = "  << inExtReqNo
    << ", inIdBuyer = "   << inIdbuyer
    << ", inIdService = " << inIdService
  );
  // -----------------------
  // Формируем текст запроса
  // -----------------------
  SQL << CLEAR;
  SQL << "call " << SQLProcedureName
    << "( " << inIdComBill
    << ", " << ToSQLString(inExtReqNo)
    << ", " << inIdbuyer
    << ", " << inIdService
    << " )";

  Result = ExecStoredProcedure(
    SQL.str(),
    EstimatedResultCount,
    SQLResults
  );

  CHECK_ERROR_RETURN(Result);
  //--------------------------------
  //  SELECT SP_result, result, msg, Type0, Type1, Type2, Type3, Type4, Type5, Type6, Type7;
  //--------------------------------
  // Вычитываем данные запроса, msg
  //--------------------------------
  Result = GetField(SQLResults, 0, 2, error_msg);
  CHECK_ERROR_RETURN(Result);
  //--------------------------------
  // result
  //--------------------------------
  Result = GetField(SQLResults, 0, 1, ExecResult);
  CHECK_ERROR_RETURN(Result);

  if (ExecResult != 0)
  {
    Result = ERROR_WRONG_AW_AL_RAG_DATA;
  }
  //--------------------------------
  // int IPIndexValue = 0, Item = 3;
  S_IP_DATA locIpData;
  //--------------------------------
  // Type0..Type7
  //--------------------------------
  bool isScroogeNegative = false;

  for (int IPIndexValue = 0, Item = 3; Item < EstimatedResultCount; Item++, IPIndexValue++)
  {
    locIpData.infoType = IntToString(IPIndexValue);  
    // IPIndexValue++;
    Result = GetField(SQLResults, 0, Item, stValue); // Item++;
    CHECK_ERROR_RETURN(Result);

    if (stValue.empty() || stValue == "*" || StringToUpper(stValue) == SQL_NULL) {
      continue;
    }

    // Статус виконання
    if (IPIndexValue == 1 && stValue == "-1") {
      isScroogeNegative = true;
      continue;
    }

    // Дата/час виконання платежу, dd.mm.yyyy hh:mm:ss
    if (IPIndexValue == 2 && (stValue.empty() || stValue == "*" || isScroogeNegative == true) )
    {
      continue;
    }

    locIpData.infoValue = stValue;

    outIpDataList[locIpData.infoType] = locIpData;
  }
  //--------------------------------
  LogOutputParametr(SQLProcedureName, MS << " error_msg = " << (error_msg.empty()? "Handled successfully" : error_msg) << ". ");

  return Result;
}
//----------------------------------------------------------------
// #include "Common/CUrlEncode.h"

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
/// <param name="outSendAuthType">Тип аутентифікації - 0 - sms, 1 - phone call</param>
/// <param name="outSmsAmnt">Сума за смс</param>
/// <param name="outSmsText">Смс текст до коду</param>
/// <returns>Результат виконання методу</returns>
int TGivsDatabase::IntegProcess113Stage2Add(
  std::string	inIdComBill,
  std::string	inIdService,
  std::string	inIdBuyer,
  std::string	inAmountPayed,
  std::string	inIdTerm,
  std::string	inIdClient,

  eUniKioskStatus& UniKioskStatus,
  eUniBillStatus&  UniBillStatus,
  std::string&     outMsgError,
  int&         outSendAuthType,
  std::string& outSmsAmnt,
  std::string& outSmsText
)
{
  int	Result  = OK;
  TID	ClassID = tidDATABASE;
  std::string	ProcedureName    = "TUniBillDatabase::IntegProcess113Stage2Add : ";
  std::string	SQLProcedureName = "IntegProcess113Stage2Add_v2";
  
  msstream		SQL;  
  TSQLResults	SQLResults;
  int					EstimatedResultCount = 6;
  
  int					Col = 0;
  std::string	StValue;
  std::string	ErrorMsg;

  outSendAuthType = 0;

  LogInputParametr(SQLProcedureName, MS
    << ": inIdComBill = "   << inIdComBill
    << ", inIdService = "   << inIdService
    << ", inIdBuyer = "     << inIdBuyer
    << ", inAmountPayed = " << inAmountPayed
    << ", inIdTerm = "      << ToSQLString(inIdTerm)
    << ", inIdClient = "    << ToSQLString(inIdClient)
    << ". "
  );

  // CREATE PROCEDURE `IntegProcess113Stage1Add0_v1`(
  //IN inIdComBill bigint,
  //IN inIdService int,
  //IN inIdBuyer int,
  //IN inAmountPayed numeric(10, 2),
  //IN inIdTerm varchar(50))
  // 
  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "CALL "  << SQLProcedureName
    << "( " << inIdComBill
    << ", " << inIdService
    << ", " << inIdBuyer
    << ", " << inAmountPayed
    << ", " << ToSQLString(inIdTerm)
    << ", " << ToSQLString(inIdClient)
    << " ); ";

  Result = ExecStoredProcedure( SQL.str(), EstimatedResultCount, SQLResults );
  CHECK_ERROR_RETURN(Result);

  //SELECT SP_result, OUT_result, OUT_msg;

 // Вычитываем данные запроса
  Col = 1;

  // OUT_result = UniKioskStatus,
  Result = GetField(SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  UniKioskStatus = (eUniKioskStatus)StringToInt(StValue);

  // OUT_msg = error_msg, -> для тага ERR
  Result = GetField(SQLResults, 0, Col++, ErrorMsg);
  CHECK_ERROR_RETURN(Result);

  // OUT_send: 0 - M85 не отправлять, 1 - М85 отправлять
  Result = GetField(SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  outSendAuthType = (int)StringToInt(StValue);

  // OUT_amt - стоимость смс в грн. с точкой
  Result = GetField(SQLResults, 0, Col++, outSmsAmnt);
  CHECK_ERROR_RETURN(Result);

  // OUT_smstxt
  Result = GetField(SQLResults, 0, Col++, StValue);
  CHECK_ERROR_RETURN(Result);

  if (!StValue.empty())
  {
    outSmsText = StValue;
  }

  LogOutputParametr(SQLProcedureName, MS
    << ": OUT_result(UniKioskStatus) = " << UniKioskStatus
    << ", OUT_msg(error_msg) = "         << ErrorMsg
    << ", OUT_send = "                   << ((outSendAuthType == 0) ? "send sms" : "create phone call")
    << ", OUT_smstxt: "                  << outSmsText
    << ". "
  );
  
  //CURLEncode locCURLEncode;
  //outSmsText = locCURLEncode.URLEncode(outSmsText);

  return Result;
}

/// <summary>
/// Отримати код смс та номер телефону для певного статусу. Якщо дані не отримані, то надати помилку вибірки.
/// </summary>
/// <param name="inIdComBill">Ідентифікатор операції</param>
/// <param name="inStatus">Статус коду: 1 - записано телефон для відправки СМС, 2 - записан код для відправки в смс, 3 - смс відправлена, 9 - відміна. код від партнера не приймати, 10 - отриман код в операції. Все Ок, співпало. </param>
/// <param name="outCodeSend">Отриманий з БД код для надсилання клієнту</param>
/// <param name="outPhone">Отриманий з БД номер телефону</param>
/// <returns>Результат виконання методу</returns>
int TGivsDatabase::GetSmsParamsByStatus(std::string inIdComBill, int inStatus, std::string& outCodeSend, std::string& outPhone)
{
  TID					ClassID = tidDATABASE;
  std::string	ProcedureName = "TGivsDatabase::GetSmsParamsByStatus : ";
  int					Result = OK;
  std::string	SQLProcedureName = "GetSmsParamsByStatus";

  BaseMsstream SQL;
  TSQLResults	 SQLResults;

  std::string strValue;

  Inform(MS << SQLProcedureName << " in parameters: IdComBill = " << inIdComBill << " для mstatus = " << IntToString(inStatus) << ". ", ClassID, mtMessage);

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "SELECT code_send, phone FROM SMS_CONTROL WHERE idcombill =" << inIdComBill << " AND mstatus = " << IntToString(inStatus) << "; ";

  // Выполняем запрос
  Result = ExecSQLRequest( SQL.str(), SQLResults );
  CHECK_ERROR_RETURN(Result);

  // Если такой ваучер вообще отсутствует
  if (IsNullSQLResult(SQLResults)) {

    Result = ERROR_BUYER_TYPE;

    Inform(MSML << " Відсутній запис у таблиці за параметрами: IdComBill = " << inIdComBill << " для mstatus = " << IntToString(inStatus) << ". ", ClassID, mtError, Result);

    return Result;
  }

  // Вичитуємо дані з віповіді
  // outCodeSend
  int Col = 0;
  Result = GetField(SQLResults, 0, Col++, outCodeSend);
  CHECK_ERROR_RETURN(Result);

  // outPhone
  Result = GetField(SQLResults, 0, Col++, outPhone);
  CHECK_ERROR_RETURN(Result);
 
  Inform(MS << SQLProcedureName << " output parameters: outCodeSend = " << outCodeSend << ", outPhone = " << outPhone << ". ", ClassID, mtMessage);

  return Result;
}

/// <summary>
/// Оновити статус смс у SMS_CONTROL
/// </summary>
/// <param name="inIdComBill">Ідентифікатор операції</param>
/// <param name="inStatus">Статус коду: 1 - записано телефон для відправки СМС, 2 - записан код для відправки в смс, 3 - смс відправлена, 9 - відміна. код від партнера не приймати, 10 - отриман код в операції. Все Ок, співпало. </param>
/// <returns>Результат виконання методу</returns>
int TGivsDatabase::UpdateSmsParamsByStatus(std::string inIdComBill, int inStatus)
{
  int	Result  = OK;
  TID	ClassID = tidDATABASE;
  std::string	ProcedureName    = "TGivsDatabase::UpdateSmsParamsByStatus : ";
  std::string	SQLProcedureName = "UpdateSmsParamsByStatus";

  msstream		SQL;
  TSQLResults SQLResults;
  std::string stValue;

  LogInputParametr(SQLProcedureName, MS << ": inIdComBill = " << inIdComBill << ", mStatus = " << IntToString(inStatus) << ". ");

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "UPDATE SMS_CONTROL SET mstatus = " << IntToString(inStatus) << ", dt_send = Now() WHERE IdComBill =  " << inIdComBill << "; ";

  // Выполняем запрос
  Result = ExecSQLRequest(SQL.str());
  CHECK_ERROR_RETURN(Result);

  LogOutputParametr(SQLProcedureName, MS  );

  return Result;
}


/// <summary>
/// Отримати статус, код смс та номер телефону за ідентифікатором операції IdComBill. Якщо дані не отримані, то надати помилку вибірки.
/// </summary>
/// <param name="inIdComBill">Ідентифікатор операції</param>
/// <param name="outStatus">Статус коду: 1 - записано телефон для відправки СМС, 2 - записан код для відправки в смс, 3 - смс відправлена, 9 - відміна. код від партнера не приймати, 10 - отриман код в операції. Все Ок, співпало. </param>
/// <param name="outCodeSend">Отриманий з БД код для надсилання клієнту</param>
/// <param name="outPhone">Отриманий з БД номер телефону</param>
/// <param name="outIpPort">IP та PORT для обміну з сервером надсилання дзвінків клієнтові</param>
/// <returns>Результат виконання методу</returns>
int TGivsDatabase::GetSmsParams(std::string inIdComBill, int& outStatus, std::string& outCodeSend, std::string& outPhone, std::string& outIpPort)
{
  int	Result  = OK;
  TID	ClassID = tidDATABASE;
  std::string	ProcedureName    = "TGivsDatabase::GetSmsParams : ";
  std::string	SQLProcedureName = "GetSmsParams";

  BaseMsstream SQL;
  TSQLResults	 SQLResults;

  std::string strValue;

  Inform(MS << SQLProcedureName << " in parameters: IdComBill = " << inIdComBill << ". ", ClassID, mtMessage);

  // Формируем текст запроса
  SQL << CLEAR;
  SQL << "SELECT mstatus, code_send, phone, sadr FROM SMS_CONTROL WHERE idcombill =" << inIdComBill << "; ";

  // Выполняем запрос
  Result = ExecSQLRequest(SQL.str(), SQLResults);
  CHECK_ERROR_RETURN(Result);

  // Если такой ваучер вообще отсутствует
  if (IsNullSQLResult(SQLResults)) {

    // Result = ERROR_BUYER_TYPE;
    outStatus = 99; // return 21 status

    Inform(MSML << " Відсутній запис у таблиці за параметрами: IdComBill = " << inIdComBill << ". ", ClassID, mtError, Result);

    return Result;
  }
  
  int Col = 0;
  // Вичитуємо дані з віповіді
  // outStatus
  Result = GetField(SQLResults, 0, Col++, strValue);
  CHECK_ERROR_RETURN(Result);

  outStatus = StringToInt(strValue);

  // outCodeSend 
  Result = GetField(SQLResults, 0, Col++, outCodeSend);
  CHECK_ERROR_RETURN(Result);

  // outPhone
  Result = GetField(SQLResults, 0, Col++, outPhone);
  CHECK_ERROR_RETURN(Result);

  // sadr
  Result = GetField(SQLResults, 0, Col++, outIpPort);
  CHECK_ERROR_RETURN(Result);

  Inform(MS << SQLProcedureName << " output parameters: outCodeSend = " << outCodeSend << ", outPhone = " << outPhone << ", outStatus = " << IntToString(outStatus) << ", outIpPort = " << outIpPort << ". ", ClassID, mtMessage);

  return Result;
}

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
int TGivsDatabase::Add_AWPAWF(
  std::string	 inIdComBill,
  std::string	 inIdBuyer,
  std::string	 inIdService,
  std::string	 inAmount,
  TPosDateTime inPayDate,
  std::string	 inTagName,
  S_tagAccOwnerPay intagAccOwnerPay,

  eUniKioskStatus& outUniKioskStatus,
  std::string& outMsgError
)
{
  int	Result  = OK;
  TID	ClassID = tidDATABASE;
  std::string	ProcedureName    = "TGivsDatabase::Add_AWPAWF: "; 
  std::string	SQLProcedureName = "Add_AWPAWF_v1";
    
  int					EstimatedResultCount = 3;
  msstream		SQL;
  TSQLResults	SQLResults;

  int	ExecResult = 0;
  int	Col        = 0;

  std::string stValue;
  std::string	error_msg;

  // -----------------------
  LogInputParametr(SQLProcedureName, MS
    << ": inIdComBill = " << inIdComBill
    << ", inIdBuyer = " << inIdBuyer
    << ", inIdService = " << inIdService
    << ", inAmount = "    << inAmount
    << ", inPayDate = "   << ToSQLString(TPosDateTime_To_MySQLDateTime(inPayDate))
    << ", surname = "     << intagAccOwnerPay.surname
    << ", name = "        << intagAccOwnerPay.name
    << ", patronymic = "  << intagAccOwnerPay.patronymic
    << ", doctype = "     << IntToString((int)intagAccOwnerPay.doctype)
    << ", docnumber = "   << intagAccOwnerPay.docnumber
    << ", bithday = "     << (!intagAccOwnerPay.bithday.empty() ? intagAccOwnerPay.bithday : "*")
    << ", bplace = "      << intagAccOwnerPay.bplace
    << ", phone = "       << intagAccOwnerPay.phone
    << ", other = "       << intagAccOwnerPay.other
    << ", ptype = "       << IntToString((int)intagAccOwnerPay.Ptype) 
    << ", inTagName = "   << inTagName
    << ". "    
  );
/*
IN inIdcombill bigint(20),
IN inIdbuyer integer,
IN inIdservice integer,
IN inAmount decimal(12, 4),
IN inPaydate datetime,

IN insurname varchar(64),+
IN inname varchar(64),+
IN inpatronymic varchar(64),+
IN indoctype int ,
IN indocnumb varchar(24),
IN inBithday varchar(10),
in inBPlace  varchar(64),
IN inPhone   varchar(12),
IN inOther varchar(64),
IN inPType integer,
IN inTAG  varchar(3)
*/
  // -----------------------
  // Формируем текст запроса
  // -----------------------
  SQL << CLEAR;
  SQL << "CALL " << SQLProcedureName
    << "( " << inIdComBill
    << ", " << inIdBuyer
    << ", " << inIdService
    << ", " << inAmount
    << ", " << ToSQLString(TPosDateTime_To_MySQLDateTime(inPayDate))
    //
    << ", " << ToSQLString(intagAccOwnerPay.surname)
    << ", " << ToSQLString(intagAccOwnerPay.name)
    << ", " << ToSQLString(intagAccOwnerPay.patronymic)
    << ", " << IntToString((int)intagAccOwnerPay.doctype)
    << ", " << ToSQLString((!intagAccOwnerPay.docnumber.empty() ? intagAccOwnerPay.docnumber : "*"))

    << ", " << ToSQLString((!intagAccOwnerPay.bithday.empty() ? intagAccOwnerPay.bithday : "*"))
    << ", " << ToSQLString((!intagAccOwnerPay.bplace.empty() ? intagAccOwnerPay.bplace : "*"))
    << ", " << ToSQLString((!intagAccOwnerPay.phone.empty() ? intagAccOwnerPay.phone : "*"))
    << ", " << ToSQLString((!intagAccOwnerPay.other.empty() ? intagAccOwnerPay.other : "*"))
    << ", " << IntToString((int)intagAccOwnerPay.Ptype) 
  
    << ", " << ToSQLString(inTagName)
    << " ); ";

  Result = ExecStoredProcedure(SQL.str(), EstimatedResultCount, SQLResults);
  CHECK_ERROR_RETURN(Result);
  //--------------------------------
  //  SELECT exec_result, error_msg;
  //--------------------------------
  // Вычитываем данные запроса
  //--------------------------------
  Result = GetField(SQLResults, 0, 2, error_msg);
  CHECK_ERROR_RETURN(Result);

  outMsgError = error_msg;
  //--------------------------------
  Result = GetField(SQLResults, 0, 1, ExecResult);
  CHECK_ERROR_RETURN(Result);

  outUniKioskStatus = (eUniKioskStatus)ExecResult;
  //--------------------------------
  LogOutputParametr(SQLProcedureName, MS << " error_msg = " << error_msg << ". ");

  return Result;
}
