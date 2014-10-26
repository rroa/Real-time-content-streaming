#pragma once

#ifndef _VECTOR4_H_
#define _VECTOR4_H_

#include "Vector2.h"
#include "Vector3.h"

class Vector4
{
public:
	// CTOR
	//
	Vector4();
	explicit Vector4( float initialValue );
	Vector4( float x_, float y_, float z_, float w_ );
	Vector4( const Vector2& vector, bool direction );
	Vector4( const Vector3& vector, bool direction );

	// FUNCTIONS
	//
	float Vector4::DotProduct( const Vector4& rhs );
	static float DotProduct( const Vector4& lhs, const Vector4& rhs );
	static bool  IsEqual( const Vector4& lhs, const Vector4& rhs );

	// OPERATORS
	Vector4 operator+( const Vector4& rhs );
	Vector4 operator-( const Vector4& rhs );
	Vector4 operator*( const Vector4& rhs );
	bool    operator==( const Vector4& rhs );

	// INLINES
	//
	inline float operator [] ( const size_t i ) const
	{
		return *( &x + i );
	}

	inline float& operator [] ( const size_t i )
	{
		return *( &x + i );
	}

	// FRIENDLIES
	//
	friend Vector4 operator*( float scaleUnit, const Vector4& rhs );
	friend Vector4 operator*( const Vector4& lhs, float scaleUnit );

	// STATIC
	static const Vector4 EmptyColor;

	// DATA
	float x;
	float y;
	float z;
	float w;
};

#endif