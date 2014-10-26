#include "stdafx.h"
#include "MapManager.h"
#include "Settings.h"
#include "FileManager.h"
#include "ReportErrors.h"

#include <vector>
#include <sstream>
#include "StringExtensions.h"
#include "BluePrint.h"
#include "BluePrintManager.h"
#include "TextureManager.h"

namespace Solidus
{
	MapManager* MapManager::s_instance = nullptr;

	void MapManager::Create( const bool& force )
	{
		if( !s_instance || force )
		{							
			s_instance = new MapManager;

			// Loading tile type definitions
			//
			s_instance->LoadValidTileTypes();			

			// Loading the maps
			//
			s_instance->LoadMapsFromPath();
		}
	}

	MapManager& MapManager::Instance()
	{
		if( !s_instance )
		{
			Create();
		}

		return *s_instance;
	}

	void MapManager::Destroy()
	{
		if( s_instance )
		{
			// Clearing the map
			s_instance->m_maps.clear();

			// Resetting the instance
			delete s_instance;
			s_instance = nullptr;
		}		
	}

	std::shared_ptr< Map > MapManager::GetMap( const char* mapName )
	{
		// Check if the map was previously loaded
		auto ptrToFoundMap = m_maps.find( mapName );

		if( ptrToFoundMap != m_maps.end() )
		{
			// Found the map
			return ( ptrToFoundMap->second );
		}
		return nullptr;
	}

	std::shared_ptr< Map > MapManager::GetMap( const std::string& mapName )
	{
		return GetMap( mapName.c_str() );
	}


	void MapManager::LoadMapsFromPath()
	{
		// Load the files in the path
		//
		std::stringstream mapsPath;
		mapsPath << Settings::Instance().GetMapsConfigPath().c_str();
		mapsPath << "*.xml";

		/*std::vector< std::string > mapFiles = 
			ListFilesInDirectory( mapsPath.str() );
*/
		std::vector< std::string > mapFiles;
		mapFiles.push_back("map1.xml");
		mapFiles.push_back("map2.xml");

		for( auto ptrToMapFile = mapFiles.begin(); ptrToMapFile != mapFiles.end(); ++ptrToMapFile )
		{
			// Load the map doc
			//
			std::string fileName = ptrToMapFile->c_str();
				//BuildPath( Settings::Instance().GetMapsConfigPath().c_str(), ptrToMapFile->c_str(), "" );

			pugi::xml_document mapsDocument;
			pugi::xml_parse_result mapsDocumentResult = mapsDocument.load_file( fileName.c_str() );			 
			
			DEBUG( "Loading " << fileName );

			if( !mapsDocumentResult ) // Error Checking.
			{
				std::stringstream errorMessage;
				errorMessage << fileName
					<< " was not loaded properly. Check your map definition files.";

				ReportError( errorMessage.str() );
				return;				
			}

			// Load the root node of the file
			//
			XMLNode rootNode = mapsDocument.child( "maps" );
			if( !rootNode ) // Error Checking.
			{
				std::stringstream errorMessage;
				errorMessage << "The file "
					<< fileName
					<< " was skipped because the root node \"maps\" was not found.";

				ReportError( errorMessage.str() );
				continue;
			}

			DEBUG( "Root node for " << fileName << " loaded properly." );

			// Getting the maps
			//
			for( auto ptrToChildElement = rootNode.begin(); ptrToChildElement != rootNode.end(); ++ptrToChildElement )
			{
				bool mapElementIsValid = ValidateXMLAttributes( *ptrToChildElement, "name" );
				if( !mapElementIsValid )
				{					
					ReportError( "Skipping map element. The name attribute is required" );
					continue;
				}

				// Load map construction rules
				//
				std::string mapName		   = GetStringXMLAttribute( *ptrToChildElement, "name" );
				int rows				   = GetIntXMLAttribute( *ptrToChildElement, "tilesPerRow", 20 );
				int columns				   = GetIntXMLAttribute( *ptrToChildElement, "tilesPerColumn", 20 );			
				const float pixelsPerTile  = GetFloatXMLAttribute( *ptrToChildElement, "PixelsPerTile", 40.0f );
				std::string generationMode = GetStringXMLAttribute( *ptrToChildElement, "ItemsGenerationMode", "REGULAR" );
				
				std::map< int, std::shared_ptr< TileType >> cumulativeTiles;				

				// Get the tile types
				pugi::xpath_variable_set vars;
				vars.add("name", pugi::xpath_type_string);

				pugi::xpath_query queryTileTypes("/maps/map[@name= string($name)]/tileType", &vars);

				vars.set("name", mapName.c_str());
				pugi::xpath_node_set tileTypes = queryTileTypes.evaluate_node_set( mapsDocument );
				
				DEBUG( "Amount of tile types " << tileTypes.size() );
				for( auto tileTypeElementFromQuery = tileTypes.begin(); tileTypeElementFromQuery != tileTypes.end(); ++tileTypeElementFromQuery )
				{
					XMLNode tyleNode = tileTypeElementFromQuery->node();

					// Loading Tile Types
					bool tileTypeElementIsValid = ValidateXMLAttributes( tyleNode, "name" );
					if( !tileTypeElementIsValid )
					{					
						ReportError( "Skipping tileType element. The name attribute is required" );
						continue;
					}

					std::string name = GetStringXMLAttribute( tyleNode, "name" );
					int weight		 = GetIntXMLAttribute( tyleNode, "weight", 1 );

					auto currentTileType = s_instance->GetTileType( name );
					if(  currentTileType )
					{						
						cumulativeTiles[ weight ] = currentTileType;
					}
				}

				// After gathering the map creation rules let's build it!
				std::shared_ptr< Map > newMap;
				newMap.reset( new Map( mapName ) );

				// 
				if( StringExtensions::CompareStrings( generationMode, "REGULAR" ))
				{
					newMap->SetGenerationMode( REGULAR );
				}
				else if( StringExtensions::CompareStrings( generationMode, "RANDOM" ))
				{
					newMap->SetGenerationMode( RANDOM );
				}
				else
				{
					newMap->SetGenerationMode( RANDOM_WEIGHTED );
				}
				
				// Create the map
				newMap->Create( rows, columns, pixelsPerTile,( 1.0f ), cumulativeTiles );

				/*if( newMap->GenerationMode() == REGULAR )
				{*/
					// Let's get the map content!
					// ITEMS
					//
					pugi::xpath_query queryItems("/maps/map[@name= string($name)]/item", &vars);

					vars.set("name", mapName.c_str());
					pugi::xpath_node_set items = queryItems.evaluate_node_set( mapsDocument );

					DEBUG("The map " << mapName << " contains " << items.size() << " items.");

					for( auto itemElementFromQuery = items.begin(); itemElementFromQuery != items.end(); ++itemElementFromQuery )
					{
						XMLNode itemNode = itemElementFromQuery->node();

						DumpItems( itemNode, newMap );
					}

					// AGENTS
					//
					pugi::xpath_query queryAgents("/maps/map[@name= string($name)]/agent", &vars);

					vars.set("name", mapName.c_str());
					pugi::xpath_node_set agents = queryAgents.evaluate_node_set( mapsDocument );

					DEBUG("The map " << mapName << " contains " << agents.size() << " agents.");

					for( auto agentElementFromQuery = agents.begin(); agentElementFromQuery != agents.end(); ++agentElementFromQuery )
					{
						XMLNode agentNode = agentElementFromQuery->node();

						DumpAgents( agentNode, newMap, true );
					}

					// ENTITIES
					//
					pugi::xpath_query queryEntities("/maps/map[@name= string($name)]/entity", &vars);

					vars.set("name", mapName.c_str());
					pugi::xpath_node_set entities = queryEntities.evaluate_node_set( mapsDocument );

					DEBUG("The map " << mapName << " contains " << entities.size() << " entities.");
					for( auto entityElementFromQuery = entities.begin(); entityElementFromQuery != entities.end(); ++entityElementFromQuery )
					{
						XMLNode entityNode = entityElementFromQuery->node();

						DumpEntities( entityNode, newMap );
					}
				//}
				if( newMap->GenerationMode() == RANDOM_WEIGHTED )
				{
					std::map< int, std::shared_ptr< BluePrintBase >> cumulativeEntities;
					LoadValidEntities();

					pugi::xpath_query queryElements("/maps/map[@name= string($name)]/*", &vars);

					vars.set("name", mapName.c_str());
					pugi::xpath_node_set childs = queryElements.evaluate_node_set( mapsDocument );

					DEBUG( "Amount of map child elements " << childs.size() );
					for( auto elementFromQuery = childs.begin(); elementFromQuery != childs.end(); ++elementFromQuery )
					{
						XMLNode childNode = elementFromQuery->node();

						// Loading Tile Types
						bool tileTypeElementIsValid = ValidateXMLAttributes( childNode, "name" );
						if( !tileTypeElementIsValid )
						{					
							ReportError( "Skipping tileType element. The name attribute is required" );
							continue;
						}

						std::string nodeName = childNode.name();

						std::string name = GetStringXMLAttribute( childNode, "name" );
						int weight		 = GetIntXMLAttribute( childNode, "weight", 1 );

						auto currentEntity = s_instance->GetEntityBluePrint( name );
						if(  currentEntity )
						{						
							cumulativeEntities[ weight ] = currentEntity;
						}
					}

					// TODO: Add code to generate content from cumulative entities
					int generatedElements = newMap->GenerateWeightedContent( cumulativeEntities, newMap );
					DEBUG( "The map randomly generated " << generatedElements << " entities." );
				}

				// Add the map to the list!
				//
				m_maps[ mapName ] = newMap;
			}
		}
	}

	void MapManager::LoadValidEntities()
	{
		std::string entitiesDefinitionFile( Settings::Instance().GetEntitiesConfigPath() );

		// Load the entities doc
		//		
		pugi::xml_document entitiesDocument;
		pugi::xml_parse_result entitiesDocumentResult = entitiesDocument.load_file( entitiesDefinitionFile.c_str() );

		DEBUG( "Loading " << entitiesDefinitionFile.c_str() );

		if( !entitiesDocumentResult ) // Error Checking.
		{
			std::stringstream errorMessage;
			errorMessage << entitiesDefinitionFile.c_str()
				<< " was not loaded properly. Check the path defined in the app.xml file and try again.";

			ReportError( errorMessage.str() );
			return;				
		}

		// Load the root node of the file
		//
		XMLNode rootNode = entitiesDocument.child( "entities" );
		if( !rootNode ) // Error Checking.
		{
			std::stringstream errorMessage;
			errorMessage << "The file "
				<< entitiesDefinitionFile.c_str()
				<< " does not contain the root node \"entities\". Fix the structure and try again.";

			ReportError( errorMessage.str() );
			return;
		}

		DEBUG( "Root node for " << entitiesDefinitionFile.c_str() << " loaded properly." );

		for( auto ptrToChildElement = rootNode.begin(); ptrToChildElement != rootNode.end(); ++ptrToChildElement )
		{
			bool entityElementIsValid = ValidateXMLAttributes( *ptrToChildElement, "name" );
			if( !entityElementIsValid )
			{					
				ReportError( "Skipping entity element. The name attribute is required" );
				continue;
			}


			std::string entityName = ptrToChildElement->name();

			// Name attribute
			std::string name = GetStringXMLAttribute( *ptrToChildElement, "name" );

			if( StringExtensions::CompareStrings( entityName, "agent" ))
			{
				// Creating the blue print for the entity
				BluePrint< Agent >* agentBluePrint;
				BluePrintManager::Instance().GetBluePrint( name, agentBluePrint );
				m_ValidEntities.insert( std::make_pair( name, agentBluePrint ));
			}
			else if( StringExtensions::CompareStrings( entityName, "entity" ))
			{
				// Creating the blue print for the entity
				BluePrint< Entity >* entityBluePrint;
				BluePrintManager::Instance().GetBluePrint( name, entityBluePrint );
				m_ValidEntities.insert( std::make_pair( name, entityBluePrint ));
			}
			else if( StringExtensions::CompareStrings( entityName, "item" ))
			{
				// Creating the blue print for the entity
				BluePrint< Item >* itemBluePrint;
				BluePrintManager::Instance().GetBluePrint( name, itemBluePrint );
				m_ValidEntities.insert( std::make_pair( name, itemBluePrint ));
			}		
		}
	}

	void MapManager::LoadValidTileTypes()
	{
		std::string tilesDefinitionFile( Settings::Instance().GetTilesConfigPath() );

		// Load the tiles doc
		//		
		pugi::xml_document tilesDocument;
		pugi::xml_parse_result tilesDocumentResult = tilesDocument.load_file( tilesDefinitionFile.c_str() );

		DEBUG( "Loading " << tilesDefinitionFile.c_str() );

		if( !tilesDocumentResult ) // Error Checking.
		{
			std::stringstream errorMessage;
			errorMessage << tilesDefinitionFile.c_str()
				<< " was not loaded properly. Check the path defined in the app.xml file and try again.";

			ReportError( errorMessage.str() );
			return;				
		}

		// Load the root node of the file
		//
		XMLNode rootNode = tilesDocument.child( "tiles" );
		if( !rootNode ) // Error Checking.
		{
			std::stringstream errorMessage;
			errorMessage << "The file "
				<< tilesDefinitionFile.c_str()
				<< " does not contain the root node \"tiles\". Fix the structure and try again.";

			ReportError( errorMessage.str() );
			return;
		}

		DEBUG( "Root node for " << tilesDefinitionFile.c_str() << " loaded properly." );

		for( auto ptrToChildElement = rootNode.begin(); ptrToChildElement != rootNode.end(); ++ptrToChildElement )
		{
			bool tileTypeElementIsValid = ValidateXMLAttributes( *ptrToChildElement, "name" );
			if( !tileTypeElementIsValid )
			{					
				ReportError( "Skipping tileType element. The name attribute is required" );
				continue;
			}

			// Creating the tile type object
			//
			std::shared_ptr< TileType > currentTileType;
			currentTileType.reset( new TileType );

			// Retrieving the attributes
			currentTileType->Name				 = GetStringXMLAttribute( *ptrToChildElement, "name" );
			currentTileType->MaxHealth			 = GetFloatXMLAttribute( *ptrToChildElement, "maxHealth", 1500.0f );
			currentTileType->IsSolid			 = GetBoolXMLAttribute( *ptrToChildElement, "isSolid", false );
			currentTileType->Color				 = GetRGBAXMLAttribute( *ptrToChildElement, "color", Vector4( 1.0f ));
			currentTileType->DamageSpeed		 = GetIntXMLAttribute( *ptrToChildElement, "damageSpeedInSeconds", 1 );
			currentTileType->DamageRatePerSecond = GetFloatXMLAttribute( *ptrToChildElement, "damageRatePerSecond", 0.0f );
			currentTileType->DoesNotWorkAgainst  = GetStringXMLAttribute( *ptrToChildElement, "doesNotWorkAgainst" );
			currentTileType->DamageBehavior		 = TileType::GetBehaviorFromString( GetStringXMLAttribute( *ptrToChildElement,"damageBehavior", "NONE" ));

			m_ValidTileTypes.insert( std::make_pair( currentTileType->Name, currentTileType ));
		}
	}

	std::shared_ptr< TileType> MapManager::GetTileType( const std::string& tileTypeName )
	{
		// Check if the tile type was previously loaded
		auto ptrToFoundTileType = m_ValidTileTypes.find( tileTypeName );

		if( ptrToFoundTileType != m_ValidTileTypes.end() )
		{
			// Found the tile type
			return ( ptrToFoundTileType->second );
		}
		return nullptr;
	}

	std::shared_ptr< BluePrintBase> MapManager::GetEntityBluePrint( const std::string& entityName )
	{
		// Check if the tile type was previously loaded
		auto ptrToFoundTileType = m_ValidEntities.find( entityName );

		if( ptrToFoundTileType != m_ValidEntities.end() )
		{
			// Found the tile type
			return ( ptrToFoundTileType->second );
		}
		return nullptr;
	}

	bool MapManager::SetMap( const std::string& mapName )
	{
		auto currentMap = GetMap( mapName );
		if( currentMap )
		{
			m_currentMap = currentMap;
			return true;
		}
		return false;
	}

	bool MapManager::SetMap( const char* mapName )
	{
		auto currentMap = GetMap( mapName );
		if( currentMap )
		{
			m_currentMap = currentMap;
			return true;
		}
		return false;
	}

	void MapManager::DumpAgents( const XMLNode& agentNode, const std::shared_ptr< Map >& currentMap, bool scanForItems )
	{
		// Loading Agents
		bool agentElementIsValid = ValidateXMLAttributes( agentNode, "name" );
		if( !agentElementIsValid )
		{					
			ReportError( "Skipping Agent element. The name attribute is required" );
			return;
		}

		// Ignore weighted entities
		//
		if( ValidateXMLAttributes( agentNode, "", "weight" ) )
		{
			return;
		}

		// Name attribute
		std::string name = GetStringXMLAttribute( agentNode, "name" );

		// Creating the blue print for the entity
		BluePrint< Agent > * bp;
		BluePrintManager::Instance().GetBluePrint( name, bp );

		// Creating the agent instance
		Agent* agent = bp->Create();

		// Set map relationship
		agent->m_map = currentMap;

		// CHECK FOR DATA OVERRIDES
		//
		DEBUG("Checking for data overrides for the agent: " << name );
		if( ValidateXMLAttributes( agentNode, "", "position" ) )
		{
			Vector2 position  = GetVector2XMLAttribute( agentNode, "position", Vector2::ZeroVector );
			agent->Translate( position.x, position.y );
			DEBUG("The members m_x & m_y were overridden to the value [" << position.x << ", " << position.y << "]" );
		}									

		if( ValidateXMLAttributes( agentNode, "", "texture" ) )
		{
			std::string texture  = GetStringXMLAttribute( agentNode, "texture", "default_alpha.png" );
			agent->m_texture = TextureManager::Instance().GetTexture( texture.c_str(), 0.1f );
			DEBUG("The member m_texture was overridden to the value " << texture );
		}

		if( ValidateXMLAttributes( agentNode, "", "scale" ) )
		{
			float scale  = GetFloatXMLAttribute( agentNode, "scale", 0.01f );
			agent->SetScale( scale, scale );
			DEBUG("The member m_scale was overridden to the value " << scale );
		}

		if( ValidateXMLAttributes( agentNode, "", "updateAlways" ) )
		{
			bool updateAlways    = GetBoolXMLAttribute( agentNode, "updateAlways", true );
			agent->m_updateAlways = updateAlways;						
			DEBUG("The member m_updateAlways was overridden to the value " << updateAlways );
		}

		if( ValidateXMLAttributes( agentNode, "", "behavior" ) )
		{
			BehaviorType behavior	= AI::GetBehaviorFromString( GetStringXMLAttribute( agentNode, "behavior", "2" ) );
			agent->m_behavior = behavior;
			DEBUG("The member m_behavior was overridden to the value " << AI::GetStringFromBehavior( behavior ));
		}

		if( ValidateXMLAttributes( agentNode, "", "movementSpeed" ) )
		{
			float movementSpeed  = GetFloatXMLAttribute( agentNode, "movementSpeed", 1.0f );
			agent->m_movementSpeed = movementSpeed;
			DEBUG("The member m_movementSpeed was overridden to the value " << movementSpeed );
		}

		if( ValidateXMLAttributes( agentNode, "", "canMove" ) )
		{
			bool canMove  = GetBoolXMLAttribute( agentNode, "canMove", false );
			agent->m_canMove = canMove;
			DEBUG("The member m_movementSpeed was overridden to the value " << canMove );
		}

		if( ValidateXMLAttributes( agentNode, "", "canBeDamaged" ) )
		{
			bool canBeDamaged = GetBoolXMLAttribute( agentNode, "canBeDamaged", false );
			agent->m_canBeDamanged = canBeDamaged;
			DEBUG("The member m_canBeDamaged was overridden to the value " << canBeDamaged );
		}

		if( ValidateXMLAttributes( agentNode, "", "maxHealth" ) )
		{
			float maxHealth = GetFloatXMLAttribute( agentNode, "maxHealth", 1000.0f );
			agent->m_maxHealth = maxHealth;
			DEBUG("The member m_maxHealth was overridden to the value " << agentNode );
		}

		if( ValidateXMLAttributes( agentNode, "", "health" ) )
		{
			float health	  = GetFloatXMLAttribute( agentNode, "health", 100 );
			agent->m_health = health;
			DEBUG("The member m_health was overridden to the value " << health );
		}

		if( ValidateXMLAttributes( agentNode, "", "inventoryLimit") )
		{
			int inventoryItemsLimit = GetIntXMLAttribute( agentNode, "inventoryLimit", -1 );						
			agent->m_inventoryItemsLimit = inventoryItemsLimit;
			DEBUG("The member m_inventoryLimit was overridden to the value " << inventoryItemsLimit );
		}

		if( ValidateXMLAttributes( agentNode, "", "conditionalHealth" ) )
		{
			float conditionalHealth	= GetFloatXMLAttribute( agentNode, "conditionalHealth", agent->m_health );
			agent->m_conditionalHealth = conditionalHealth;
			DEBUG("The member m_conditionalHealth was overridden to the value " << conditionalHealth );
		}		

		if( scanForItems )
		{
			for( auto child = agentNode.begin(); child != agentNode.end(); ++child )
			{
				std::string nodeName = child->name();
				if( StringExtensions::CompareStrings( nodeName, "item" ))
				{
					DumpItems( *child, currentMap, agent );
				}				
			}
		}

		// Adding the agent to the map
		currentMap->AddEntity( agent );

		DEBUG( "The agent " << agent->Name() << " was successfully added to the map " << currentMap->Name() );
	}

	void MapManager::DumpEntities( const XMLNode& entityNode, const std::shared_ptr< Map >& currentMap, bool scanForItems )
	{
		// Loading Items
		bool entityElementIsValid = ValidateXMLAttributes( entityNode, "name" );
		if( !entityElementIsValid )
		{					
			ReportError( "Skipping Entity element. The name attribute is required" );
			return;
		}

		// Ignore weighted entities
		//
		if( ValidateXMLAttributes( entityNode, "", "weight" ) )
		{
			return;
		}

		// Name attribute
		std::string name  = GetStringXMLAttribute( entityNode, "name" );

		// Creating the blue print for the entity
		BluePrint< Entity > * bp;
		BluePrintManager::Instance().GetBluePrint( name, bp);

		// Creating the object instance
		Entity* entity = bp->Create();					

		// Set map relationship
		entity->m_map = currentMap;

		// CHECK FOR DATA OVERRIDES
		//
		DEBUG("Checking for data overrides for the entity: " << name );
		if( ValidateXMLAttributes( entityNode, "", "position" ) )
		{
			Vector2 position  = GetVector2XMLAttribute( entityNode, "position", Vector2::ZeroVector );
			entity->Translate( position.x, position.y );
			DEBUG("The members m_x & m_y were overridden to the value [" << position.x << ", " << position.y << "]" );
		}

		if( ValidateXMLAttributes( entityNode, "", "texture" ) )
		{
			std::string texture  = GetStringXMLAttribute( entityNode, "texture", "entity.png" );
			entity->m_texture = TextureManager::Instance().GetTexture( texture.c_str(), 0.1f );
			DEBUG("The member m_texture was overridden to the value " << texture );
		}

		if( ValidateXMLAttributes( entityNode, "", "scale" ) )
		{
			float scale  = GetFloatXMLAttribute( entityNode, "scale", 0.01f );
			entity->SetScale( scale, scale );						
			DEBUG("The member m_scale was overridden to the value " << scale );
		}

		if( ValidateXMLAttributes( entityNode, "", "WarpTo" ) )
		{
			Vector2 warpTo  = GetVector2XMLAttribute( entityNode, "WarpTo", Vector2::ZeroVector );
			entity->m_warpTo = warpTo;
			DEBUG("The member m_warpTo was overridden to the value [" << warpTo.x << ", " << warpTo.y << "]" );
		}

		if( ValidateXMLAttributes( entityNode, "", "Destination" ) )
		{
			std::string destination  = GetStringXMLAttribute( entityNode, "Destination", "" );
			entity->m_destination = destination;
			DEBUG("The member m_destination was overridden to the value " << destination );
		}

		if( ValidateXMLAttributes( entityNode, "", "canBeDamaged" ) )
		{
			bool canBeDamaged = GetBoolXMLAttribute( entityNode, "canBeDamaged", false );
			entity->m_canBeDamanged = canBeDamaged;
			DEBUG("The member m_canBeDamaged was overridden to the value " << canBeDamaged );
		}

		if( ValidateXMLAttributes( entityNode, "", "blocksPosition" ) )
		{
			bool blocksPosition = GetBoolXMLAttribute( entityNode, "blocksPosition", true );
			entity->m_blocksPosition = blocksPosition;
			DEBUG("The member m_blocksPosition was overridden to the value " << blocksPosition );
		}					

		if( ValidateXMLAttributes( entityNode, "", "entityThatUnblocks" ) )
		{
			std::string entityThatUnblocks = GetStringXMLAttribute( entityNode, "entityThatUnblocks", "" );
			entity->m_entityThatUnblocks = entityThatUnblocks;
			DEBUG("The member m_entityThatUnblocks was overridden to the value " << entityThatUnblocks );
		}					

		if( ValidateXMLAttributes( entityNode, "", "maxHealth" ) )
		{
			float maxHealth = GetFloatXMLAttribute( entityNode, "maxHealth", 1000.0f );
			entity->m_maxHealth = maxHealth;
			DEBUG("The member m_maxHealth was overridden to the value " << maxHealth );
		}

		if( ValidateXMLAttributes( entityNode, "", "health" ) )
		{
			float health	  = GetFloatXMLAttribute( entityNode, "health", 100 );
			entity->m_health = health;
			DEBUG("The member m_health was overridden to the value " << health );
		}

		if( ValidateXMLAttributes( entityNode, "", "inventoryLimit") )
		{
			int inventoryItemsLimit = GetIntXMLAttribute( entityNode, "inventoryLimit", -1 );
			entity->m_inventoryItemsLimit = inventoryItemsLimit;
			DEBUG("The member m_inventoryItemsLimit was overridden to the value " << inventoryItemsLimit );
		}

		if( scanForItems )
		{
			for( auto child = entityNode.begin(); child != entityNode.end(); ++child )
			{
				std::string nodeName = child->name();
				if( StringExtensions::CompareStrings( nodeName, "item" ))
				{
					DumpItems( *child, currentMap, entity );
				}				
			}
		}

		// Adding the entity to the map
		currentMap->AddEntity( entity );

		DEBUG( "The entity " << entity->Name() << " was successfully added to the map " << currentMap->Name() );
	}

	void MapManager::DumpItems( const XMLNode& itemNode, const std::shared_ptr< Map >& currentMap, Entity* entity /*= nullptr */ )
	{
		// Loading Items
		bool itemElementIsValid = ValidateXMLAttributes( itemNode, "name" );
		if( !itemElementIsValid )
		{					
			ReportError( "Skipping Item element. The name attribute is required" );
			return;
		}

		// Ignore weighted entities
		//
		if( ValidateXMLAttributes( itemNode, "", "weight" ) )
		{
			return;
		}

		// Name attribute
		std::string name = GetStringXMLAttribute( itemNode, "name" );

		// Creating the item blue print
		BluePrint< Item > * bp;
		BluePrintManager::Instance().GetBluePrint( name, bp );

		// Creating the instance from the blue print
		Item* item = bp->Create();

		// Set map relationship
		item->m_map = currentMap;

		// CHECK FOR DATA OVERRIDES
		//
		DEBUG("Checking for data overrides for the item: " << name );

		if( ValidateXMLAttributes( itemNode, "", "inventorySlot" ) )
		{
			InventorySlots inventorySlot  = IInventory::GetInventorySlotFromString( GetStringXMLAttribute( itemNode, "inventorySlot", "NO_SLOT" ));
			item->m_inventorySlot = inventorySlot;
			DEBUG("The member m_inventorySlot was overridden to the value " << IInventory::GetStringFromInventorySlot( inventorySlot ));
		}

		if( ValidateXMLAttributes( itemNode, "", "texture" ) )
		{
			std::string texture  = GetStringXMLAttribute( itemNode, "texture", "item.png" );
			item->m_texture = TextureManager::Instance().GetTexture( texture.c_str(), 0.1f );
			DEBUG("The member m_texture was overridden to the value " << texture );
		}							

		if( ValidateXMLAttributes( itemNode, "", "scale" ) )
		{
			float scale  = GetFloatXMLAttribute( itemNode, "scale", 0.01f );
			item->SetScale( scale, scale );
			DEBUG("The member m_scale was overridden to the value " << scale );
		}					

		if( ValidateXMLAttributes( itemNode, "", "position" ) )
		{
			Vector2 position  = GetVector2XMLAttribute( itemNode, "position", Vector2::ZeroVector );
			item->Translate( position.x, position.y );
			DEBUG("The members m_x & m_y were overridden to the value [" << position.x << ", " << position.y << "]" );
		}

		if( ValidateXMLAttributes( itemNode, "", "maxHealth" ) )
		{
			float maxHealth = GetFloatXMLAttribute( itemNode, "maxHealth", 1000.0f );
			item->m_maxHealth = maxHealth;
			DEBUG("The member m_maxHealth was overridden to the value " << maxHealth );
		}

		if( ValidateXMLAttributes( itemNode, "", "singleUse" ) )
		{
			bool singleUse	  = GetBoolXMLAttribute( itemNode, "singleUse", true );
			item->m_singleUse = singleUse;
			DEBUG("The member m_singleUse was overridden to the value " << singleUse );
		}
		
		if( ValidateXMLAttributes( itemNode, "", "health" ) )
		{
			float health	  = GetFloatXMLAttribute( itemNode, "health", 100 );
			item->m_health = health;
			DEBUG("The member m_health was overridden to the value " << health );
		}

		if( ValidateXMLAttributes( itemNode, "", "canBeDamaged" ) )
		{
			bool canBeDamaged = GetBoolXMLAttribute( itemNode, "canBeDamaged", false );
			item->m_canBeDamanged = canBeDamaged;
			DEBUG("The member m_canBeDamaged was overridden to the value " << canBeDamaged );
		}										

		if( ValidateXMLAttributes( itemNode, "", "effectInSeconds" ) )
		{
			int effectInSeconds = GetIntXMLAttribute( itemNode, "effectInSeconds", 0 );			
			item->m_effectInSeconds = effectInSeconds;
			DEBUG("The member m_effectInSeconds was overridden to the value " << effectInSeconds );
		}		

		// Adding the entity to the map
		if( entity )
		{
			entity->AddItem( item );
		}
		else
		{
			currentMap->AddEntity( item );
		}		

		DEBUG( "The item " << item->Name() << " was successfully added to the map " << currentMap->Name() );
	}
}