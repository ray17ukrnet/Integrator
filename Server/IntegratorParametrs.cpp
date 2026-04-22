// #ifdef FIND_MEMORY_LEAKS
// #include "vld.h"
// #endif // FIND_MEMORY_LEAKS

#include "integrator_pch.h"

#include "IntegratorParametrs.h"

//------------------------------------------------------------
// Конструктор
//------------------------------------------------------------
TGivsParametrs::TGivsParametrs(std::string aParamsFileName) :
	TBaseParametrs(aParamsFileName)
{

}

//------------------------------------------------------------
// Деструктор
//------------------------------------------------------------
TGivsParametrs::~TGivsParametrs()
{

}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Заполняет параметры конфигурации значениями по умолчанию
//////////////////////////////////////////////////////////////////////////////////////////////////////
void TGivsParametrs::PrepareDefaultConfParams(TConfigValues &Values)
{
	TBaseParametrs::PrepareDefaultConfParams(Values);

//	Values[ACCEPT_CERTIFICATE_PROGRAM_NAME] = "NULL";
  Values[STORE_I_TRANSACTIONS_COMM] = "1";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Check values of deamon's parameters
//////////////////////////////////////////////////////////////////////////////////////////////////////
int TGivsParametrs::ParseParams(TConfigValues &Values)
{
	int				Result = OK;

	string ParamName, ParamValue;

	Result = TBaseParametrs::ParseParams(Values);

	CHECK_ERROR_RETURN(Result);

// 	ParamName = FLASH_IP_ADDRESS;
// 	Result = ParseIPAddress(Values, ParamName);
// 
// 	CHECK_ERROR_RETURN(Result);
// 
// 	ParamName = FLASH_PORT;
// 	Result = ParseParam(Values, ParamName, 65535);
// 
// 	CHECK_ERROR_RETURN(Result);

	ParamName = SIGNED_IP_ADDRESS;
	Result = ParseIPAddress(Values, ParamName);

	CHECK_ERROR_RETURN(Result);

	ParamName = SIGNED_PORT;
	Result = ParseParam(Values, ParamName, 65535);

	CHECK_ERROR_RETURN(Result);

//   ParamName = OSMP_IP_ADDRESS;
//   Result = ParseIPAddress(Values, ParamName);
// 
//   CHECK_ERROR_RETURN(Result);
// 
//   ParamName = OSMP_PORT;
//   Result = ParseParam(Values, ParamName, 65535);
// 
//   CHECK_ERROR_RETURN(Result);
// 
//   ParamName = STORNO_PORT;
//   Result = ParseParam(Values, ParamName, 65535);
// 
//   CHECK_ERROR_RETURN(Result);

	return Result;
}

//////////////////////////////////////////////////////////////////////////
// Store setting that always read from local file
//////////////////////////////////////////////////////////////////////////
int TGivsParametrs::SetPreAllocConfParams(TConfigValues &Values)
{
	int				Result = OK;

	Result = TBaseParametrs::SetPreAllocConfParams(Values);

	CHECK_ERROR_RETURN(Result);

//   if (GetParametr(Values, INTEGRATOR_TAGS_BILL,
//     SERVER_CONF->IntegratorTagsBill)) {
//       return 1;
//   }
// 
//   if (GetParametr(Values, INTEGRATOR_TAGS_PAY,
//     SERVER_CONF->IntegratorTagsPay)) {
//       return 1;
//   }
/*
  if (GetParametr(Values, ROUTE_DATABASE_HOST,
    SERVER_CONF->RouteDatabaseHost)) {
      return 1;
  }

  if (GetParametr(Values, ROUTE_DATABASE_LOGIN,
    SERVER_CONF->RouteDatabaseLogin)) {
      return 1;
  }

  if (GetParametr(Values, ROUTE_DATABASE_NAME,
    SERVER_CONF->RouteDatabaseName)) {
      return 1;
  }

  if (GetParametr(Values, ROUTE_DATABASE_PASSWORD,
    SERVER_CONF->RouteDatabasePassword)) {
      return 1;
  }

  if (GetParametr(Values, ROUTE_DATABASE_PORT,
    SERVER_CONF->RouteDatabasePort)) {
      return 1;
  }
*/
  if (GetParametr(Values, SIGNED_IP_ADDRESS,
    SERVER_CONF->SignedIpAddress)) {
      return 1;
  }

  if (GetParametr(Values, SIGNED_PORT,
    SERVER_CONF->SignedPort)) {
      return 1;
  }

  if (GetParametr(Values, STORE_I_TRANSACTIONS_COMM,
    SERVER_CONF->StoreITransactionsComm)) {
      return 1;
  }

  if (GetParametr(Values, STORE_I_TRANSACTIONS_COMM,
    SERVER_CONF->StoreITransactionsComm)) {
      return 1;
  }


//   if (GetParametr(Values, STORNO_IP_ADDRESS,
//     SERVER_CONF->StornoIpAddress)) {
//       return 1;
//   }
// 
//   if (GetParametr(Values, STORNO_PORT,
//     SERVER_CONF->StornoPort)) {
//       return 1;
//   }

	return Result;
}
