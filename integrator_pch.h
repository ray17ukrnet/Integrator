#ifndef __PCH_H__
#	define __PCH_H__

// Макрос для описания класса контейнера в котором хранится конфигурация сервера
#define SERVER_CONF				GIVS_PRE_ALLOC_CONF::instance()

// Макрос для описания класса контейнера в котором хранится классы обьекты сервера
#define SERVER_OBJECTS		GIVS_PRE_ALLOC::instance()

#include "base_pch.h"

#include "IntegratorConfiguration.h"

#include "Common/IntegratorSrvConst.h"
#include "Common/IntegratorSrvTypes.h"
#include "Common/IntegratorSrvError.h"
#include "Common/IntegratorConverters.h"
#include "Common/IntegratorMsstream.h"
#include "Common/IntegratorCommon.h"

#include "Database/IntegratorDatabase.h"

//#include "Crypto/HsmCrypt.h"

//#include "XML/XMLTools.h"

#include "Common/IntegratorPreAllocated.h"

#include "KbdHandler/IntegratorKbdHandler.h"

#include "Server/IntegratorParametrs.h"
#include "Server/IntegratorServer_i.h"

#include "SaListener/IntegratorSignedListener.h"
#include "SaListener/IntegratorConsoleListener.h"

#include "SendThread/IntegratorSendThread.h"

//#include "Requests/BaseWebserverRequests.h"
//#include "Logger/IntegratorLogClient.h"

#endif // __PCH_H__
