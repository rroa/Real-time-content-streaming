#pragma once

#ifndef _EASING_FILTERS_H_
#define _EASING_FILTERS_H_

inline float Linear( float time )  { return time; }
inline float LinearReversed( float time )  { return 1 - time; }
inline float SmoothStart( float time )  { return time * time; }
inline float SmoothEnd( float time )  { return time * ( 2 - time ); }
inline float SmoothStep( float time )  { return ( time < 0.5f ) ? ( 2 * time * time ) : -1 + ( 4 - 2 * time ) * time; }
inline float SmoothStepReversed( float time )  { return ( time > 0.5f ) ? ( 2 * time * time ) : -1 + ( 4 - 2 * time ) * time; }

#endif