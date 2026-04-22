#ifndef __INTEGRATOR_PREALLOCATED_H__
#	define __INTEGRATOR_PREALLOCATED_H__

#include "integrator_pch.h"

#include "KbdHandler/IntegratorKbdHandler.h"
#include "SaListener/IntegratorSignedListener.h"
#include "SaListener/IntegratorStornoListener.h"
#include "SaListener/IntegratorConsoleListener.h"

// База данных счетов
#include "Database/IntegratorDatabase.h"

// Криптование и раскриптование
#include "Crypto/HsmCrypt.h"
// RSA Криптование и раскриптование
#include "Crypto/RSACrypt.h"
// CryptoLib Криптование и раскриптование RSA
#include "Crypto/CryptoLibCrypt.h"
// CryptoLib Криптование и раскриптование DSTU
#include "Crypto/CryptoLibCryptDSTU.h"

// Конфигурация сервера
struct Givs_Pre_Allocated_conf : Base_Pre_Allocated_conf
{
	Givs_Pre_Allocated_conf (void)
	{
		// ***************************************
		// Конфигурация GivsSaListener
		// ***************************************

		// Максимальное количество одновременно открытых потоков приема сообщений
		MaxAcceptConnect = 10;

	};


	// Деструктор
	~Givs_Pre_Allocated_conf (void) {};
	// Destructor

  // ***************************************
  // Интерфейс к базе данных маршрутизации
  // ***************************************

  // Адресс базы данных
//   std::string RouteDatabaseHost; 
// 
//   // Логин в базу данных
//   std::string RouteDatabaseLogin;
// 
//   // Название базы данных к которой коннектиться
//   std::string RouteDatabaseName;
// 
//   // Пароль в базу данных
//   std::string RouteDatabasePassword;
// 
//   // Порт к базе данных
//   int					RouteDatabasePort;

	// ***************************************
	// Интерфейс клиентских приложений
	// ***************************************

  // Адресс слушалки подписанных клиентских приложений 
  std::string SignedIpAddress;

  // Порт на котором слушать подписанныt клиентские приложения
  int SignedPort;

  // Адресс слушалки Storno
//   std::string StornoIpAddress;

  // Порт на котором слушать Storno
//   int StornoPort;

 	// ***************************************
	// Списки тагов
	// ***************************************

  // Перечень обрабатываемых сервером тагов счет
  std::string   IntegratorTagsBill;

  // Перечень обрабатываемых сервером тагов оплата
  std::string   IntegratorTagsPay;

  // Сохранять список присланных тагов в таблицу iTransactionsComm
  std::string   StoreITransactionsComm;
};

typedef ACE_Singleton<Givs_Pre_Allocated_conf, ACE_SYNCH_MUTEX> GIVS_PRE_ALLOC_CONF;

// Общие классы сервера
struct Givs_Pre_Allocated : Base_Pre_Allocated
{
	// Работа с базой данных
	autoGivsDatabase				Database;

	// Работа с базой данных маршрутизации
// 	autoRouteDatabase				RouteDatabase;

	// Обработчик запросов с клавиатуры
	GivsKbdHandler					kbd_handler;

	// Слушалка входящих подписанных запросов клиентов
	Givs_Signed_Listener_ACCEPTOR	Client_Signed_Listener_acceptor;

  // Слушалка комманд сервера
  IntegConsole_Listener_ACCEPTOR Integ_SrvCommand_Listener_acceptor;

  // Слушалка входящих подписанных запросов сторно
//   Givs_Storno_Listener_ACCEPTOR Client_Storno_Listener_acceptor;

  // Шифрование и расшифрование запросов сервера ключем автора
  autoHsmCrypto									ClientCryptoAvtor;

  // Шифрование и расшифрование запросов сервера ключем RSA
  autoRSACrypto									ClientCryptoRSA;

  // Шифрование и расшифрование запросов сервера ключем CryptoLib автора ключами RSA
  autoCryptoLibCrypt            ClientCryptoLibCrypt;

  // Шифрование и расшифрование запросов сервера ключем CryptoLib автора ключами DSTU
  autoCryptoLibCryptDSTU        ClientCryptoLibCryptDSTU;

};

typedef ACE_Singleton<Givs_Pre_Allocated, ACE_SYNCH_MUTEX> GIVS_PRE_ALLOC;

#endif // __GIVS_PREALLOCATED_H__
