#ifndef __INTEGRATOR_CONFIGURATION_H__
#  define __INTEGRATOR_CONFIGURATION_H__

//**************************************************************************
// В этом файле описаниы параметры для различных конфигураций сервера
// Для релиза все опции отключаются
//**************************************************************************

// Эмулировать запросы к внутренним серверам
#ifndef EMULATE_INTEG_INTERNAL_REQUEST
// 	#define EMULATE_INTEG_INTERNAL_REQUEST
#endif


// Скомпилировать проект в режиме тестирования
#ifndef TEST_SERVER
 	/// #define TEST_SERVER
#endif

#ifndef TEST_SERVER_AG
  // #define TEST_SERVER_AG
#endif /// TEST_SERVER_AG

#endif // __INTEGRATOR_CONFIGURATION_H__
