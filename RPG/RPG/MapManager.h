#pragma once


#ifndef _MAP_MANAGER_H_
#define _MAP_MANAGER_H_

#include <map>
#include <string>

#include "Map.h"
#include "TileType.h"
#include "XMLExtensions.h"

namespace Solidus
{
	class Agent;
	class BluePrintBase;
	class MapManager
	{
	public:
		/* =============================================================
		 * FUNCTIONS
		 * ============================================================= */

		static void							Create( const bool& force = false );
		static void							Destroy();
		std::shared_ptr< Map >				GetMap( const std::string& mapName );
		std::shared_ptr< Map >				GetMap( const char* mapName );
		bool								SetMap( const std::string& mapName );
		bool								SetMap( const char* mapName );
		static MapManager&					Instance();

		/* =============================================================
		 * INLINE
		 * ============================================================= */

		inline const std::shared_ptr< Map > CurrentMap() const { return m_currentMap; }
	private:
		/* =============================================================
		 * CTOR
		 * ============================================================= */

		MapManager() {}		
		MapManager( MapManager const& ) {};
		MapManager& operator=( MapManager const& ) {};		

		/* =============================================================
		 * PRIVATE FUNCTIONS
		 * ============================================================= */

		void							DumpAgents( const XMLNode& node, const std::shared_ptr< Map >& currentMap, bool scanForItems = false );
		void							DumpEntities( const XMLNode& node, const std::shared_ptr< Map >& currentMap, bool scanForItems = false );
		void							DumpItems( const XMLNode& node, const std::shared_ptr< Map >& currentMap, Entity* entity = nullptr );
		std::shared_ptr< TileType>		GetTileType( const std::string& tileTypeName );
		std::shared_ptr< BluePrintBase> GetEntityBluePrint( const std::string& entityName );
		void							LoadValidTileTypes();
		void							LoadValidEntities();
		void							LoadMapsFromPath();		
		/* =============================================================
		 * DATA
		 * ============================================================= */

		static MapManager*										  s_instance;
		std::map< std::string, std::shared_ptr< Map >>			  m_maps;
		std::map< std::string, std::shared_ptr< TileType >>		  m_ValidTileTypes;
		std::map< std::string, std::shared_ptr< BluePrintBase >>  m_ValidEntities;
		std::shared_ptr< Map >									  m_currentMap;
	};
}

#endif