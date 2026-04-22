#ifndef __INTEGRATOR_SRV_CONST_H__
#define __INTEGRATOR_SRV_CONST_H__

#include "Common/BaseSrvConst.h"

// Название сервера
#define SERVER_NAME						"Integrator"

// База данных
#define DATABASE							SERVER_OBJECTS->Database
#define ROUTE_DATABASE				BASE_SERVER_OBJECTS->RouteDatabase
#define DEP_DATABASE					SERVER_OBJECTS->DepDatabase

// Список тагов, которые может добавлять интегрированный сервер
// #define ALL_INTEGRATOR_TAGS_BILL   "CH1|CH2|CH3"
// #define ALL_INTEGRATOR_TAGS_PAY    "CH1|CH2|CH3"

// Завершающий символ для подписанных сообщений
#define ETX_SIGNED														"\n"

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Универсальный интерфейс
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

// Значение для пустых полей 
#define UNI_KIOSK_NULL_RECORD_VALUE					  ""

//////////////////////////////////////////////////////////////////////////
// Описание названий параметров конфигурации
//////////////////////////////////////////////////////////////////////////

#define   SIGNED_IP_ADDRESS												"SIGNED_IP_ADDRESS"
#define   SIGNED_PORT															"SIGNED_PORT"
#define   STORE_I_TRANSACTIONS_COMM               "STORE_I_TRANSACTIONS_COMM"

#define REQ_INTEG_GET_BILL_RESP_COUNT             8
#define REQ_INTEG_PAY_BILL_RESP_COUNT             6
#define REQ_INTEG_STORNO_BILL_RESP_COUNT          3
// #define REQ_INTEG_ERROR_RESP_COUNT                2

#endif // __INTEGRATOR_SRV_CONST_H__
