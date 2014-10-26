#pragma once

#ifndef _NAMEDPROPERTYBASE_H_
#define _NAMEDPROPERTYBASE_H_

namespace Solidus
{
	class NamedPropertyBase
	{
	public:
		virtual NamedPropertyBase* Clone() const = 0;
	};
}

#endif