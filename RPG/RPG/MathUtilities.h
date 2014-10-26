#pragma once

#ifndef _MATHUTILITIES_H_
#define _MATHUTILITIES_H_

#define PI 3.14159265359f

#define RAD2DEG(a) ( a * ( 180 / PI ) )
#define DEG2RAD(a) ( a * ( PI / 180 ) )

#include <cstdlib>
#include "Filters.h"

typedef float (*FilterFuncPtr)( float value );

const float TWO_PI = 6.28318530718f;

// Rounds to the nearest int to a float
int RoundUnbiasedToInt( float n );

// Rounds to the nearest float to a float
float RoundUnbiasedToFloat( float n );

// Returns the maximum value in a pair
template <typename T>
T GetMaximum ( T a, T b ) {
	T max;
	max = (a > b) ? a : b;
	return max;
}

template <typename T>
T GetMaximum ( T a, T b, T c ) {
	T max = a; // assume a is the largest

	if(b > max)	
		max = b; // if b is larger then assign b to max
	
	if(c > max)
		max = c; // if c is larger then assign c to max

	return max;
}

template <typename T>
T GetMaximum ( T a, T b, T c, T d ) {	
	return GetMaximum( a, GetMaximum( b, GetMaximum( c, d ) ) );
}

// Returns the minimum value in a pair
template <typename T>
T GetMinimum ( T a, T b ) {
	T min;
	min = (a < b) ? a : b;
	return min;
}

template <typename T>
T GetMinimum ( T a, T b, T c ) {
	T min = a; // assume a is the largest

	if(b < min)	
		min = b; // if b is larger then assign b to min

	if(c < min)
		min = c; // if c is larger then assign c to min

	return min;
}

template <typename T>
T GetMinimum ( T a, T b, T c, T d ) {	
	return GetMinimum( a, GetMinimum( b, GetMinimum( c, d ) ) );
}

template <typename T>
T Clamp( T lowerBound, T upperBound, T newBound )
{
	if (newBound > upperBound)
		newBound = upperBound;
	else if (newBound < lowerBound)
		newBound = lowerBound;
	return newBound;
}

template< typename real_t, typename alpha_t >
real_t Lerp( real_t a, real_t b, alpha_t alpha)
{
	return static_cast< real_t >( a + (( b - a ) * alpha ));
}

template< typename T >
T Sign( T x )
{
	return x < 0 ? T( -1 ) : ( x > 0 ? T( 1 ) : T( 0 ));
}

template< typename T >
T SignNoZero( T x )
{
	return x < 0 ? T( -1 ) : T( 1 );
}

// Converts a value given in degress to radians
float ConvertToRadians( float degress );

// Converts a value given in radians to degress
float ConvertToDegrees( float radians );

// Calculates the angular distance between two angles
float CalculateAngularDistanceInDegrees( float startingAngle, float endingAngle );

// Calculates the angular distance between two radians
float CalculateAngularDistanceInRadians( float startingAngle, float endingAngle );

// Calculates the angular displacement between two angles
float CalculateAngularDisplacementInDegrees( float startingAngle, float endingAngle );

// Calculates the angular displacement between to radians
float CalculateAngularDisplacementInRadians( float startingAngle, float endingAngle );

// Returns true if an integer if a power of two, false if its not
bool IsPowerOf2( int n );

//Rounds n up to the next multiple of m, even is n already is a multiple of m
int RoundUpToMultiple(int n, int m);

//Rounds n down to the next multiple of m, even is n already is a multiple of m
int RoundDownToMultiple(int n, int m); 


template <typename T>
float Interpolate(T a, T b, float fraction)
{	
	return (a * ( 1- fraction ) + b * fraction);
}

float RangeMapFloat(float currentValue, float currentRangeStart, float currentRangeEnd, float outputRangeStart, float outputRangeEnd, FilterFuncPtr func = nullptr, bool clamp = false );

#endif