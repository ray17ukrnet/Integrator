#ifndef __INTEGRATOR_SRV_ERROR_H__
#define __INTEGRATOR_SRV_ERROR_H__

#include "Common/BaseSrvError.h"

// Диапазон 859500..859599 - ошибки работы с запросами сервера Integrator
// #define RANGE_ERROR_INTEGRATOR																				  ( 59500 + RANGE_ERROR)

// #define ERROR_UNKNOWN_INTERNAL_SERVICE																	( 0 + RANGE_ERROR_INTEGRATOR)
#define ERROR_INVALID_RESPONSE_PARAMS_COUNT                             ( 1 + RANGE_ERROR_INTEGRATOR)
#define ERROR_UNEXPECTED_INTER_RESP_TYPE                                ( 2 + RANGE_ERROR_INTEGRATOR)
#define ERROR_UNKNOWN_TAG                                               ( 3 + RANGE_ERROR_INTEGRATOR)
// #define ERROR_UNKNOWN_INTEG_ID_SERVICE                                  ( 4 + RANGE_ERROR_INTEGRATOR)
#define ERROR_EXEC_STORNO_PROCEDURE                                     ( 5 + RANGE_ERROR_INTEGRATOR)
#define ERROR_EXEC_STORNO_PROVIDER                                      ( 6 + RANGE_ERROR_INTEGRATOR)
#define ERROR_INVALID_TRANSACTION_STATUS_FOR_STORNO                     ( 7 + RANGE_ERROR_INTEGRATOR)
#define ERROR_TRANSACTION_ABSENT                                        ( 8 + RANGE_ERROR_INTEGRATOR)
#define ERROR_OPER_STATUS_REQUEST_FORBIDDEN_BY_PROVIDER                 ( 9 + RANGE_ERROR_INTEGRATOR)
#define ERROR_STORNO_IN_HAND_PROCESS                                    ( 10 + RANGE_ERROR_INTEGRATOR)
#define ERROR_INVALID_TRANSACTION_STATUS                                ( 11 + RANGE_ERROR_INTEGRATOR)
#define ERROR_NOT_EXISTS_TAG_DATA_ID_COMPANY                            ( 12 + RANGE_ERROR_INTEGRATOR)
#define ERROR_OUT_OF_BUYER_LIMIT                                        ( 13 + RANGE_ERROR_INTEGRATOR)
#define ERROR_TRANSACTION_CANCELED                                      ( 14 + RANGE_ERROR_INTEGRATOR)
#define ERROR_WRONG_AW_AL_RAG_DATA                                      ( 15 + RANGE_ERROR_INTEGRATOR)
#define ERROR_DEFINE_SEND_AUTH_TYPE                                     ( 16 + RANGE_ERROR_INTEGRATOR)
#define ERROR_CALLPHONE_WRONG_TEL_ID                                    ( 17 + RANGE_ERROR_INTEGRATOR)
#define ERROR_CALLPHONE_EXCHANGE_FAIL                                   ( 18 + RANGE_ERROR_INTEGRATOR)

#endif // __INTEGRATOR_SRV_ERROR_H__
