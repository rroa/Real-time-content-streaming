#pragma once

#ifndef _CONSOLE_COMMAND_H_ 
#define	_CONSOLE_COMMAND_H_

#include <string>

void ConsoleCommand_Help( const std::string& argsString );
void ConsoleCommand_Say( const std::string& argsString );
void ConsoleCommand_Clear( const std::string& argsString );
void ConsoleCommand_SetColor( const std::string& argsString );
void ConsoleCommand_Exit( const std::string& argsString );
//void ConsoleCommand_AddLights( const std::string& argsString );
void ConsoleCommand_ExportLog( const std::string& argsString );
void ConsoleCommand_EnableFBO( const std::string& argsString );
void ConsoleCommand_DisableFBO( const std::string& argsString );
void ConsoleCommand_RotateModel( const std::string& argsString );
void ConsoleCommand_RotateModelRatio( const std::string& argsString );

#endif
