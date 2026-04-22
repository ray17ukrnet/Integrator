#ifdef FIND_MEMORY_LEAKS
#include "vld.h"
#endif // FIND_MEMORY_LEAKS

// GivsServer.cpp : Defines the entry point for the console application.
//
#include "integrator_pch.h"

int main(int argc, char* argv[])
{
	TID               ClassID = tidSERVER;
	std::string       ProcedureName = "main : ";

	int								Result = OK;

	GivsServer_i Server(SERVER_NAME);

	try
	{
		if ( (Result = Server.init(argc, argv) ) != 0 )
		{
			InformStr(MS << "Initialization: NOT OK!!!\n");

			Sleep(10000);
			ACE_OS::_exit(-1);

			return Result;
		}

    /// Release 2018-02-20 15:17, version 8.326 Integrator Production Add AL, DR, TR
    /// Release 2018-02-21 14:52, version 8.326 Integrator Production. Tag DR edition.
    /// Release 2018-06-15 14:58, version 8.327 Integrator Production. Ukrtelecom IdService=99,IdSubService=UTCIdSubService
    /// Release 2018-06-18 16:30, version 8.327 Integrator Production. Ukrtelecom IdService=99,IdSubService=UTCIdSubService, No Stub
    /// Release 2018-07-30 15:41, version 8.327 Integrator Production. Ukrtelecom IdService=99,IdSubService=UTCIdSubService, No Stub
    /// Release 2018-07-31 15:22, version 8.327 Integrator Production. Command port, Status request
    /// Release 2018-08-02 15:31, version 8.327 Integrator Production. 
    /// Release 2018-11-30 10:55, version 8.327 Integrator Production. 
    /// Release 2019-03-04 19:11, version 8.327 Integrator Production. AP+AL
    /// Release 2019-03-05 13:01, version 8.327 Integrator Production. AP+AL в методі MillExtAcceptor::AcceptPayBillMill
    /// Release 2019-03-05 17:20, version 8.354 Integrator Production. 
    /// Release 2019-03-27 13:05, version 8.356 Integrator Production. Tag DO
    /// Release 2019-03-27 18:41, version 8.356 Integrator Production. Tag DO
    /// Release 2019-04-01 17:28, version 8.356 Integrator Production. Tag DO-> 115,117
    /// Release 2019-04-05 11:51, version 8.356 Integrator Production. Tag DO-> 115,117
    /// Release 2019-04-22 14:45, version 8.356 Integrator Production. Tag DO-> 115,117
    /// Release 2019-05-29 12:17, version 8.356 Integrator Production. Tag AN(112), PN(113)
    /// Release 2019-06-04 17:00, version 8.356 Integrator Production. Staus 81 on 111(112) request-response
    /// Release 2019-06-19 16:07, version 8.356 Integrator Production. Include Cryptolib
    /// Release 2019-06-21 14:00, version 8.356 Integrator Production. Migration to a New TRADE
    /// Release 2019-06-21 18:25, version 8.356 Integrator Production. Migration to a New TRADE
    /// Release 2019-07-15 15:30, version 8.356 Integrator Production. 
    /// Release 2019-07-31 12:07, version 8.356 Integrator Production. IBAN Reqs: 111,113
    /// Release 2019-08-19 10:24, version 8.356 Integrator Production. IBAN Reqs: 111,113 refresh :)
    /// Release 2019-08-29 12:54, version 8.356 Integrator Production. IBAN Reqs: 111,113 update :)
	  /// Release 2019-09-09 16:33, version 8.356 Integrator Production. IBAN to Account Reqs: 111,113 update :)
    /// Release 2019-10-29 14:30, version 8.356 Integrator Production. Tag PN(113)
    /// Release 2019-12-09 14:50, version 8.356 Integrator Production. 115 (..\IntegratorMillExtAcceptor.cpp::AcceptStornoReq()::line: 3071)
    /// Release 2020-04-27 21:22, version 8.356 Integrator Production. add Tag AW to 113 PayHandler -> https://mail.google.com/mail/u/0/#inbox/KtbxLvhRXgfdPhpbdzhwNMmTCVRDktMGDq?compose=jrjtXLFHXGdXqkRQSGZZMRmBvHNznJtFRBPZKfbSKdmFSmFGKQSGGLHHFpBVWJMPrvZFmKld
    /// Release 2020-04-29 16:22, version 8.356 Integrator Production. add Tag AW+AL to 113 - скидую у лог дані цих тагів + контроль суми > 5K
    /// Release 2020-04-30 14:16, version 8.356 Integrator Production. add Tag AW+AL to 113 - скидую у лог дані цих тагів + контроль суми > 5K
    /// Release 2020-05-04 13:20, version 8.356 Integrator Production. add Tag AW+AL to 113 - контроль даних IПН у полі AW.other
    /// Release 2020-05-04 22:42, version 8.356 Integrator Production. add Tag AW+AL to 113 - контроль даних IПН у полі AW.other
    /// Release 2020-05-08 14:30, version 8.356 Integrator Production. add контроль лімітів за допомогою тагів AL,AW
    /// Release 2020-06-15 16:25, version 8.356 Integrator Production. add контроль лімітів за допомогою тагів AL,AW
    /// Release 2020-10-01 17:22, version 8.356 Integrator Test. https://mail.google.com/mail/u/0/#inbox/FMfcgxwJZJSxmvhkgXljbMscrTDCmrhW
    /// Release 2020-10-02 09:53, version 8.356 Integrator Test. https://mail.google.com/mail/u/0/#inbox/FMfcgxwJZJSxmvhkgXljbMscrTDCmrhW
    /// Release 2020-10-19 13:27, version 8.356 Integrator Production. https://mail.google.com/mail/u/0/#inbox/FMfcgxwJZJSxmvhkgXljbMscrTDCmrhW
    /// Release 2020-12-07 15:56, version 8.356 Integrator Production. До запиту 111 додано виклиск СП IntegProcess111Stage0_v1()
    /// Release 2020-12-11 15:16, version 8.356 Integrator Production. До запиту 111 додано виклиск СП IntegProcess111Stage0_v1()
    /// Release 2020-12-30 14:46, version 8.356 Integrator Production. До запиту 111 додано виклиск СП IntegProcess111Stage0_v1() Production
    /// Release 2021-01-04 14:20, version 8.356 Integrator Production. На запитi 113-ть додано обробку ситуацiї, коли не змогли коректно оновити значення лiчильника з тагу CN
    /// Release 2021-12-21 17:25, version 8.356 Integrator Production. Додано у ядро збереження у окремий файл відповіді, якщо її розмір більше MAX_ANSWER_MSG_SIZE
    /// Release 2021-12-22 11:20, version 8.356 Integrator Production. Базова перезбірка ядра та проекта
    /// Release 2021-12-24 12:52, version 8.356 Integrator Production. Базова перезбірка ядра та проекта
    /// Release 2022-01-04 11:30, Перезбірка з-за змін у BaseServer -> зробив збереження логів до структури каталогів MM/DD
    /// Release 2022-01-12 16:22, version 8.356 Integrator Production. контроль суми операції на запиті 113 на нуль
    /// Release 2022-12-13 16:00, version 8.356 Integrator Production. Додано контроль введених значень для лічильника перед вставкою до БД, task http://192.168.71.10/issues/34651
    /// Release 2022-12-22 10:35, version 8.356 Integrator Production. Додано обробник для тага ERR, задача http://192.168.71.10/issues/32831
    /// Release 2023-07-27 17:00, version 8.356 Integrator Production. Додано обробник для SMS, задача  http://192.168.71.10/issues/48226
    /// Release 2023-07-28 13:42, version 8.356 Integrator Production. Додано обробник для SMS, задача  http://192.168.71.10/issues/48226
    /// Release 2023-07-31 17:27, version 8.356 Integrator Production. Додано обробник для SMS, задача  http://192.168.71.10/issues/48226
    /// Release 2023-08-04 11:41, version 8.356 Integrator Production. Додано обробник для PhoneCall -> stend (test version)
    /// Release 2023-08-04 14:22, version 8.356 Integrator Production. Додано обробник для PhoneCall
    /// Release 2023-11-15 15:45, version 8.356 Integrator Production. Додано обробники тагів AWP та AWF, task http://192.168.71.10/issues/50922
    /// Release 2023-11-20 17:28, version 8.356 Integrator Production. Додано обробники тагів AWP та AWF, task http://192.168.71.10/issues/50922
    /// Release 2025-07-28 17:45, version 8.356 Integrator Production. M1 передання номеру точки, task https://redmine.bf.com.ua/issues/106705
    /// Release 2025-08-08 14:45, version 8.356 Integrator Production. Зміна у обробці запиту 117/217 - обробка 30/36, task https://redmine.bf.com.ua/issues/101641
    /// Release 2026-01-14 17:06, version 8.356 Integrator Production. Зміна у обробці запиту 117/217 - обробка 30/36task https://redmine.bf.com.ua/issues/117030

#ifdef TEST_SERVER 
		Result = Server.TestServer ();

		if (Result != OK) {
			Sleep(10000);
		}

    Server.destroy();

		return Result;
#endif // TEST_SERVER 
		
		Result = Server.run ();

		Server.destroy();

	}
	catch (...)
	{
		Result = ERROR_EXCEPTION;
	}

	if (Result != OK) {
		Sleep(10000);
	}

	return Result;
}

