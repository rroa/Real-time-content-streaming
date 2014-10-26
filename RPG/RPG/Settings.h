#pragma once

#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include <string>
#include <map>
#include <vector>
#include "XMLExtensions.h"

namespace
{	
	enum RenderAPI
	{
		OPENGL,   // OpenGL (Open Graphics Library)[3] is a cross-language, multi-platform API for rendering 2D and 3D computer graphics.
		DIRECT3D, // Direct3D is part of Microsoft's DirectX application programming interface (API). Direct3D is available for Microsoft Windows operating systems (Windows 95 and above), and for other platforms through the open source software Wine.
		RENDER_API_TYPES
	};

	enum ShaderType
	{
		FRAGMENT_SHADER = 0x8B30, // A Fragment Shader is a user-supplied program that, when executed, will process a Fragment from the rasterization process into a set of colors and a single depth value.
		VERTEX_SHADER   = 0x8B31, // A Vertex Shader is the programmable Shader stage in the rendering pipeline that handles the processing of individual vertices.
		GEOMETRY_SHADER = 0x8DD9, // A Geometry Shader (GS) is a Shader program written in GLSL that governs the processing of primitives.
		SHADER_TYPES    = 3
	};

	enum TextureType
	{
		INVALID_TYPE	  = -1,
		DIFFUSE_TEXTURE   =  0,
		NORMALMAP_TEXTURE =  1,
		SPECULAR_TEXTURE  =  2,
		EMISSIVE_TEXTURE  =  3,
		HEIGHTMAP_TEXTURE =  4,
		NUM_TEXTURES	  =  6
	};

	enum LightBehaviorParameters
	{
		LIGHT_INITIALSTATE		  = 0,
		LIGHT_ROTATIONOVERX		  = 1,
		LIGHT_ROTATIONOVERY		  = 2,
		LIGHT_ROTATIONOVERZ		  = 4,
		LIGHT_ROTATIONOVERXYZ	  = 8,
		LIGHT_NOMOVEMENT		  = 16,
		LIGHT_ON				  = 32,
		LIGHT_SHOW				  = 64,
		LIGHT_SHOWDIRECTION		  = 128,
		LIGHT_MOVESPOTONLY		  = 256,
		LIGHT_MOVEPOINTONLY		  = 512,
		LIGHT_MOVEDIRECTIONALONLY = 1024,
		LIGHT_INVALIDFLAG         = 2048
	};
}

namespace Solidus
{
	struct FrameBufferObjectRules
	{
		FrameBufferObjectRules()
			: ColorTextureSlot( 0 )
			, DepthTextureSlot( 0 )
		{}

		FrameBufferObjectRules( int colorTextureSlot, int depthTextureSlot )
			: ColorTextureSlot( colorTextureSlot )
			, DepthTextureSlot( depthTextureSlot )
		{}

		int ColorTextureSlot;
		int DepthTextureSlot;
	};

	class Settings
	{
	public:	
		// SINGLETON FUNCTIONS
		//
		static void Create( const char* szPath );
		static void Destroy();	
		static Settings& Instance();	

		// Static Members
		static FrameBufferObjectRules FBORules; 
		static int					  LightBehaviorRules;

		// FUNCTIONS
		//
		std::string GetShaderExtension( const int& type_ );
		std::string GetTextureUniformName( const int& type_ );

		// INLINE
		//
		inline bool								ShowWindowMenu() const      { return m_showWindowMenu; }
		inline RenderAPI						GetRenderAPI() const        { return m_renderAPI; }
		inline std::string						GetShadersPath() const      { return m_shadersPath; }
		inline std::vector< ShaderType > const	GetSupportedShaders() const { return m_supportedShaders; }

		inline std::string						CurrentVersion() const	{ return m_version; }
		inline std::string						GetMapsConfigPath() const	{ return m_mapsPath; }
		inline std::string						GetEntitiesConfigPath() const { return m_entitiesConfigFile; }
		inline std::string						GetTilesConfigPath() const	{ return m_tileTypeConfigFile; }
	private:
		//Functions
		//
		void LoadConfig( const char* szFilePath );		
		void LoadBluePrints();

		// Setting the entity as non-copyable
		//
		Settings();
		Settings( const char * defaultPath );
		Settings( Settings const& ) {};
		Settings& operator=( Settings const& ) {};

		// DATA Members
		//
		bool								 m_showWindowMenu;
		RenderAPI							 m_renderAPI;
		std::vector< ShaderType >			 m_supportedShaders;
		std::string							 m_shadersPath;
		static Settings*					 s_instance;
		XMLNode								 m_configRoot;
		std::map< std::string, ShaderType >  m_shadersMap;
		std::map< std::string, RenderAPI >   m_renderAPIs;
		std::map< ShaderType, std::string >  m_exts;
		std::map< TextureType, std::string > m_textureUniforms;

		// References to other config files
		std::string							 m_mapsPath;
		std::string							 m_tileTypeConfigFile;
		std::string							 m_entitiesConfigFile;

		// Current version
		std::string							m_version;
	};
}
#endif

// TODO: Add context management. E.g. Are watching the scene or are we watching the console?