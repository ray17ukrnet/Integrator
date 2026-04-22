#ifndef UKR_TELE_SEND_THREAD_H
#  define UKR_TELE_SEND_THREAD_H

#if !defined (ACE_LACKS_PRAGMA_ONCE)
#pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "SendThread/BaseSendThread.h"

//-----------------------------------------------------------------------------
class GivsSendThread : public BaseSendThread
{
public:

  GivsSendThread();
  // Constructor.
	
	//virtual int Initialize (int SendThreadNumber);

protected:
	virtual int Processing(autoTaskMessage &TaskMessage);

	///////////////////////////////////////////////////////////////////
	//отсылка
	///////////////////////////////////////////////////////////////////
  
	//---------------------------------------------------------------------------
	// Выполнить тест
	//---------------------------------------------------------------------------
	virtual int Send_RUN_TESTS(autoTaskMessage &TaskMessage); 

	//---------------------------------------------------------------------------
	// Сделать тестовый запрос счета и его оплату 
	//---------------------------------------------------------------------------
	virtual int Send_TEST_EXEC_CLIENT_REQUEST (autoTaskMessage &TaskMessage);

	///////////////////////////////////////////////////////////////////

};

#endif  // UKR_TELE_SEND_THREAD_H
