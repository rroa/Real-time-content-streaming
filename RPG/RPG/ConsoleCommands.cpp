#include "stdafx.h"
#include "ConsoleCommands.h"

#include "ReportErrors.h"
#include "Console.h"
#include "Vector4.h"
#include "StringExtensions.h"
#include "RenderParameters.h"

typedef Vector4 Color;

void ConsoleCommand_Help( const std::string& argsString )
{
	if( argsString.length() > 0 ) 
	{
		Solidus::ConsoleSystem::Instance().PrintErrorMessage( "HELP does not take arguments." );
	}

	// TODO: This should be an iteration through the registered commands.
	// All commands should have a description of what they do.
	Solidus::ConsoleSystem::Instance().AddToLog( "HELP - Displays commands list" );
	Solidus::ConsoleSystem::Instance().AddToLog( "CLEAR	- Clears the display" );
	Solidus::ConsoleSystem::Instance().AddToLog( "SAY - Prints message to the console. E.g. say hello" );
	Solidus::ConsoleSystem::Instance().AddToLog( "SETCOLOR - Sets a new color default color for the console commands. E.g. SETCOLOR blue" );
	//Solidus::ConsoleSystem::Instance().AddToLog( "ADDLIGHTS	- Adds the amount of lights specified" );
	Solidus::ConsoleSystem::Instance().AddToLog( "ENABLEFBO	- Enables the FBO Pipeline" );
	Solidus::ConsoleSystem::Instance().AddToLog( "DISABLEFBO - Disables the FBO Pipeline" );
	Solidus::ConsoleSystem::Instance().AddToLog( "ROTATEMODEL - Rotates the renderer model" );
	Solidus::ConsoleSystem::Instance().AddToLog( "ROTATEMODELRATIO - Changes the speed of rotation of the model" );
	Solidus::ConsoleSystem::Instance().AddToLog( "EXPORTLOG - Saves the console log to a file" );
	Solidus::ConsoleSystem::Instance().AddToLog( "QUIT - Exits the application." );
}

void ConsoleCommand_Clear( const std::string& argsString )
{
	if( argsString.length() > 0 ) 
	{
		Solidus::ConsoleSystem::Instance().PrintErrorMessage( "EXIT does not take arguments." );
	}

	Solidus::ConsoleSystem::Instance().Clear();
}

void ConsoleCommand_Say( const std::string& argsString )
{	
	Solidus::ConsoleSystem::Instance().AddToLog( argsString );
}

void ConsoleCommand_SetColor( const std::string& argsString )
{
	if( Solidus::StringExtensions::ToUpper(  argsString ) == "RED" )
	{
		Solidus::ConsoleSystem::Instance().SetFontColor( Color( 1.0f, 0.0f, 0.0f, 1.0f ) );
	}
	else if( Solidus::StringExtensions::ToUpper( argsString ) == "YELLOW" )
	{
		Solidus::ConsoleSystem::Instance().SetFontColor( Color( 1.0f, 1.0f, 0.0f, 1.0f ) );
	}
	else if( Solidus::StringExtensions::ToUpper( argsString ) == "BLUE" )
	{
		Solidus::ConsoleSystem::Instance().SetFontColor( Color( 0.0f, 0.0f, 1.0f, 1.0f ) );
	}
	else if( Solidus::StringExtensions::ToUpper( argsString ) == "GREEN" )
	{
		Solidus::ConsoleSystem::Instance().SetFontColor( Color( 0.0f, 1.0f, 0.0f, 1.0f ) );
	}
	else
	{
		Solidus::ConsoleSystem::Instance().PrintErrorMessage( "Color not supported!" );
	}
}

void ConsoleCommand_Exit( const std::string& argsString )
{
	if( argsString.length() > 0 ) 
	{
		Solidus::ConsoleSystem::Instance().PrintErrorMessage( "EXIT does not take arguments." );
	}

	// Close the program
	//
	exit( 0 );
}

void ConsoleCommand_ExportLog( const std::string& argsString )
{
	if( argsString.length() > 0 ) 
	{
		Solidus::ConsoleSystem::Instance().PrintErrorMessage( "EXPORTLOG does not take arguments." );
	}

	Solidus::ConsoleSystem::Instance().ExportLogToHTML();
}

//void ConsoleCommand_AddLights( const std::string& argsString )
//{
//
//}

void ConsoleCommand_EnableFBO( const std::string& argsString )
{
	if( argsString.length() > 0 ) 
	{
		Solidus::ConsoleSystem::Instance().PrintErrorMessage( "ENABLEFBO does not take arguments." );
	}

	g_RenderParameters.FrameBufferPipelineEnabled = true;
}

void ConsoleCommand_DisableFBO( const std::string& argsString )
{
	if( argsString.length() > 0 ) 
	{
		Solidus::ConsoleSystem::Instance().PrintErrorMessage( "DISABLEFBO does not take arguments." );
	}

	g_RenderParameters.FrameBufferPipelineEnabled = false;
}

void ConsoleCommand_RotateModel( const std::string& argsString )
{
	if( argsString.length() > 0 ) 
	{
		Solidus::ConsoleSystem::Instance().PrintErrorMessage( "ROTATEMODEL does not take arguments." );
	}

	g_RenderParameters.RotateModel = !g_RenderParameters.RotateModel;
}

void ConsoleCommand_RotateModelRatio( const std::string& argsString )
{
	if( argsString.length() > 0 ) 
	{
		float temp = static_cast< float >( ::atof( argsString.c_str() ));
		g_RenderParameters.RotationAngleRatio = temp;
	}	
	else
	{
		Solidus::ConsoleSystem::Instance().PrintErrorMessage( "ROTATEMODELRATIO takes 1 argument. E.g. ROTATEMODELRATIO 1.5" );
	}
}
