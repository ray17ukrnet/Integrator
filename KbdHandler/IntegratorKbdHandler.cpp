#include "integrator_pch.h"

#include "Http/SSLTools.h"

// Выполнение комманд ГИВЦ
//#include "Requests/IntegratorCommand.h"

#include "IntegratorKbdHandler.h"

//#define CERT_LIST												"cert_list"
//#define PAY_SHEET												"pay_sheet"
//#define UPDATE_PAYMENT									"update_payment"
#define RUN_TEST								        "run_test"

//--------------------------------------------------------------------
// Constructor.
//--------------------------------------------------------------------
GivsKbdHandler::GivsKbdHandler (void) : BaseKbdHandler()
{

}
//--------------------------------------------------------------------
// Destructor.
//--------------------------------------------------------------------
GivsKbdHandler::~GivsKbdHandler (void)
{
}

//--------------------------------------------------------------------
// Обрабатывает комманды полученные с консоли
// Вход 
//	Command - комманда для выполнения
// Выход 
//	результат функции - обработана комманда или нет
//--------------------------------------------------------------------
bool GivsKbdHandler::ProcessCommand(TStdStringList Command)
{
	TID               ClassID = tidKBD_HANDLER;
	std::string       ProcedureName = "GivsKBDHandler::ProcessCommand : ";

	std::string       MessageText;
  autoTaskMessage		TaskMessage;

	bool							Result = true;

	Result = BaseKbdHandler::ProcessCommand(Command);

	// Если комманда обработана то выходим
	if (Result) {
		return Result;
	}

	Result = true;
  /*
	// Показать список ssl серьтификатов в хранилище
 	if (Command[0] == CERT_LIST) {

		CertList(Command);

	// Обновить названия платежей ГИВЦ в базе данных
	} else if (Command[0] == UPDATE_PAYMENT) {

		UpdatePayment(Command);

		// Создать документ с реестром платежей
	} else if (Command[0] == PAY_SHEET) {

		PaySheet(Command);

  } else*/ if (Command[0] == RUN_TEST) {

      TaskMessage = CreateAutoTaskMessage (stcRunTest, Command[1]);

      TaskMessage->test_givs.ThreadCount = StringToInt(Command[1]);

      QueryTaskMessage (TaskMessage);

	// Комманда не найдена в списке комманд
 	} else {
 		Result = false;
	}

	return Result;
}

//--------------------------------------------------------------------
// Возвращает текстовую строку для хелпа
//--------------------------------------------------------------------
std::string GivsKbdHandler::GetHelpText()
{
	std::string				Result;

	Result = BaseKbdHandler::GetHelpText();

// 	Result += "* ";
// 	Result += CERT_LIST;
// 	Result += " - show ssl certificate list\n";
// 
// 	Result += "* ";
// 	Result += PAY_SHEET;
// 	Result += " IdPaySheet ";
// 	Result += " - create pay sheet pdf documents on GIVS server side\n";
// 
// 	Result += "* ";
// 	Result += UPDATE_PAYMENT;
// 	Result += " - update givs payment name in database\n";

  Result += "* ";
  Result += RUN_TEST;
  Result += " count - run tests\n";

  
	return Result;
}

//---------------------------------------------------------------------------
// Показывает статус сервера и текущую версию сервера 
//---------------------------------------------------------------------------
void GivsKbdHandler::ShowServerStatus(void)
{
  TID               ClassID = tidSERVER;
  std::string       ProcedureName = "GivsKbdHandler::ShowServerStatus : ";

  BaseKbdHandler::ShowServerStatus();

  // Эмулировать запросы к внутренним серверам
#ifdef EMULATE_INTEG_INTERNAL_REQUEST
  Inform(MS
    << "Enable emulation internal request"
    ,ClassID, mtMessage, 0, DsLog | DsMon| DsHist);
#endif // EMULATE_INTEGRATOR_REQUEST

}

