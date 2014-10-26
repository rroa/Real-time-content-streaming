#pragma once

#ifndef _PROFILEZONE_H_
#define _PROFILEZONE_H_

#include "ProfileData.h"

#if PROFILER
namespace Solidus {


//-----------------------------------------------------------------------------

	struct ProfileZone 
	{
		enum STATE 
		{
			STATE_HIDDEN = 0,
			STATE_INITIALIZED,
			STATE_UPDATING
		};

		ProfileZone* next;

		STATE _state;

		mutable const char* name;

		ProfileData data;

		//

		bool IsInitialized() const { return _state != 0; }

		void Init(ProfileZone* a_prev) 
		{
			_state = STATE_INITIALIZED;

			a_prev->next = this;
		}

		void UnInit() 
		{
			_state = STATE_HIDDEN;
		}

		void PreUpdateChain() 
		{
			data.ClearCurrent();
			if (next) next->PreUpdateChain();
		}

		void UpdateChain( float a_damping ) 
		{
			data.CalculateAverage( a_damping );
			if (next) 
				next->UpdateChain( a_damping );
		}

		bool IsUpdating() const { return _state == STATE_UPDATING; }

		void EnableUpdating() { _state = STATE_UPDATING; }
		void DisableUpdating() { _state = STATE_INITIALIZED; }
	};

}
#endif

#endif
