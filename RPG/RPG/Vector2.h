#pragma once

#ifndef VECTOR2_H
#define VECTOR2_H

#include <utility>

class Vector2
{
public: 
	// constructors & destructor
	Vector2();
	Vector2( int x, int y );
	Vector2( float x, float y );
	Vector2( const Vector2& other );
	~Vector2();

	// functions
	void Abs();	
	float AddFields() const;
	float Normalize();
	void SetXY( float x, float y );	
	void SetUnitLengthAndYawRadians( float radians );
	void SetUnitLengthAndYawDegrees( float degress );
	void SetLengthAndYawRadians( float length, float radians );
	void SetLengthAndYawDegrees( float length, float degress );
	void Scale( float scaleUnit );
	float SetLength( float newLength );
	Vector2 MultiplyComponents( const Vector2& rhs );
	Vector2& RotateDegrees( float degrees );
	Vector2& RotateRadians( float radians );
	Vector2& Rotate90Degrees();
	Vector2& RotateMinus90Degrees();
	Vector2& Reverse();
	void GetXY( float& x, float& y );
	float CalcLength();
	float CalcLengthSquared();
	float CalcYawRadians();
	float CalcYawDegrees();

	// operators
	Vector2& operator=( const Vector2& rhs );
	Vector2& operator+=( const Vector2& rhs );
	Vector2& operator-=( const Vector2& rhs );
	Vector2& operator*=( const Vector2& rhs );
	Vector2& operator/=( const Vector2& rhs );
	Vector2 operator+( const Vector2& rhs );
	Vector2 operator-( const Vector2& rhs );
	Vector2 operator-();
	Vector2 operator*( const Vector2& rhs );
	Vector2 operator/( const Vector2& rhs );
	bool operator==( const Vector2& rhs );
	bool operator!=( const Vector2& rhs );
	float DotProduct( const Vector2& rhs );
	friend Vector2 operator*( float scaleUnit, const Vector2& rhs );
	friend Vector2 operator*( const Vector2& lhs, float scaleUnit );

	// Statics
	static const Vector2 ZeroVector;

	// DATA
	float x;
	float y;
	float length;
};

#endif