#include "integrator_pch.h"

#ifdef  TEST_SERVER

  // Парсинг запросов
  #include "Requests/BaseWebserverRequests.h"

  // Работа с Https запросами
  #include "Http/ExecuteHttpRequest.h"

#endif

#include "IntegratorServer_i.h"

#ifdef USE_OLD_MILL_REQUEST

  #error Этот проект компилится только с отключенным макросом USE_OLD_MILL_REQUEST

#endif // USE_OLD_MILL_REQUEST

#ifdef DISABLE_SEQUENCER

  #error Этот проект компилится только с отключенным макросом DISABLE_SEQUENCER

#endif // USE_OLD_MILL_REQUEST



//---------------------------------------------------------------------------
// Конструктор
//---------------------------------------------------------------------------
GivsServer_i::GivsServer_i(std::string aServerName) : BaseServer_i(aServerName)
{
	// Создаем класс для парсинга параметров сервера
	BaseParametrs.reset(new TGivsParametrs(ParamsFileName));
}

//---------------------------------------------------------------------------
// Деструктор
//---------------------------------------------------------------------------
GivsServer_i::~GivsServer_i(void)
{

}

//---------------------------------------------------------------------------
// Инициализация базы данных
//---------------------------------------------------------------------------
int GivsServer_i::InitDatabase (void)
{
	TID										ClassID = tidCOMMON;
	std::string						ProcedureName = "InitDatabase : ";
	int										Result = OK;

#ifdef USE_OLD_DB_CONNECT
 	mysqlpp::Connection		con(false);
#else
  mysqlpp::Connection*		con = NULL;
#endif

	Result = BaseServer_i::InitDatabase();

	CHECK_ERROR_RETURN(Result);

	Inform(MS << "Try to connect to Database...",
		ClassID, mtMessage,	0, DsLog | DsMon);

	if (BASE_SERVER_CONF->DatabasePassword == "NULL") {
		BASE_SERVER_CONF->DatabasePassword = "";
	}

	SERVER_OBJECTS->Database.reset(new TGivsDatabase(
		BASE_SERVER_CONF->DatabaseHost,
		BASE_SERVER_CONF->DatabasePort,
		BASE_SERVER_CONF->DatabaseName,
		BASE_SERVER_CONF->DatabaseLogin,
		BASE_SERVER_CONF->DatabasePassword
		));
	
	Result = SERVER_OBJECTS->Database->ConnectDB(con);

	CHECK_ERROR_RETURN(Result);

	// Закрываем соединение к базе данных
	//con.close();
	Result = SERVER_OBJECTS->Database->DisconnectDB(con);

	CHECK_ERROR_RETURN(Result);

	Inform(MS << "Database interface initialized successfully.",
		ClassID, mtMessage,	0, DsLog | DsMon);

// #endif // EMULATE_DATABASE

	return Result;
}

//---------------------------------------------------------------------------
// Инициализация и запуск потоков Listener для клиента с проверкой подписи
//---------------------------------------------------------------------------
int GivsServer_i::InitSignedListener (void)
{
	TID											ClassID = tidCOMMON;
	std::string							ProcedureName = "GivsServer_i::InitSignedListener : ";
	int                     Result = OK;

	Givs_Signed_Listener_ACCEPTOR	*Client_Listener_acceptor;
	msstream								iplisten(30);

	Inform(MS << "Start open Client Signed Listener...",
		ClassID, mtMessage,	0, DsLog | DsMon);

	if (Givs_Signed_Listener_server_factory_.open() == -1) {
		Result = ERROR_OPEN_LISTENER_FACTORY;
		Inform(MS << "Error open Listener_server_factory."
			, ClassID
			, mtError, Result, DsLog | DsMon);

		return Result;
	}

	iplisten << SERVER_CONF->SignedIpAddress << ":"
		<< SERVER_CONF->SignedPort;

	ACE_INET_Addr local_host(iplisten.str());

	Client_Listener_acceptor = &SERVER_OBJECTS->Client_Signed_Listener_acceptor;

	// Open the Acceptor factory.
	if (Client_Listener_acceptor->open(local_host,
		TAO_ORB_Core_instance()->reactor(),
		Givs_Signed_Listener_server_factory_.creation_strategy(), 0,
		Givs_Signed_Listener_server_factory_.concurrency_strategy(), 0) == -1) {

			Result = ERROR_OPEN_LISTENER_ACCEPTOR;

			Inform(MS << "Error open Signed Listener_acceptor - server already run or invalid IP address."
        << "\nIPAddress = " << SERVER_CONF->SignedIpAddress
        << "\nPort = " << SERVER_CONF->SignedPort
				, ClassID
				, mtError, Result, DsLog | DsMon);

			return Result;
	}

  BASE_SERVER_CONF->serverPorts += " " + IntToString(SERVER_CONF->SignedPort);

	Inform(MS << "Client Signed Listener started successfully.",
		ClassID, mtMessage,	0, DsLog | DsMon);

	return Result;
}

//---------------------------------------------------------------------------
// Инициализация и запуск потоков пула задач сервера
//---------------------------------------------------------------------------
int GivsServer_i::InitSendThread (void)
{
  TID											ClassID = tidCOMMON;
  std::string							ProcedureName = "UniBillServer_i::InitSendThread : ";
  int                     Result = OK;

  Inform(MS << "Start init task pool...",
    ClassID, mtMessage,	0, DsLog | DsMon);

  // Создаем пул потоков задач сервера
  BASE_SERVER_OBJECTS->SendThread.reset(new GivsSendThread());

  Result = BASE_SERVER_OBJECTS->SendThread->Initialize (BASE_SERVER_CONF->SendThreadNumber);

  CHECK_ERROR_RETURN(Result);

  // Привязываем пул потоков задач сервера к обработчику комманд с консоли
  SERVER_OBJECTS->kbd_handler.SetQueryTaskPool(BASE_SERVER_OBJECTS->SendThread.get());

  Inform(MS << "Task pool started successfully.",
    ClassID, mtMessage,	0, DsLog | DsMon);

  return Result;
}
//---------------------------------------------------------------------------
// Инициализация сервера
//---------------------------------------------------------------------------
int GivsServer_i::init (int argc, char** argv)
{
	int				Result;

	Result = BaseServer_i::init(argc, argv);

	CHECK_ERROR_RETURN(Result);

	// Инициализация базы данных отчетов
	Result = InitReportsDatabase (SERVER_NAME);

	CHECK_ERROR_RETURN(Result);

	// Инициализация базы данных
	Result = InitDatabase ();

	CHECK_ERROR_RETURN(Result);

  // Инициализация базы данных маршрутизации
  Result = InitRouteDatabase ();

  CHECK_ERROR_RETURN(Result);

  // Инициализация мониторинга
  Result = InitMonitoring ();

  CHECK_ERROR_RETURN(Result);

  // Инициализация интерфейса запросов к молотилке
  Result = InitMillInterface ();

  CHECK_ERROR_RETURN(Result);

	// Инициализация системы криптографии
	Result = InitCrypto ();

	CHECK_ERROR_RETURN(Result);

 	// Инициализация и запуск потоков Listener для клиента с проверкой подписи
 	Result = InitSignedListener ();
 
 	CHECK_ERROR_RETURN(Result);

  // Инициализация и запуск потоков пула задач сервера
  Result = InitSendThread ();

  CHECK_ERROR_RETURN(Result);

  // Инициализация планировщика задач
  Result = InitCron ();

  CHECK_ERROR_RETURN(Result);

  // Инициализация и запуск потоков Listener для коммандных запросов
  Result = InitServerCommandListener ();

  CHECK_ERROR_RETURN(Result);

  // Инициализация догонщика сервера
  
  Result = InitOvertaking ();

  CHECK_ERROR_RETURN(Result);

	return Result;
}

//---------------------------------------------------------------------------
// Запустить сервер
//---------------------------------------------------------------------------
int GivsServer_i::run(void)
{
	// Инициализация обьектов сервера
	SERVER_OBJECTS;

	return BaseServer_i::run();
}

//---------------------------------------------------------------------------
// Тестирование функций сервера
//---------------------------------------------------------------------------
int GivsServer_i::TestServer (void)
{
	TID               ClassID = tidSERVER;
	std::string       ProcedureName = "GivsServer_i::TestServer : ";
	int								Result = OK;

#ifdef  TEST_SERVER

  // Запускаем мониторинг
  if (BASE_SERVER_OBJECTS->Monitoring.get() != NULL) {
    Result = BASE_SERVER_OBJECTS->Monitoring->StartMonitoring();

    CHECK_ERROR_RETURN(Result);
  }

  // **************************************************

  // Парсим логи
  std::string       line;
  std::string       PhoneList = "";
  TWebRequestList 	Request;
  

//   ifstream myfile ("D:\\temp\\39\\Parsed\\Integrator\\IntegratorStutzer_8_0_140603_100642.log.00");
//   ifstream myfile ("D:\\temp\\39\\Parsed\\Integrator\\IntegratorStutzer_8_0_140603_101756.log.00");
  ifstream myfile ("D:\\temp\\39\\Parsed\\Integrator\\IntegratorStutzer_8_1_140603_111503.log.00"); 

  if (myfile.is_open())
  {
    while ( getline (myfile,line) )
    {
      getline (myfile,line);
      getline (myfile,line);
      
	    // Бьем телеграмму на отдельные поля
	    Request = ParseWebServerRequest(
		    line, 
		    ";",
		    "\n"
		    );

      if (Request[0] == "113") {
        if (PhoneList != "") {
          PhoneList += ",";
        }

        PhoneList += Request[3];
      }

    }

    myfile.close();
  }

  Inform(MS
    << "\n" + PhoneList
    ,ClassID);

  // **************************************************

  // Останавливаем мониторинг
  if (BASE_SERVER_OBJECTS->Monitoring.get() != NULL) {

    Result = BASE_SERVER_OBJECTS->Monitoring->StopMonitoring();

    CHECK_ERROR_RETURN(Result);
  }  

#endif // TEST_SERVER

	return Result;
}

//---------------------------------------------------------------------------
// Инициализация и запуск потоков Listener для коммандных запросов
//---------------------------------------------------------------------------
int GivsServer_i::InitServerCommandListener(void)
{
	int Result  = OK;
	TID	ClassID = tidCOMMON;
	std::string	ProcedureName = "GivsServer_i::InitServerCommandListener : ";

	IntegConsole_Listener_ACCEPTOR* Client_Listener_acceptor;
	BaseMsstream						iplisten(30);

	Inform(MS << "Start open Server Command Listener...", ClassID, mtMessage, 0, DsLog | DsMon);

	if (Integ_Command_Listener_server_factory_.open() == -1) 
	{
		Result = ERROR_OPEN_LISTENER_FACTORY;
		Inform(MS << "Error open Command_Listener_server_factory."
			, ClassID
			, mtError, Result, DsLog | DsMon);

		return Result;
	}

	// iplisten << SERVER_CONF->ServerCMDIPAddress << ":" << SERVER_CONF->ServerCMDPort;
	iplisten << BASE_SERVER_CONF->ServerCMDIPAddress << ":" << BASE_SERVER_CONF->ServerCMDPort;

	ACE_INET_Addr local_host(iplisten.str());

	Client_Listener_acceptor = &SERVER_OBJECTS->Integ_SrvCommand_Listener_acceptor;

	// Open the Acceptor factory.
	if (Client_Listener_acceptor->open(local_host,
		TAO_ORB_Core_instance()->reactor(),
		Integ_Command_Listener_server_factory_.creation_strategy(), 0,
		Integ_Command_Listener_server_factory_.concurrency_strategy(), 0) == -1) {

		Result = ERROR_OPEN_LISTENER_ACCEPTOR;

		Inform(MS << "Error open Server Command Listener_acceptor - server already run or invalid IP address."
			<< "\nIPAddress = " << BASE_SERVER_CONF->ServerCMDIPAddress
			<< "\nPort = " << BASE_SERVER_CONF->ServerCMDPort
			, ClassID, mtError, Result, DsLog | DsMon);

		ACE_OS::_exit(900082);

		return Result;
	}

	Inform(MS << "Server Command Listener started successfully.",	ClassID, mtMessage, 0, DsLog | DsMon);

	return Result;
}
