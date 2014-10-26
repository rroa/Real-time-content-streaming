#pragma once

#ifndef _NODETYPE_H_
#define _NODETYPE_H_

namespace Solidus
{
	struct NodeType
	{
		enum Node
		{
			StartNode = 1,
			GoalNode  = 2,
			Terrain   = 4
		};
	};
}

#endif