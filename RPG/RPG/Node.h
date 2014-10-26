#pragma once

#ifndef _NODE_H_
#define _NODE_H_

namespace Solidus
{
	struct Node
	{
		/* =============================================================
		 * CTOR
		 * ============================================================= */

		Node( int id, int x, int y, int gCost = 0, int hCost = 0, Node* parent = 0, int parentDirectionX = 0, int parentDirectionY = 0 );

		/* =============================================================
		 * MEMBERS
		 * ============================================================= */
		
		int	  Id;		
		Node* Parent;		
		int   ParentDirectionX;
		int	  ParentDirectionY;
		
		// Costs
		int	  fCost;
		int	  gCost;
		int	  hCost;

		// Position
		int	  X;		
		int	  Y;
	};
}

#endif