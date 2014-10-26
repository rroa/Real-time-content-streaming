#include "stdafx.h"
#include "AStar.h"

#include <algorithm>
#include "Map.h"
#include "Entity.h"

namespace Solidus
{
	AStar* AStar::s_instance = nullptr;
	const int PLAYER_RADIUS	 = 3;

	AStar::AStar()
		: m_mode( MANHATTAN )
	{
		comparator = ByTotalNodeCost( this );
	}

	void AStar::Create()
	{
		if( !s_instance )
		{
			s_instance = new AStar;
		}
	}

	void AStar::Destroy()
	{
		if( s_instance )
		{
			// Clearing the map
			s_instance->Reset();

			// Resetting the instance
			delete s_instance;
			s_instance = nullptr;
		}		
	}

	AStar& AStar::Instance()
	{
		if( !s_instance )
		{
			Create();
		}

		return *s_instance;
	}

	void AStar::SearchStep()
	{
		/* We have no nodes in the open list and we haven't found the goal node */
		if( m_openNodes.size() == 0 /*&& ( m_state & SearchState::SearchStarted )*/ || !m_goal )
		{
			//std::cout << "Couldn't find a path to the goal" << std::endl;
			m_state |= SearchState::SearchDone;
			return;
		}

		//if( m_openNodes.size() == )

		m_state |= SearchState::SearchStarted;

		/* Get the node with the lowest cost */
		sort( m_openNodes.begin(), m_openNodes.end(), comparator );
		m_currentNode = m_nodes[ m_openNodes[ 0 ]];

		/* Remove the node ID from the open list */
		m_openNodes.erase( m_openNodes.begin() );

		/* Add the node ID to closed */
		m_closedNodes.push_back( m_currentNode->Id );
		
		/* Check if it was the goal node */
		if(m_currentNode->Id == m_goal->Id)
		{
			//std::cout << "A path was found" << std::endl;
			m_state |= SearchState::SearchDone;
			m_state |= SearchState::GoalFound;
			
			BuildFinalPath();
			return;
		}

		/* There is a square to the left */
		bool left = m_currentNode->X > 0;

		/* There is a square to the right */
		bool right = m_currentNode->X < m_columns - 1;

		/* There is a square above */
		bool above = m_currentNode->Y > 0;

		/* There is a square below */
		bool below = m_currentNode->Y < m_rows - 1;
		
		if( left && above && ( m_mode & DIAGONAL ))
		{
			EvaluateNode( m_currentNode->X - 1, m_currentNode->Y - 1, AStar::s_diagonalCost, 1, 1 );
		}
		if( above )
		{
			EvaluateNode( m_currentNode->X, m_currentNode->Y-1, AStar::s_orthogonalCost, 0, 1 );
		}
		if( right && above && ( m_mode & DIAGONAL ))
		{
			EvaluateNode( m_currentNode->X + 1, m_currentNode->Y - 1, AStar::s_diagonalCost, -1, 1 );
		}
		if( left )
		{
			EvaluateNode( m_currentNode->X - 1, m_currentNode->Y, AStar::s_orthogonalCost, 1, 0 );
		}
		if(right)
		{
			EvaluateNode(m_currentNode->X+1, m_currentNode->Y, AStar::s_orthogonalCost, -1, 0);
		}
		if( left && below && ( m_mode & DIAGONAL ))
		{
			EvaluateNode( m_currentNode->X - 1, m_currentNode->Y + 1, AStar::s_diagonalCost, 1, -1 );
		}
		if( below )
		{
			EvaluateNode( m_currentNode->X, m_currentNode->Y + 1, AStar::s_orthogonalCost, 0, -1 );
		}
		if( right && below && ( m_mode & DIAGONAL ))
		{
			EvaluateNode( m_currentNode->X + 1, m_currentNode->Y + 1, AStar::s_diagonalCost, -1, -1 );
		}
	}

	void AStar::EvaluateNode( const int x, const int y, const int movementCost, int parentDirectionX, int parentDirectionY )
	{
		/* Look for the node it in our grid-location-to-id map */
		std::map<std::pair<int, int>, int>::const_iterator locationToIDMapIt = m_locationToIDMap.find( std::make_pair( x, y ));
		
		/* If we found the node we were looking for */
		if(locationToIDMapIt != m_locationToIDMap.end())
		{
			Node* node = m_nodes[ locationToIDMapIt->second ];

			/* If the node is walkable and its ID is not in the closed list */
			if(node->fCost < std::numeric_limits<int>::max() && std::find(m_closedNodes.begin(), m_closedNodes.end(), node->Id) == m_closedNodes.end())
			{
				/* Search for the node in the open list */
				std::vector<int>::const_iterator openIt = find(m_openNodes.begin(), m_openNodes.end(), node->Id);
				
				/* If it's in the open list already */
				if(openIt != m_openNodes.end())
				{
					/* The cost to go from currentNode to the node being evaluated */
					int newPathCost = m_currentNode->gCost + movementCost;

					/* If moving through currentNode to get to this node is cheaper */
					if(newPathCost < node->gCost)
					{
						/* Update this node */
						node->Parent = m_currentNode;
						node->ParentDirectionX = parentDirectionX;
						node->ParentDirectionY = parentDirectionY;
						node->gCost = newPathCost;
						node->fCost = node->gCost + node->hCost;
					}
				}
				/* We found a node that has been created, but isn't in the closed, un-walkable or open list... It is the goal node */
				else
				{
					m_openNodes.push_back(m_goal->Id);
					m_goal->Parent = m_currentNode;
					m_goal->ParentDirectionX = parentDirectionX;
					m_goal->ParentDirectionY = parentDirectionY;
				}
			}
		}
		/* The node doesn't exist yet, create it */
		else
		{
			Node* newNode = new Node(m_totalNodes, x, y, m_currentNode->gCost + movementCost, Cost(x, y), m_currentNode, parentDirectionX, parentDirectionY);
			++ m_totalNodes;
			m_nodes.push_back(newNode);
			m_locationToIDMap.insert(std::make_pair(std::make_pair(newNode->X, newNode->Y), newNode->Id));
			m_openNodes.push_back(newNode->Id);
		}
	}

	int AStar::Cost( const int x, const int y ) const
	{
		int xDistance = abs( x - m_goal->X );
		int yDistance = abs( y - m_goal->Y );

		if( xDistance > yDistance )
		{
			return ( AStar::s_diagonalCost * yDistance ) + ( AStar::s_orthogonalCost * ( xDistance-yDistance ));
		}
		else
		{
			return ( AStar::s_diagonalCost * xDistance ) + ( AStar::s_orthogonalCost * ( yDistance-xDistance ));
		}
	}

	void AStar::InsertNode( NodeType::Node nodeType, int x, int y )
	{
		if(m_state & SearchState::SearchStarted)
		{
			//std::cout << "Can't place a node when the search has started" << std::endl;
			return;
		}

		/* Look for the node it in our grid-location-to-id map */
		std::map<std::pair<int, int>, int>::iterator locationToIDMapIt = m_locationToIDMap.find( std::make_pair( x, y ));
		if(locationToIDMapIt != m_locationToIDMap.end())
		{
			/* There is already a node there */
			return;
		}

		if(nodeType == NodeType::StartNode && m_start != 0)
		{
			locationToIDMapIt = m_locationToIDMap.find(std::make_pair(m_start->X, m_start->Y));
			m_locationToIDMap.erase(locationToIDMapIt);
			m_start->X = x;
			m_start->Y = y;
			m_locationToIDMap.insert(std::make_pair(std::make_pair(m_start->X, m_start->Y), m_start->Id));
			return;
		}		
		else if(nodeType == NodeType::GoalNode && m_goal != 0)
		{
			locationToIDMapIt = m_locationToIDMap.find( std::make_pair( m_goal->X, m_goal->Y ));
			m_locationToIDMap.erase( locationToIDMapIt );
			m_goal->X = x;
			m_goal->Y = y;
			m_locationToIDMap.insert( std::make_pair( std::make_pair( m_goal->X, m_goal->Y ), m_goal->Id ));
			return;
		}
		else
		{
			Node* newNode = new Node( m_totalNodes, x, y );
			++m_totalNodes;

			if( nodeType == NodeType::Terrain )
			{
				newNode->fCost = std::numeric_limits<int>::max();
				newNode->gCost = std::numeric_limits<int>::max();
				newNode->hCost = std::numeric_limits<int>::max();
				m_blockedNodes.push_back( newNode->Id );
			}

			m_nodes.push_back( newNode );
			m_locationToIDMap.insert( std::make_pair( std::make_pair( newNode->X, newNode->Y ), newNode->Id ));

			if( nodeType == NodeType::StartNode )
			{
				m_start = newNode;
				m_openNodes.push_back( newNode->Id );
				m_state |= SearchState::StartPlaced;				
			}
			else if( nodeType == NodeType::GoalNode )
			{
				m_goal = newNode;
				m_state |= SearchState::GoalPlaced;				
			}
		}
	}

	void AStar::RemoveNode( int x, int y )
	{
		if( m_state & SearchState::SearchStarted )
		{			
			return;
		}

		std::map<std::pair<int, int>, int>::iterator locationToIDMapIt = m_locationToIDMap.find(std::make_pair( x, y ));

		if(locationToIDMapIt == m_locationToIDMap.end())
		{			
			return;
		}

		/* This is the node that will be removed */
		Node* remove = m_nodes[(*locationToIDMapIt).second];
		m_locationToIDMap.erase(locationToIDMapIt);
		--m_totalNodes;

		if( remove == m_start )
		{
			m_start = 0;
			m_openNodes.clear();
			m_state &= ~SearchState::StartPlaced;
		}
		else if( remove == m_goal )
		{
			m_goal = 0;
			m_state &= ~SearchState::GoalPlaced;
		}		
		else
		{
			m_blockedNodes.erase( find( m_blockedNodes.begin(), m_blockedNodes.end(), remove->Id ));
		}

		/* Since we are going to remove a node from nodes, we need to adjust all IDs after the removed element so that indexing won't be broken */
		std::vector<Node*>::iterator removedIt = m_nodes.erase(m_nodes.begin() + remove->Id);
		for(; removedIt != m_nodes.end(); ++removedIt)
		{
			Node* update = *removedIt;
			int newID = update->Id - 1;
			/*
			* In case the ID is present in the terrain list, we need to update it there.
			* If we didn't do this an ID in the terrain list could point to where the terrain node _used to be_ in the node list!
			*/
			std::vector<int>::iterator terrainIt = find( m_blockedNodes.begin(), m_blockedNodes.end(), update->Id );
			if(terrainIt != m_blockedNodes.end())
			{
				(*terrainIt) = newID;
			}

			/* The above is always true for the location-to-id-map, so we remove the old ID value so that we can update it */
			std::map<std::pair<int,int>, int>::iterator locationIt = m_locationToIDMap.find( std::make_pair( update->X, update->Y ));
			m_locationToIDMap.erase( locationIt );
			m_locationToIDMap.insert(std::make_pair( std::make_pair( update->X, update->Y ), newID ));

			/* If it's the start node we've got to update the open list too */
			if( update == m_start )
			{
				m_openNodes[ 0 ] = newID;
			}

			update->Id = newID;
		}

		/* Finally free the memory allocated for the removed node */
		delete remove;
	}

	void AStar::BuildFinalPath()
	{		
		assert(m_state & SearchState::SearchDone && "A path cannot be built when the search isn't finished");
		Node* node = m_goal;
		while( node->Parent != 0 )
		{
			int x = node->X;
			int y = node->Y;
			m_finalPath.push_back( Vector2( x, y ));			
			node = node->Parent;
		}

		std::reverse( m_finalPath.begin(), m_finalPath.end() );
		m_finalPath.push_back( Vector2( m_goal->X, m_goal->Y ));
	}

	void AStar::Reset()
	{
		// Deleting the nodes!
		for(std::vector<Node*>::const_iterator it = m_nodes.begin(); it != m_nodes.end(); ++it)
		{
			delete (*it);
		}
		
		// Resetting the data members
		m_totalNodes  = 0;
		m_currentNode = 0;
		m_goal		  = 0;
		m_start		  = 0;
		m_state		  = 0;

		// Clearing the containers
		m_nodes.clear();
		m_openNodes.clear();
		m_closedNodes.clear();
		m_blockedNodes.clear();
		m_finalPath.clear();
		m_locationToIDMap.clear();		
	}

	const std::vector< Vector2 > AStar::FindPath( Map* map, const Vector2& searchStart, const Vector2& searchGoal, const Vector2& heroLocation )
	{
		Reset();

		m_columns = map->Width();
		m_rows    = map->Height();

		for( int x = 0; x < m_columns; ++x )
		{
			// Swap the elements of the row.
			//
			for( int y = 0; y < m_rows; ++y )
			{
				auto currentTile = map->GetTileAtLocation( x, y );
				if( currentTile->IsSolid() )
				{
					InsertNode( NodeType::Terrain, x, y );
				}				
			}
		}

		// BLOCK PLAYER RADIUS
		if( heroLocation.x != 0 && heroLocation.y != 0 )
		{	
			int intXCoord = static_cast< int >( heroLocation.x );
			int intYCoord = static_cast< int >( heroLocation.y );

			InsertNode( NodeType::Terrain, intXCoord, intYCoord );			
			
			// Corners
			/*if( map->IsInBounds( intXCoord + 1, intYCoord + 1))
			{
				if( !map->GetTileAtLocation( intXCoord + 1, intYCoord + 1 )->IsSolid() )
				{
					InsertNode( NodeType::Terrain, heroLocation.x + 1.0f, heroLocation.y + 1.0f );
				}
			}

			if( map->IsInBounds( intXCoord - 1, intYCoord + 1))
			{
				if( !map->GetTileAtLocation( intXCoord - 1, intYCoord + 1 )->IsSolid() )
				{
					InsertNode( NodeType::Terrain, heroLocation.x - 1.0f, heroLocation.y + 1.0f );
				}
			}

			if( map->IsInBounds( intXCoord + 1, intYCoord - 1))
			{
				if( !map->GetTileAtLocation( intXCoord + 1, intYCoord - 1 )->IsSolid() )
				{
					InsertNode( NodeType::Terrain, heroLocation.x + 1.0f, heroLocation.y - 1.0f );
				}
			}

			if( map->IsInBounds( intXCoord - 1, intYCoord - 1))
			{
				if( !map->GetTileAtLocation( intXCoord - 1, intYCoord - 1 )->IsSolid() )
				{
					InsertNode( NodeType::Terrain, heroLocation.x - 1.0f, heroLocation.y - 1.0f );
				}
			}*/

			for( unsigned int offset = 1; offset <= PLAYER_RADIUS; ++offset )
			{
				if( map->IsInBounds( intXCoord + 1, intYCoord ))
				{
					if( !map->GetTileAtLocation( intXCoord + 1, intYCoord )->IsSolid() )
					{
						InsertNode( NodeType::Terrain, intXCoord + 1, intYCoord );
					}
				}

				if( map->IsInBounds( intXCoord - 1, intYCoord ))
				{
					if( !map->GetTileAtLocation( intXCoord - 1, intYCoord )->IsSolid() )
					{
						InsertNode( NodeType::Terrain, intXCoord - 1, intYCoord );
					}
				}

				if( map->IsInBounds( intXCoord, intYCoord + 1 ))
				{
					if( !map->GetTileAtLocation( intXCoord, intYCoord + 1 )->IsSolid() )
					{
						InsertNode( NodeType::Terrain, intXCoord, intYCoord + 1 );
					}
				}

				if( map->IsInBounds( intXCoord, intYCoord - 1 ))
				{
					if( !map->GetTileAtLocation( intXCoord, intYCoord - 1 )->IsSolid() )
					{
						InsertNode( NodeType::Terrain, intXCoord, intYCoord - 1 );
					}
				}
			}
		}

		// Insert starting node
		InsertNode( NodeType::StartNode, 
			static_cast< int >( searchStart.x ), static_cast< int >( searchStart.y ));

		// Insert goal node
		InsertNode( NodeType::GoalNode, 
			static_cast< int >( searchGoal.x ), static_cast< int >( searchGoal.y ) );

		// Let's search!
		while( !(m_state & SearchState::SearchDone )) // Do a search step until search is done 
		{
			SearchStep();
		}

		return m_finalPath;
	}

	const std::vector< Vector2 > AStar::FindPath( Map* map, Entity* searchStart, Entity* searchGoal, const Vector2& heroLocation )
	{
		// Retrieving locations from the entities
		Vector2 startigLocation = searchStart->GetLocation();
		Vector2 goalLocation	= searchGoal->GetLocation();
		
		InsertNode( NodeType::Terrain, static_cast< int >( heroLocation.x ), 
			static_cast< int >( heroLocation.y ) );
		
		return FindPath( map, startigLocation, goalLocation );
	}
}