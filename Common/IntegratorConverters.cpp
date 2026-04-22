#include "integrator_pch.h"

// Преобразование типов
#include "Common/Converters.h"

#include "IntegratorConverters.h"

/*
//=============================================================================
// Преобразует MySql дату в дату в формате ГИВЦ 
//=============================================================================
std::string MySQLDateTime_To_GivsDateTime (std::string Value)
{
	std::string					Result;

	if (Value.length() < 10) {
		Result = "!!!INVALID!!!";
	}

	// 2008-08-08 08:08
	Result = Value.substr(0, 4);
	Result += Value.substr(5, 2);
	Result += Value.substr(8, 2);

	return Result;
}
*/
