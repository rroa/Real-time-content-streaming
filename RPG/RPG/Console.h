#pragma once

#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <string>
#include <vector>
#include <map>
#include <utility>

#include "Vector4.h"
#include "Trie.h"

namespace Solidus
{	
	class ConsoleSystem
	{
		typedef void (*ConsoleCommandFuncPtr)( const std::string& argsString );
		typedef std::map<std::string, ConsoleCommandFuncPtr> script_map;
		typedef Vector4 Color;

	public:
		void RegisterConsoleCommand( const std::string& cmdName, ConsoleCommandFuncPtr );
		void ExecuteConsoleCommand( const std::string cmdName, std::string args );
		void ExportLogToHTML();

		// FUNCTIONS
		//
		static void		      Create();
		static void		      Create( int width, int height );
		static void		      Destroy();
		static ConsoleSystem& Instance();		

		// FUNCTIONS
		//		
		void			 PrintErrorMessage( const std::string& errMsg );		
		void			 UpdateInput( const unsigned char& letter );
		void			 Draw();
		void			 AddToLog( const std::string& msg );
		void			 PreviousCommand();
		void			 NextCommand();
		void			 MoveCursorLeft();
		void			 MoveCursorRight();

		// INLINE
		//		
		inline void		 SetFontColor( const Color& fontColor ) { m_defaultColor = fontColor; }
		inline void		 SetVisibility( const bool& _visible ) { m_visible = _visible; }
		inline const int GetRows() const { return m_rows; }
		inline const int IsVisibile() const { return m_visible; }
		inline void		 Clear()   
		{ 
			m_rawLog.clear();
			m_compoundLog.clear(); 
		}		
	private:		
		// CTOR
		//
		ConsoleSystem();
		ConsoleSystem( int width, int height );
		ConsoleSystem( ConsoleSystem const& ) {};
		ConsoleSystem& operator=( ConsoleSystem const& ) {};
		~ConsoleSystem();

		// FUNCTIONS
		//
		void	  RegisterCommands();
		void	  DefaultBuffer();
		void	  DrawBackground();
		void	  DrawCursor();
		void	  DrawBuffer();
		void      DrawLog();
		void	  DrawSuggestions();
		void      DrawString( const std::string& text, float x, float y, int fontSize, Color color );
		void	  Init();
		void	  Cleanup();
		void	  EvaluateCommand( const std::string& command, const std::vector< std::string >& parsedText );
		void	  ParseCommand( const std::string& textCommand );
		
		// DATA
		//
		
		// COMMANDS
		size_t					   m_currentCommandIndex;
		size_t					   m_currentCursorIndex;
		script_map				   m_registeredCommands;
		std::vector< std::string > m_dictionary;
		std::vector< int >		   m_suggestions;
		std::vector< char >		   m_commandBuffer;
		std::unique_ptr< Trie >	   m_trie;
		bool					   m_autoCompleted;
		int						   m_currentAutoCompleteIndex;
		bool					   m_blink;
		
		// CONSOLE		
		int						   m_width;
		int						   m_height;
		float					   m_padding;
		int						   m_currentRowIndex;		
		int						   m_rows;
		int						   m_fontSize;
		Color					   m_defaultColor;
		Color					   m_suggestionsColor;
		Color					   m_errorMessageColor;
		bool					   m_visible;

		// SINGLETON
		static ConsoleSystem*						  s_instance;
		std::vector< std::string >					  m_rawLog;
		std::vector< std::pair< std::string, Color >> m_compoundLog;			
	};
}

#endif