#include "integrator_pch.h"

#include "IntegratorCommon.h"

/// <summary>
/// Функція, яка контролює введене значення показника лічильника на максимальне можливе DECIMAL(12,4), тобто 10^(12-4)-1= 99999999
/// </summary>
/// <param name="inCounterValue">Значення показника лічильника</param>
/// <returns>Повертає значення лічильника inCounterValue або 0, якщо данні некоректні або більші за 10^(12-4)-1= 99999999.</returns>
std::string ControlCounterValue12dot4(std::string inCounterValue)
{
	std::string Result = "0";
	const std::string strMaxValue = "99999999";

	size_t Pos = inCounterValue.find(".");
	// Перевіряємо ціле число чи ні
	if (Pos != std::string::npos) {
		Result = inCounterValue.substr(0, Pos);
	}	else {
		Result = inCounterValue;
	}
	// Перевіряємо всі введені символи є цифрами
	if (!isCheckDigitalSequence(Result))
	{
		return "0";
	}
	// Перевіряємо на максимальне значення
	if (StringToInt(Result) > StringToInt(strMaxValue))
	{
		Result = "0";
	}

	return Result;
}
