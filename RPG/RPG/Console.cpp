#include "stdafx.h"
#include "Console.h"

#include <fstream>
#include <time.h>

#include "TextManager.h"
#include "ResourceManager.h"
#include "pugixml.hpp"
#include "TextureManager.h"
#include "ReportErrors.h"
#include "InputManager.h"
#include "ConsoleCommands.h"
#include "StringExtensions.h"
#include "Geometry.h"
#include "Utilities.h"

namespace Solidus
{
	const int _bufferDefaultLength = 3;

	ConsoleSystem::ConsoleSystem()
		: m_width( 0 )
		, m_height( 0 )		
		, m_padding( 0.0f )
		, m_currentRowIndex( 0 )
		, m_currentCommandIndex( 0 )
		, m_currentCursorIndex( 1 )
		, m_fontSize( 30 )
		, m_defaultColor( 1.0f, 1.0f, 0.0f, 1.0f )
		, m_suggestionsColor( 1.0f, 1.0f, 0.0f, 1.0f )
		, m_errorMessageColor( 1.0f, 0.0f, 0.0f, 1.0f )
		, m_visible( false )
		, m_autoCompleted( false )
		, m_currentAutoCompleteIndex( -1 )
		, m_blink( true )
	{
		Init();
	}

	ConsoleSystem::ConsoleSystem( int width, int height )		
		: m_width( width )
		, m_height( height )		
		, m_padding( 0.3f )
		, m_currentRowIndex( 0 )
		, m_currentCommandIndex( 0 )
		, m_currentCursorIndex( 1 )
		, m_fontSize( 25 )
		, m_defaultColor( 0.0f, 1.0f, 0.0f, 1.0f )
		, m_suggestionsColor( 1.0f, 1.0f, 0.0f, 1.0f )
		, m_errorMessageColor( 1.0f, 0.0f, 0.0f, 1.0f )
		, m_visible( false )
		, m_autoCompleted( false )
		, m_currentAutoCompleteIndex( -1 )
		, m_blink( true )
	{
		Init();
	}

	ConsoleSystem::~ConsoleSystem()
	{
		Cleanup();
	}

	void ConsoleSystem::Init()
	{
		m_trie.reset( new Trie );

		float calcRows = static_cast< float >( m_height ) / ( m_fontSize );
		m_rows         = static_cast< int >( calcRows );

		// Registering the commands
		//
		RegisterCommands();

		// Loading the default command prompt prefix (>
		//
		DefaultBuffer();
	}

	void ConsoleSystem::EvaluateCommand( const std::string& command, const std::vector< std::string >& parsedText )
	{
		// Change the evaluations to use the actual commands dictionary
		//
		if( command == "SAY")
		{
			std::string message;
			for( int i = 1; i < static_cast< int>( parsedText.size() ); ++i )
			{
				message.append( parsedText[ i ] );
				message.append( " " );
			}

			ExecuteConsoleCommand( command, message );
		}
		else if( command == "SETCOLOR")
		{
			std::string message;
			for( int i = 1; i < static_cast< int>( parsedText.size() ); ++i )
			{
				message.append( parsedText[ i ] );						
			}

			ExecuteConsoleCommand( command, message );
		}
		else if( command == "CLEAR" )
		{
			ExecuteConsoleCommand( command, "" );
		}
		else if( command == "HELP" )
		{
			ExecuteConsoleCommand( command, "" );
		}
		else if( command == "EXPORTLOG" )
		{
			ExecuteConsoleCommand( command, "" );
		}
		else if( command == "ADDLIGHTS" )
		{
			ExecuteConsoleCommand( command, "" );
		}
		else if( command == "ENABLEFBO" )
		{
			ExecuteConsoleCommand( command, "" );
		}
		else if( command == "DISABLEFBO" )
		{
			ExecuteConsoleCommand( command, "" );
		}
		else if( command == "ROTATEMODEL" )
		{
			ExecuteConsoleCommand( command, "" );
		}
		else if( command == "ROTATEMODELRATIO" )
		{
			std::string message;
			for( int i = 1; i < static_cast< int>( parsedText.size() ); ++i )
			{
				message.append( parsedText[ i ] );
				message.append( " " );
			}

			float f;
			if( !from_string<float>( f, message, std::dec ))
			{
				PrintErrorMessage( "INVALID INPUT" );
			}
			else
			{
				ExecuteConsoleCommand( command, message );
			}
		}
		else if( command == "QUIT" )
		{
			ExecuteConsoleCommand( command, "" );
		}
		else
		{
			if( parsedText.size() != _bufferDefaultLength )
			{
				std::string message("Invalid command format: ");
				for( int i = 0; i < static_cast< int>( parsedText.size() ); ++i )
				{
					message.append( parsedText[ i ] );
					message.append( " " );
				}
				PrintErrorMessage( message );
			}
			else
			{
				ExecuteConsoleCommand( parsedText[ 0 ], parsedText[ 1 ] );
			}
		}
	}

	void ConsoleSystem::ParseCommand( const std::string& textCommand )
	{
		// Splitting the buffer in order to evaluate the arguments
		//
		std::vector< std::string > parsed = StringExtensions::Split( textCommand, " " );

		// Up-casing the text
		//				
		const std::string command = StringExtensions::ToUpper( parsed[ 0 ] );

		// Evaluating that the first argument is a valid command
		//
		script_map::const_iterator iter = m_registeredCommands.find( command );

		if ( iter == m_registeredCommands.end() )
		{
			// Building the error string
			//
			std::string message("Invalid command: ");
			message.append( textCommand );

			PrintErrorMessage( message  );

			//Clearing the command buffer
			//
			m_commandBuffer.clear();
			m_currentRowIndex = 0;
			m_currentCommandIndex = 0;

			// Setting the default buffer values
			//
			DefaultBuffer();
			return;
		}

		// Evaluating the command
		//
		EvaluateCommand( command, parsed );
	}

	void ConsoleSystem::UpdateInput( const unsigned char& letter )
	{		
		if( letter == 0x60 ) return;		
		if( letter == 0x0D || letter == 0x0A )
		{			
			// Building the text from the command buffer
			//
			std::string buffer( m_commandBuffer.begin() + _bufferDefaultLength, 
				m_commandBuffer.end() );

			m_rawLog.push_back( buffer );

			// Evaluating if the command is long enough to be checked
			//
			int length = buffer.length();
			if( length > 0 )
			{				
				// Displaying the submitted command
				AddToLog( ">>> " + buffer + " <<< " );

				ParseCommand( buffer );
			}

			// Clearing the command buffer
			//
			m_autoCompleted = false;
			m_currentAutoCompleteIndex = -1;
			m_suggestions.clear();
			m_commandBuffer.clear();
			m_currentRowIndex = 0;
			m_currentCommandIndex = 0;
			DefaultBuffer(); // Shows the (> prefix on the command prompt
		}
		else if( letter == 0x09 )
		{			
			if( m_suggestions.size() > 0 )
			{
				m_autoCompleted = true;
				m_currentAutoCompleteIndex++;
				if( m_currentAutoCompleteIndex >= static_cast< int >( m_suggestions.size() ))
				{
					m_currentAutoCompleteIndex = 0;
				}

				int index = m_suggestions[ m_currentAutoCompleteIndex ];
				
				m_commandBuffer.clear();
				DefaultBuffer();
				const std::string command = m_dictionary[ index ];
				for( int i = 0; i < static_cast< int >( command.size() ); ++i )
				{
					m_commandBuffer.push_back( command[ i ] );
				}	
				m_commandBuffer.push_back(' ');
			}
		}
		else if( letter == 0x08 )
		{
			if( m_commandBuffer.size() > 3 )
			{
				m_commandBuffer.pop_back();
				m_autoCompleted = false;
				m_currentAutoCompleteIndex = -1;
			}			
		}
		else if( letter == 0x21 )
		{
			if( static_cast< int>( m_compoundLog.size() ) > m_rows )
			{				
				if( ( m_currentRowIndex + m_rows ) - 1 >= static_cast< int>( m_compoundLog.size() ))
				{
					
				}
				else
				{
					m_currentRowIndex++;
				}
			}			
		}
		else if( letter == 0x22 )
		{
			m_currentRowIndex--;
			if( m_currentRowIndex < 0 )
			{
				m_currentRowIndex = 0;
			}
		}
		else
		{
			/*if( m_currentFont.CalcStringPixelWidth( buffer ) > m_width )
				{
				std::string parsedBuffer( buffer );
				int startingIndex = 0;
				for( int i = 0; i < length; ++i )
				{						
				if( m_currentFont.CalcStringPixelWidth( parsedBuffer.substr( startingIndex, i ) ) > m_width )
				{
				m_rawLog.push_back( parsedBuffer.substr( startingIndex, i ) );
				m_compoundLog.push_back( std::make_pair( parsedBuffer.substr( startingIndex, i ), m_defaultColor ) );
				startingIndex = i;
				}
				}
				m_rawLog.push_back( parsedBuffer.substr( startingIndex, ( length - startingIndex ) - 1 ) );
				m_compoundLog.push_back( std::make_pair( parsedBuffer.substr( startingIndex, ( length - startingIndex ) - 1 ), m_defaultColor ) );
				}
				else
				{
				m_rawLog.push_back( buffer );
				m_compoundLog.push_back( std::make_pair( buffer, m_defaultColor ) );
				}				*/
			m_commandBuffer.push_back( letter );
		}
	}

	void ConsoleSystem::DefaultBuffer()
	{		
		m_commandBuffer.push_back( '(' );
		m_commandBuffer.push_back( '>' );
		m_commandBuffer.push_back( ' ' );
	}

	void ConsoleSystem::DrawString( const std::string& text, float x, float y, int fontSize, Color color )
	{
		if( text.length() == 0 ) return;
		
		// Offset
		//
		float yOffset = m_rows * 0.23f * m_fontSize;

		// Print Text
		//
		TextManager::Instance().Print( text, x, y, fontSize, color, 0.0f, yOffset );		
	}

	void ConsoleSystem::Cleanup()
	{
		// Clear buffers
		//
		m_commandBuffer.clear();
		m_rawLog.clear();
	}

	void ConsoleSystem::DrawLog()
	{
		int index = 2;

		for ( std::vector< std::pair< std::string, Color >>::reverse_iterator rit = m_compoundLog.rbegin() + m_currentRowIndex; 
			rit != m_compoundLog.rend() && index <= m_rows; ++rit )
		{
			DrawString( (*rit).first, 0.0f, static_cast< float >( ( m_fontSize * index )), m_fontSize, (*rit).second );
			++index;
		}
	}

	void ConsoleSystem::DrawBuffer()
	{
		std::string currentBuffer( m_commandBuffer.begin(), m_commandBuffer.end() );
		std::string rawBuffer( m_commandBuffer.begin() + _bufferDefaultLength, m_commandBuffer.end() );

		if( currentBuffer.length() > 0 )
		{								
			if( !m_autoCompleted )
			{
				// Clearing the suggestions buffer
				//
				m_suggestions.clear();
				m_currentAutoCompleteIndex = -1;
			}

			// If there's a space do not suggest
			//
			auto splits = StringExtensions::Split( rawBuffer, " " );
			if( splits.size() == 1 && !m_autoCompleted )
			{
				// Search!
				//
				m_suggestions = m_trie->Search( StringExtensions::ToUpper( rawBuffer ).c_str() );
				m_autoCompleted = false;
			}			

			// Drawing the command line
			DrawString( currentBuffer, 0.0f, static_cast< float >( m_fontSize ), m_fontSize, m_defaultColor );
		}		
	}

	void ConsoleSystem::DrawBackground()
	{
		m_width = 1134;
		m_height = 526;

		// Clearing the buffer
		//
		glClear( GL_DEPTH_BUFFER_BIT );

		// Disabling obstacle GL settings
		//
		glUseProgram( 0 ); // Reset the shader		

		CommonGLDisables();

		// Switch to the projection matrix
		//
		glMatrixMode( GL_PROJECTION );
		glPushMatrix();
		glLoadIdentity();

		glOrtho( 0.0, m_width, m_height * 1.25f , 0.0, -1.0, 10.0 );
		
		glMatrixMode( GL_MODELVIEW );		
		glPushMatrix();
		glLoadIdentity();		
		
		// Console Area
		//
		glBegin( GL_QUADS );
			glColor4f( 0.0f, 0.0f, 0.0, 0.5f );
			glVertex2f( 0.0f, 0.0f );
			glVertex2f( static_cast< float >( m_width ), 0.0f );
			glVertex2f( static_cast< float >( m_width ), static_cast< float >( m_height ));
			glVertex2f( 0.0f, static_cast< float >( m_height ));
		glEnd();
				
		// The line at the end of the console
		//
		const float LineWidth = 3.0f;
		glBegin( GL_QUADS );
			glColor4f( m_defaultColor.x, m_defaultColor.y, m_defaultColor.z, 1.0f );
			glVertex2f( 0.0f, static_cast< float >( m_height ) );
			glVertex2f( static_cast< float >( m_width ), static_cast< float >( m_height ));
			glVertex2f( static_cast< float >( m_width ), static_cast< float >( m_height ) + LineWidth);
			glVertex2f( 0.0f, static_cast< float >( m_height ) + LineWidth);
		glEnd();

		// Making sure we can render 3d again
		glMatrixMode( GL_PROJECTION );
		glPopMatrix();

		glMatrixMode( GL_MODELVIEW );
		glPopMatrix();
		
		CommonGLEnables();
	}

	void ConsoleSystem::Draw()
	{
		DrawBackground();
		DrawBuffer();
		DrawCursor();
		DrawLog();
		DrawSuggestions();
	}

	/*
	###########################
		SINGLETON
	###########################
	*/

	ConsoleSystem* ConsoleSystem::s_instance = nullptr;	

	void ConsoleSystem::Create()
	{
		if( !s_instance )
		{
			s_instance = new ConsoleSystem;
		}
	}

	void ConsoleSystem::Create( int width, int height )
	{
		if( !s_instance )
		{
			s_instance = new ConsoleSystem( width, height );
		}
	}

	void ConsoleSystem::Destroy()
	{		
		if( s_instance )
		{
			// Clearing the map
			s_instance->Cleanup();

			// Resetting the instance
			delete s_instance;
			s_instance = nullptr;
		}		
	}

	ConsoleSystem& ConsoleSystem::Instance()
	{
		if( !s_instance )
		{
			Create();
		}

		return *s_instance;
	}

	void ConsoleSystem::AddToLog( const std::string& msg )
	{		
		m_compoundLog.push_back( std::make_pair( msg, m_defaultColor ) );
	}

	void ConsoleSystem::PrintErrorMessage( const std::string& errMsg )
	{		
		m_compoundLog.push_back( std::make_pair( errMsg, Color( m_errorMessageColor ) ) );
	}

	void ConsoleSystem::RegisterConsoleCommand( const std::string& cmdName, ConsoleCommandFuncPtr function )
	{
		m_dictionary.push_back( cmdName.c_str() );
		m_trie->AddWord( cmdName.c_str(), int( m_dictionary.size() - 1));
		m_registeredCommands.insert( std::make_pair( cmdName, function ));

	}

	void ConsoleSystem::ExecuteConsoleCommand( const std::string cmdName, std::string args )
	{		
		script_map::const_iterator iter = m_registeredCommands.find( cmdName );
		if ( iter == m_registeredCommands.end() )
		{
			PrintErrorMessage( "Command not found!" );
		}

		// Execute the function
		//
		(*iter).second( args );
	}

	void ConsoleSystem::RegisterCommands()
	{
		RegisterConsoleCommand( "HELP",				&ConsoleCommand_Help );
		RegisterConsoleCommand( "SAY",				&ConsoleCommand_Say );		
		RegisterConsoleCommand( "CLEAR",			&ConsoleCommand_Clear );		
		RegisterConsoleCommand( "SETCOLOR",			&ConsoleCommand_SetColor );		
		//RegisterConsoleCommand( "ADDLIGHTS",		&ConsoleCommand_AddLights );
		RegisterConsoleCommand( "ENABLEFBO",		&ConsoleCommand_EnableFBO );
		RegisterConsoleCommand( "DISABLEFBO",		&ConsoleCommand_DisableFBO );
		RegisterConsoleCommand( "ROTATEMODEL",		&ConsoleCommand_RotateModel );
		RegisterConsoleCommand( "ROTATEMODELRATIO", &ConsoleCommand_RotateModelRatio );
		RegisterConsoleCommand( "EXPORTLOG",		&ConsoleCommand_RotateModelRatio );
		RegisterConsoleCommand( "QUIT",				&ConsoleCommand_Exit );
	}

	void ConsoleSystem::ExportLogToHTML()
	{
		std::ofstream log;
		log.open ("console.html");

		log << "<html>";
		log << "<head><title>Raul's Console Log</title></head>";
		log << "<body style=\"color:rgb(255,255,255);background-color:rgb( 0, 0, 0)\">";
		log << "<h1>Console Log</h1>";
		log << "<ul>";
		for ( std::vector< std::pair< std::string, Color >>::iterator it = m_compoundLog.begin() + m_currentRowIndex; 
			it != m_compoundLog.end(); ++it )
		{
			char style[256];
			int r, g, b;
			r = static_cast< int >( (*it).second.x * 255 );
			g = static_cast< int >( (*it).second.y * 255 );
			b = static_cast< int >( (*it).second.z * 255 );
			sprintf( style, "style=\"color:rgb( %d, %d, %d)\"", 
				r , g, b );
			log << "<li "<< style << ">" << ( *it ).first << "</li>";
		}

		log << "</ul>";
		log << "</body>";
		log << "</html>";

		time_t rawtime;
		struct tm * timeinfo;
		char buffer [80];

		time (&rawtime);
		timeinfo = localtime (&rawtime);

		strftime (buffer,80,"Log saved at: %I:%M%p", timeinfo);

		AddToLog( buffer );

		log.close();
	}

	void ConsoleSystem::PreviousCommand()
	{		
		if( m_rawLog.size() == 0 || m_currentCommandIndex == m_rawLog.size()) return;
		
		m_commandBuffer.clear();
		m_currentCommandIndex++;

		int index = m_rawLog.size() - m_currentCommandIndex;	 
		if( index < 0 ) index = 0;

		std::string previousCommand = m_rawLog.at( index );
		int commandLength = static_cast< int >( previousCommand.length() );
		DefaultBuffer();
		for( int i = 0; i < commandLength; ++i )
		{
			m_commandBuffer.push_back( previousCommand[ i ] );
		}		
	}

	void ConsoleSystem::NextCommand()
	{	
		if( m_rawLog.size() == 0 || m_currentCommandIndex == 0 ) return;
		if( m_currentCommandIndex == 1 ) 
		{
			m_commandBuffer.clear();
			m_currentCommandIndex = 0;
			DefaultBuffer();
			return;
		}

		m_commandBuffer.clear();
		m_currentCommandIndex--;		

		int index = m_rawLog.size() - m_currentCommandIndex;
		if( index < 0 ) index = 0;

		std::string nextCommand = m_rawLog.at( index );
		int commandLength = static_cast< int >( nextCommand.length() );
		
		DefaultBuffer();

		for( int i = 0; i < commandLength; ++i )
		{
			m_commandBuffer.push_back( nextCommand[ i ] );
		}		
	}

	void ConsoleSystem::DrawCursor() // TODO: Needs to be fixed. It doesn't work properly
	{		
		m_blink = !m_blink;
		if( !m_blink ) return;
		std::string buffer( m_commandBuffer.begin(), m_commandBuffer.end() );
		float previousWidth = 0.0f;
		BitmapFont currentFont = TextManager::Instance().GetFont();

		if( buffer.size() > 0 )
		{
			const int index = buffer.size() - 1;
			const BitmapFontCharacter fontCharacter = currentFont.GetCharacter( buffer[ index ] );
			previousWidth = fontCharacter.A();
		}
		DrawString( "_", 0.5f + currentFont.CalcStringFontWidth( buffer ) + previousWidth, static_cast< float >( m_fontSize ), m_fontSize, m_defaultColor );
	}

	void ConsoleSystem::MoveCursorLeft()
	{
		if( m_currentCursorIndex == 0 ) return;
		m_currentCursorIndex--;
	}

	void ConsoleSystem::MoveCursorRight()
	{
		if( m_currentCursorIndex == m_commandBuffer.size() ) return;
		m_currentCursorIndex++;
	}

	void ConsoleSystem::DrawSuggestions()
	{
		if( m_autoCompleted ) return;
		int suggestions = m_suggestions.size() > 3 
			? 3 : static_cast< int >( m_suggestions.size() );
		
		for( int i = 1; i <= suggestions; ++i )
		{						
			DrawString( m_dictionary[ m_suggestions[ i - 1 ]], 0.0f, static_cast< float >( -m_fontSize * i ), 
				m_fontSize, m_suggestionsColor );
		}				
	}
}