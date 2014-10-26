#pragma once

#ifndef _ASTAR_H_
#define _ASTAR_H_

#include <map>
#include <vector>

#include "Node.h"
#include "NodeType.h"
#include "Vector2.h"

namespace Solidus
{
	enum AstarMode { MANHATTAN, DIAGONAL };

	class Entity;
	class Map;
	struct Node;
	class AStar
	{
	public:
		/* =============================================================
		* FUNCTIONS
		* ============================================================= */

		static void					 Create();
		static void					 Destroy();		
		static AStar&				 Instance();
		const std::vector< Vector2 > FindPath( Map* map, const Vector2& searchStart, const Vector2& searchGoal, const Vector2& heroLocation = Vector2::ZeroVector );
		const std::vector< Vector2 > FindPath( Map* map, Entity* searchStart, Entity* searchGoal, const Vector2& heroLocation = Vector2::ZeroVector  );

		/* =============================================================
		* INLINE
		* ============================================================= */
		inline const AstarMode GetTraversalMode() const { return m_mode; }
		inline void			   SetTraversalMode( const AstarMode& mode ) { m_mode = mode; }
	private:
		/* =============================================================
		* CTOR
		* ============================================================= */

		AStar();
		AStar( AStar const& ) {};
		AStar& operator=( AStar const& ) {};

		/* =============================================================
		* FUNCTIONS
		* ============================================================= */
		
		void					 SearchStep(); // Search is performed one step at a time
		void					 BuildFinalPath();
		int						 Cost( const int x, const int y ) const;
		void					 EvaluateNode( const int x, const int y, const int movementCost, int parentDirectionX, int parentDirectionY );		
		void					 InsertNode( NodeType::Node nodeType, int x, int y );		
		void					 RemoveNode( int x, int y );
		void					 Reset();

		/* =============================================================
		* MEMBERS
		* ============================================================= */
		
		AstarMode							  m_mode;
		int									  m_columns; 
		int									  m_rows;
		int									  m_state;
		int									  m_totalNodes;
		Node*								  m_start;
		Node*								  m_goal;		
		Node*								  m_currentNode;				
		std::vector< Node* >				  m_nodes;
		std::vector< int >					  m_openNodes;		
		std::vector< int >					  m_closedNodes;		
		std::vector< int >					  m_blockedNodes;		
		std::vector< Vector2 >				  m_finalPath;				
		std::map<std::pair< int, int >, int > m_locationToIDMap; // Maps grid locations to node index

		/* =============================================================
		* STATICS
		* ============================================================= */

		static AStar*						  s_instance;		
		static const int					  s_orthogonalCost = 10;
		static const int					  s_diagonalCost   = 14;

		/* =============================================================
		* HELPERS
		* ============================================================= */
		
		struct SearchState
		{
			enum State
			{				
				PlaceNode		= 1,
				StartPlaced		= 2,
				GoalPlaced		= 4,				
				SearchStarted	= 8,
				SearchDone		= 16,
				GoalFound		= 32
			};
		};

		struct ByTotalNodeCost
		{
			ByTotalNodeCost() {};
			ByTotalNodeCost(const AStar* instance) : instance( instance ){}
			const AStar* instance;
			inline bool operator()(const int& lhs, const int& rhs) const
			{
				return instance->m_nodes[ lhs ]->fCost < instance->m_nodes[ rhs ]->fCost;
			}
		} comparator;
	};
}

#endif