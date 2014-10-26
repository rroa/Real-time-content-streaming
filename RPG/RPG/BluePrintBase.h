#pragma once

#ifndef _BLUEPRINT_BASE_H_
#define _BLUEPRINT_BASE_H_

#include "XMLExtensions.h"

namespace Solidus
{
	class BluePrintBase
	{
	public:		
		/* =============================================================
		* PURE VIRTUALS
		* ============================================================= */
		virtual BluePrintBase* Clone() const = 0;
	};
}

#endif