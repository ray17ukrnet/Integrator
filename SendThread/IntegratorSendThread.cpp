#include "integrator_pch.h"

// Обработка запросов клиентов и запросов к серверу
//#include "Requests/UniBillClientRequests.h"

#include "IntegratorSendThread.h"

static ACE_Atomic_Op < ACE_Mutex, CORBA::ULong >TreadNumber;

//=====================================================================

GivsSendThread::GivsSendThread()
  : BaseSendThread ()
{
  ACE_Atomic_Op < ACE_Mutex, CORBA::ULong >TreadNumber (0);
}

int GivsSendThread::Processing(autoTaskMessage &TaskMessage)
{
  TID                 ClassID = tidSEND_THREAD;
  std::string         ProcedureName = "GivsSendThread::Processing : ";
//  std::string         MessageText;

  int                 Res;

	switch(TaskMessage->MSG_ID) {
		case stcRunTest:
			Res = Send_RUN_TESTS(TaskMessage);
			break;
		case stcRunTestGetAndPayBill:
			Res = Send_TEST_EXEC_CLIENT_REQUEST(TaskMessage);
			break;
    default: {
      Res = BaseSendThread::Processing (TaskMessage);
             }
	}

	return Res;
}

//---------------------------------------------------------------------------
// Выполнить тест
//---------------------------------------------------------------------------
int GivsSendThread::Send_RUN_TESTS(autoTaskMessage &TaskMessage)
{
	TID                 ClassID = tidSEND_THREAD;
	std::string         ProcedureName = "GivsSendThread::Send_RUN_TESTS : ";
	int									Result = OK;

// 	int									ThreadCount;
 	autoTaskMessage			NewTaskMessage;
  std::string         SQL;
// 	std::string					FileName = "Client.csv";
// 	S_DATA_BLOCK				DataBlock;
// 	TStdStringList			StringList;
// 	std::string					IdService;
// 	std::string					IdClient;

//   ThreadCount = TaskMessage->test_givs.ThreadCount;
// 
// 	for (int N = 1; N <= ThreadCount; ++N) {
// 		NewTaskMessage = CreateAutoTaskMessage (
// 			stcRunTestGetAndPayBill);
// 
//     SQL = "select KeyStatus from SignKeys where  KeyValue = 999900000010";
// 
// 		StringToArray(
// 			SQL,
// 			NewTaskMessage->DataArray.Data,
// 			sizeof(NewTaskMessage->DataArray.Data)
// 			);
// 
// 		Result = SERVER_OBJECTS->kbd_handler.QueryTaskMessage (NewTaskMessage);
// 
// 		CHECK_ERROR_RETURN(Result);
// 	}
	
	return Result;
}

//---------------------------------------------------------------------------
// Сделать тестовый запрос счета и его оплату 
//---------------------------------------------------------------------------
int GivsSendThread::Send_TEST_EXEC_CLIENT_REQUEST (autoTaskMessage &TaskMessage)
{
	TID                 ClassID = tidSEND_THREAD;
	std::string         ProcedureName = "GivsSendThread::Send_TEST_EXEC_CLIENT_REQUEST : ";
	int									Result = OK;

//  int                 ThreadN;

//   ThreadN = TreadNumber++;
// 
//   //************************************************
//   // Выполнить тестовый запрос в базе данных
//   //************************************************
// 
//   std::string					SQL;
// 
//   TSQLResults					SQLResults;
// 
//   //mysqlpp::Connection	con(false);
// 
//   SQL = ArrayToString(
//     TaskMessage->DataArray.Data, 
//     sizeof(TaskMessage->DataArray.Data)
//     );
// 
//   Result = DATABASE->ExecSQLRequest (	
//       SQL,
// 
//       SQLResults
//       );
// 
// //   Result = DATABASE->ConnectDB(	
// //     con
// //     );
// 
// //  if (Result == OK) {
// //     Inform(MS
// //       << "Thread " << TreadNumber.value()
// //       << "ok"
// //       ,ClassID, mtMessage, 0, DsMon);
// //   } else {
// //     Inform(MS
// //       << "In thread " << ThreadN
// //       << " has error"
// //       ,ClassID, mtMessage, 0, DsMon | DsLog);
// //   }
// 
// //  CHECK_ERROR_RETURN(Result);
// 
// //   Result = DATABASE->DisconnectDB(	
// //     con
// //     );
// 
//   if (Result == OK) {
//     Inform(MS
//       << "Thread " << ThreadN
//       << " ok"
//       ,ClassID, mtMessage, 0, DsMon);
//   } else {
//     Inform(MS
//       << "In thread " << ThreadN
//       << " has error"
//       ,ClassID, mtMessage, 0, DsMon | DsLog);
//   }
// 
//   CHECK_ERROR_RETURN(Result);

	return Result;
}

