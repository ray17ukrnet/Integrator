#ifndef __INTEGRATOR_COMMON_H__
#		define __INTEGRATOR_COMMON_H__

#include "Common/IntegratorSrvTypes.h"

/// <summary>
/// Функція, яка контролює введене значення показника лічильника на максимальне можливе DECIMAL(12,4), тобто 10^(12-4)-1= 99999999
/// </summary>
/// <param name="inCounterValue">Значення показника лічильника</param>
/// <returns>Повертає значення лічильника inCounterValue або 0, якщо данні некоректні або більші за 10^(12-4)-1= 99999999.</returns>
std::string ControlCounterValue12dot4(std::string inCounterValue);

#endif // __INTEGRATOR_COMMON_H__
