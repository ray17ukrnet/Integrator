#include "integrator_pch.h"
#include "IntegratorServerCommand.h"

//------------------------------------------------------------------
// Конструктор
//------------------------------------------------------------------
IntegratorServerCommand::IntegratorServerCommand() 
{
	UniKioskAcceptor.reset(new MillExtAcceptor());
}

//------------------------------------------------------------------
// Деструктор
//------------------------------------------------------------------
IntegratorServerCommand::~IntegratorServerCommand() 
{
	UniKioskAcceptor.reset(new MillExtAcceptor());
}

//------------------------------------------------------------------
// Выполнить комманду
//------------------------------------------------------------------
int IntegratorServerCommand::ProcessServerCommand(
	const std::string				&command,

	std::string						&message
)
{
	int         Result        = OK;
	std::string ProcedureName = "IntegratorServerCommand::ProcessCommand : ";

	TStdStringList commands; commands.clear();
	int Port = 0;
	commands = SplitTextString(command, BASE_PACKET_DIVIDER);

	Inform(MS << trancate_string_return(command), tidCOMMON, mtMessage, Result);

	StringToIntSafe(commands[0], Port);
	
	if ((isCheckDigitalSequence(commands[0])) && (Port <= 65536) && (commands[0].length() <= 5) && (commands.size() >= 2))
	{
		Result = UniKioskAcceptor->AcceptCmdPortRequest(command, message);
	  message = trancate_string_return(message) + BASE_ETX;
	}
	else
	{
		Result = TBaseServerCommand::ProcessServerCommand(command, message);

		// Because TBaseServerCommand::ProcessServerCommand_status return not 0
		// Must be repaired
		if (command.find(CMD_STATUS) != std::string::npos) { return OK; }

		return Result;
	}

/*
  if (command.find(CMD_GET_BALANCE) != std::string::npos)
  {
    // Result = SERVER_OBJECTS->kbd_handler.ProcessServerCommand_GetBalance(command, message);
  }
  else if (command.find(CMD_GET_REGISTER) != std::string::npos)
  {
    // Result = SERVER_OBJECTS->kbd_handler.ProcessServerCommand_GetRegister(command, message);
  }
  else
  {
		Result = TBaseServerCommand::ProcessServerCommand(command, message);

		// Because TBaseServerCommand::ProcessServerCommand_status return not 0
		// Must be repaired
		if (command.find(CMD_STATUS) != std::string::npos) return OK;

		return Result;
  }
*/
	Inform(MS << trancate_string_return(message), tidCOMMON, mtMessage, Result);

	return Result;
}

