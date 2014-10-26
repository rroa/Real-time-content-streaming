#include "stdafx.h"
#include "Vector4.h"

const Vector4 Vector4::EmptyColor( 1.0f, 1.0f, 1.0f, 1.0f );

Vector4::Vector4()
	: x ( 0.0f )
	, y ( 0.0f )
	, z ( 0.0f )
	, w ( 1.0f )
{}

Vector4::Vector4( float x_, float y_, float z_, float w_ )
	: x ( x_ )
	, y ( y_ )
	, z ( z_ )
	, w ( w_ )
{}

Vector4::Vector4( const Vector2& vector, bool direction )
	: x ( vector.x )
	, y ( vector.y )
	, z ( 0.0f )
	, w ( ( direction ) ? 0 : 1.0f )
{}

Vector4::Vector4( const Vector3& vector, bool direction )
	: x ( vector.x )
	, y ( vector.y )
	, z ( vector.z )
	, w ( ( direction ) ? 0 : 1.0f )
{}

Vector4::Vector4( float initialValue )
	: x ( initialValue )
	, y ( initialValue )
	, z ( initialValue )
	, w ( initialValue )
{}

float Vector4::DotProduct( const Vector4& rhs )
{
	return ( this->x * rhs.x ) + ( this->y * rhs.y ) + ( this->z * rhs.z ) + ( this->w * rhs.w );
}

float Vector4::DotProduct( const Vector4& lhs, const Vector4& rhs )
{
	return ( lhs.x * rhs.x ) + ( lhs.y * rhs.y ) + ( lhs.z * rhs.z ) + ( lhs.w * rhs.w );
}

bool Vector4::operator==( const Vector4& rhs )
{
	return 
		x == rhs.x && 
		y == rhs.y &&
		z == rhs.z &&
		w == rhs.w;
}

Vector4 Vector4::operator+( const Vector4& rhs )
{
	Vector4 sum;
	sum.x = x + rhs.x;
	sum.y = y + rhs.y;
	sum.z = z + rhs.z;
	sum.w = w + rhs.w;

	return sum;
}

Vector4 Vector4::operator-( const Vector4& rhs )
{
	Vector4 sub;
	sub.x = x - rhs.x;
	sub.y = y - rhs.y;
	sub.z = z - rhs.z;
	sub.w = w - rhs.w;

	return sub;
}

Vector4 Vector4::operator*( const Vector4& rhs )
{
	Vector4 scaled;

	scaled.x = x * rhs.x;
	scaled.y = y * rhs.y;
	scaled.z = z * rhs.z;
	scaled.w = w * rhs.w;

	return scaled;
}

bool Vector4::IsEqual( const Vector4& lhs, const Vector4& rhs )
{
		return lhs.x == rhs.x && 
			   lhs.y == rhs.y &&
			   lhs.z == rhs.z &&
			   lhs.w == rhs.w;
}

// friend functions
Vector4 operator*( float scaleUnit, const Vector4& rhs )
{
	Vector4 scaled;
	scaled.x = scaleUnit * rhs.x;
	scaled.y = scaleUnit * rhs.y;
	scaled.z = scaleUnit * rhs.z;

	return scaled;
}

Vector4 operator*( const Vector4& lhs, float scaleUnit )
{
	Vector4 scaled;
	scaled.x = scaleUnit * lhs.x;
	scaled.y = scaleUnit * lhs.y;
	scaled.z = scaleUnit * lhs.z;

	return scaled;
}