#include "stdafx.h"
#include "Settings.h"
#include "XMLExtensions.h"
#include "Entity.h"
#include "BluePrintManager.h"
#include "Item.h"
#include "Tile.h"
#include "AppInfo.h"

namespace Solidus
{
	// Initialization
	//
	Settings* Settings::s_instance = nullptr;
	const char * defaultPath = "app.xml";
	FrameBufferObjectRules Settings::FBORules = FrameBufferObjectRules( 0, 0 );
	int Settings::LightBehaviorRules = 0;

	Settings::Settings()
	{}

	Settings::Settings( const char * defaultPath )
	{
		m_renderAPIs[ "GL" ] = OPENGL;
		m_renderAPIs[ "D3D" ] = DIRECT3D;
		
		m_shadersMap[ "FragmentShader" ] = FRAGMENT_SHADER;
		m_shadersMap[ "VertexShader" ] = VERTEX_SHADER;
		m_shadersMap[ "GeometryShader" ] = GEOMETRY_SHADER;

		LoadConfig( defaultPath );		
	}

	void Settings::Create( const char* szPath )
	{
		if( !s_instance )
		{
			s_instance = new Settings( szPath );
		}
	}

	void Settings::Destroy()
	{
		if( s_instance )
		{					
			// Resetting the instance
			delete s_instance;
			s_instance = nullptr;
		}		
	}

	Settings& Settings::Instance()
	{
		if( !s_instance )
		{
			Create( defaultPath );
		}

		// Throw ex if the file was not loaded
		//
		if( !s_instance->m_configRoot ) throw;

		return *s_instance;
	}

	void Settings::LoadConfig( const char* szFilePath )
	{
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_file( szFilePath );
		
		// TODO: Add message to this exception
		if( !result ) throw;
		
		pugi::xml_node app = doc.child( "application" );
		m_configRoot = app;
		
		// TODO: Add message to this exception
		if( !m_configRoot ) throw;

		m_version = GetStringXMLAttribute( m_configRoot, "version", StringExtensions::EmptyString );

		g_AppInfo.CurrentVersion = m_version;

		// Load Engine options		
		std::string renderAPI = GetStringXMLAttribute( m_configRoot, "renderAPI", "GL" );
		if( renderAPI.length() > 0 ) 
		{
			m_renderAPI = m_renderAPIs[ renderAPI ];
		}

		// Load Window options		
		bool showWindowMenu = GetBoolXMLAttribute( m_configRoot, "showWindowMenu", false );
		m_showWindowMenu = showWindowMenu;		

		// Load FBO's
		pugi::xml_node frameBuffers = m_configRoot.child( "frameBuffers" );
		if( frameBuffers )
		{
			Settings::FBORules.ColorTextureSlot = GetIntXMLAttribute( frameBuffers, "colorTextureSlot", 0 );
			Settings::FBORules.DepthTextureSlot = GetIntXMLAttribute( frameBuffers, "depthTextureSlot", 1 );			
		}

		// Load shaders
		pugi::xml_node shaders = m_configRoot.child( "shaders" );
		m_shadersPath = GetStringXMLAttribute( shaders, "directory", "" );

		if( shaders )
		{
			//m_shadersPath = shaders.attribute( "directory ").value();

			for ( pugi::xml_node shader = shaders.child( "shader" ); shader; shader = shader.next_sibling("shader") )
			{				
				std::string name = GetStringXMLAttribute( shader, "name", "" );
				if( name.empty() ) continue;

				m_supportedShaders.push_back( m_shadersMap[ name ] );
				m_exts.insert( std::pair< ShaderType, std::string >( m_shadersMap[ name ], GetStringXMLAttribute( shader, "extension", "" )));
			}

			for ( pugi::xml_node textureUniform = shaders.child( "textureUniform" ); textureUniform; textureUniform = textureUniform.next_sibling("textureUniform") )
			{
				std::string name = GetStringXMLAttribute( textureUniform, "name", "" );
				if( name.empty() ) continue;

				auto textureType = static_cast< TextureType >( GetIntXMLAttribute( textureUniform, "textureSlot", 0 ));
				m_textureUniforms.insert( std::pair< TextureType, std::string > ( textureType, name ));
			}
		}

		// Load extra config files		
		pugi::xpath_node configuration = m_configRoot.child("configuration");		
		for ( auto configEntry = configuration.node().begin(); configEntry != configuration.node().end(); ++configEntry )
		{			
			std::string name( configEntry->name() );
			if( name == "entities" )
			{
				m_entitiesConfigFile = GetStringXMLAttribute( *configEntry, "config", "entities.xml" );
			}

			if( name == "maps" )
			{
				m_mapsPath			 = GetStringXMLAttribute( *configEntry, "path", "Maps/" );
				m_tileTypeConfigFile = GetStringXMLAttribute( *configEntry, "tilesDefinition", "tiles.xml" );
			}			
		}

		LoadBluePrints();
	}

	std::string Settings::GetShaderExtension( const int& type_ )
	{		
		return m_exts.find( static_cast< ShaderType >( type_ ) )->second;
	}

	std::string Settings::GetTextureUniformName( const int& type_ )
	{
		return m_textureUniforms.find( static_cast< TextureType >( type_ ) )->second;
	}

	void Settings::LoadBluePrints()
	{
		pugi::xml_document entitiesDocument;
		pugi::xml_parse_result entitiesDocumentResult = entitiesDocument.load_file( m_entitiesConfigFile.c_str() );

		// TODO: Add message to this exception
		if( !entitiesDocumentResult ) throw;

		pugi::xml_node entitiesRootNode = entitiesDocument.child( "entities" );

		// TODO: Add message to this exception
		if( !entitiesRootNode ) throw;
		
		for( auto child = entitiesRootNode.begin(); child != entitiesRootNode.end(); ++child )
		{
			std::string nodeName = child->name();
			std::string entityName = GetStringXMLAttribute( *child, "name", "" );
			if( nodeName == "entity" )
			{
				BluePrintManager::Instance().CreateBluePrint< Entity >( entityName, *child );
			}

			if( nodeName == "agent" )
			{
				BluePrintManager::Instance().CreateBluePrint< Agent >( entityName, *child );
			}

			if( nodeName == "tile" )
			{
				BluePrintManager::Instance().CreateBluePrint< Tile >( entityName, *child );
			}

			if( nodeName == "item" )
			{
				BluePrintManager::Instance().CreateBluePrint< Item >( entityName, *child );
			}			
		}
	}
}