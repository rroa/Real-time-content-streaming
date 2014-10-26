#include "stdafx.h"
#include "Node.h"

namespace Solidus
{
	Node::Node( int id, int x, int y, int gCost, int hCost, Node* parent, int parentDirectionX, int parentDirectionY )
		: Id( id )
		, Parent( parent )
		, ParentDirectionX( parentDirectionX )
		, ParentDirectionY( parentDirectionY )
		, X( x )
		, Y( y )
		, gCost( gCost )
		, hCost( hCost )
		, fCost( gCost + hCost )
	{}
}