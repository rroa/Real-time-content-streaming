#pragma once

#ifndef _SUBSCRIBERBASE_H_
#define _SUBSCRIBERBASE_H_

namespace Solidus
{
	class SubscriberBase
	{
	public:
		virtual void CallRegisteredFunction( NamedProperties& eventArgs ) = 0;
	};
}

#endif