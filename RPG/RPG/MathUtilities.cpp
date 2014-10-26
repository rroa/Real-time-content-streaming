#include "stdafx.h"
#include "MathUtilities.h"
#include <cmath>

int RoundUnbiasedToInt( float n )
{
	/*
		int retval;

		__asm fld n
		__asm fistp retval

		return retval;
	*/

	float unbiasedRoundedValue = floorf( n + 0.5f );
	
	return static_cast<int>( unbiasedRoundedValue );
}

float RoundUnbiasedToFloat( float n )
{
	return floorf( n + 0.5f );
}

float ConvertToRadians( float degress )
{
	return DEG2RAD( degress );
}

float ConvertToDegrees( float radians )
{
	return RAD2DEG( radians );
}

float CalculateAngularDistanceInDegrees( float startingAngle, float endingAngle )
{
	float angularDistance = fabs( startingAngle - endingAngle );
	float angle = fmodf( angularDistance, 360.0f );
	if(angle > 180)
		angle = 360 - angle;

	return angle;
}

float CalculateAngularDistanceInRadians( float startingAngle, float endingAngle )
{
	float start = RAD2DEG( startingAngle );
	float end = RAD2DEG( endingAngle );

	float angularDistance = fabs( start - end );
	float angle = fmodf( angularDistance, 360.0f );
	if(angle > 180)
		angle = 360 - angle;

	return DEG2RAD( angle );
}

float CalculateAngularDisplacementInDegrees( float startingAngle, float endingAngle )
{
	float start = DEG2RAD( startingAngle );
	float end = DEG2RAD( endingAngle );

	float angularDisplacement = start - end;
		
	return RAD2DEG( angularDisplacement );
}

float CalculateAngularDisplacementInRadians( float startingAngle, float endingAngle )
{
	float angularDisplacement = startingAngle - endingAngle;
	
	return angularDisplacement;
}

bool IsPowerOf2(int n)
{
	return n && !(n & (n - 1));
}

float RangeMapFloat( float currentValue, float currentRangeStart, float currentRangeEnd, float outputRangeStart, float outputRangeEnd, FilterFuncPtr func, bool clamp )
{
	if( currentRangeStart == currentRangeEnd )
	{
		return ( outputRangeStart + outputRangeEnd ) * 0.5f;
	}

	if( currentRangeStart == currentRangeEnd )
	{
		return 0;
	}	

	float fraction = ( currentValue - currentRangeStart ) / ( currentRangeEnd - currentRangeStart );

	if( clamp )
	{
		Clamp< float >( 0.f, 1.f, fraction );
	}

	if( func )
	{
		fraction = func( fraction );
	}

	float newValue = ( outputRangeStart +  fraction ) * ( outputRangeEnd - outputRangeStart );

	return newValue;
}

int RoundUpToMultiple(int n, int m) 
{
	if( n % m )
		return (n + m - 1) / m * m;
	else
		return n + m;
}

int RoundDownToMultiple(int n, int m) 
{
	if( n % m )
		return (n - m + 1) / m * m;
	else 
		return n - m;
}