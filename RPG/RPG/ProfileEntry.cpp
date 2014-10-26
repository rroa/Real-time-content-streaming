#include "stdafx.h"
#include "ProfileEntry.h"
#include "ProfileZone.h"

#include <memory.h>


#if PROFILER
namespace Solidus {

//-----------------------------------------------------------------------------

	ProfileEntry ProfileEntry::_dummy = {
		/* _last = */ { 0, 0 },
		/* zone = */ NULL,
		/* parent = */ NULL,
		/* nextSibling = */ NULL,
		/* firstChild = */ NULL,
		/* lastChild = */ NULL
	};


//-----------------------------------------------------------------------------

	void ProfileEntry::UpdateTree( float a_damping ) 
	{
		Data.SelfTicks.Current = _last.SelfTicks;
		Data.EntryCount.Current = _last.EntryCount;

		Zone->data.SelfTicks.Current += _last.SelfTicks;
		Zone->data.EntryCount.Current += _last.EntryCount;
		
		Data.ChildTicks.Current = 0;
		_last.SelfTicks = 0;
		_last.EntryCount = 0;

		if ( !Zone->IsUpdating() ) 
		{

			Zone->EnableUpdating();
			if (FirstChild) FirstChild->UpdateTree( a_damping );
			
			Zone->data.ChildTicks.Current += Data.ChildTicks.Current;
			Zone->DisableUpdating();

		} else {
			Zone->data.ChildTicks.Current -= Data.SelfTicks.Current;
			if (FirstChild) FirstChild->UpdateTree(a_damping);
		}

		Data.CalculateAverage( a_damping );

		if ( !IsRoot() ) Parent->Data.ChildTicks.Current += Data.SelfTicks.Current + Data.ChildTicks.Current;
		if ( NextSibling ) NextSibling->UpdateTree( a_damping );
	}


//-----------------------------------------------------------------------------

	const ProfileEntry* ProfileEntry::FindNextInTree(void) const 
	{
		if (FirstChild) 
		{
			return FirstChild;

		} 
		else if (NextSibling) 
		{
			return NextSibling;

		} 
		else 
		{
			ProfileEntry* pParent = Parent;

			while (!pParent->IsRoot()) 
			{
				if ( pParent->NextSibling ) 
					return pParent->NextSibling;
				else 
					pParent = pParent->Parent;
			}

			return NULL;
		}
	}


//-----------------------------------------------------------------------------

	void ProfileEntry::Clear(void) {
		memset(this, 0, sizeof(ProfileEntry));
	}


} // namespace Solidus
#endif
