#include "stdafx.h"
#include "Vector2.h"
#include "MathUtilities.h"
#include <cmath>

const Vector2 Vector2::ZeroVector( 0.0f, 0.0f );

// constructors & destructor
Vector2::Vector2()
	: x( 0.0f )
	, y( 0.0f )
	, length( 1.0f )
{}

Vector2::Vector2( int x, int y )
	: x( static_cast< float >( x ) )
	, y( static_cast< float >( y ) )
{}

Vector2::Vector2( float x, float y )
{
	this->x = x;
	this->y = y;
}

Vector2::Vector2( const Vector2& other )
	: x( other.x )
	, y( other.y )
	, length( other.length )
{

}

Vector2::~Vector2()
{

}

// functions
float Vector2::Normalize()
{
	// Calculating the length
	CalcLength();

	float inverseScale = 1.0f / length;
	x *= inverseScale;
	y *= inverseScale;

	return length;
}

void Vector2::SetXY( float x, float y )
{
	this->x = x;
	this->y = y;	
}

void Vector2::SetUnitLengthAndYawRadians( float radians )
{
	length = 1;
	x = cosf( radians );
	y = sinf( radians );
}

void Vector2::SetUnitLengthAndYawDegrees( float degress )
{
	float radians = ConvertToRadians( degress );
	SetUnitLengthAndYawRadians( radians );
}

void Vector2::SetLengthAndYawRadians( float length, float radians )
{
	this->length = length;
	x = length * cosf( radians );
	y = length * sinf( radians );
}

void Vector2::SetLengthAndYawDegrees( float length, float degress )
{
	float radians = ConvertToRadians( degress );
	SetLengthAndYawRadians( length, radians );
}

void Vector2::Scale( float scaleUnit )
{
	x *= scaleUnit;
	y *= scaleUnit;
}

float Vector2::SetLength( float newLength )
{
	float oldLen = length;
	float lenSquared = x * x + y * y;
	if(lenSquared == 0) return 0.0f;

	// Calculating new length
	length = sqrtf( lenSquared );

	float inverseScale = newLength / length;
	x *= inverseScale;
	y *= inverseScale;
	
	return oldLen;
}

Vector2 Vector2::MultiplyComponents( const Vector2& rhs )
{
	Vector2 nonUniform;
	nonUniform.x = x * rhs.x;
	nonUniform.y = y * rhs.y;

	return nonUniform;
}

Vector2& Vector2::RotateDegrees( float degrees )
{
	float radians = ConvertToRadians( degrees );

	return RotateRadians( radians );
}

Vector2& Vector2::RotateRadians( float radians )
{
	float sn = sinf( radians );
	float cs = cosf( radians );

	x = ( x * cs ) - ( y * sn );
	y = ( x * sn ) - ( y * cs );
	
	return *this;
}

Vector2& Vector2::Rotate90Degrees()
{
	return RotateDegrees( 90.0f );
}

Vector2& Vector2::RotateMinus90Degrees()
{
	return RotateDegrees( -90.0f );
}

Vector2& Vector2::Reverse()
{
	float temp = x;
	x = y;
	y = temp;

	return *this;
}

void Vector2::GetXY( float& x, float& y )
{
	x = this->x;
	y = this->y;
}

float Vector2::CalcLength()
{
	float lenSquared = x * x + y * y;
	if(lenSquared == 0) 
	{
		length = 0.0f;
		return length;
	}
	
	length = sqrtf( lenSquared );

	return length;
}

float Vector2::CalcLengthSquared()
{
	float lenSquared = x * x + y * y;
	if(lenSquared == 0) return 0.0f;

	return lenSquared;
}

float Vector2::CalcYawRadians()
{
	return atan2( y, x );
}

float Vector2::CalcYawDegrees()
{
	float yawRadians = atan2( y, x );
	return ConvertToDegrees( yawRadians );
}

float Vector2::DotProduct( const Vector2& rhs )
{
	return ( x * rhs.x ) + ( y * rhs.y );
}

// operators
Vector2& Vector2::operator=( const Vector2& rhs )
{
	// Prevent self assignment. Two objects
	// are equal if their memory address are equal.
	if( this == &rhs )
	{
		return *this;
	}

	this->x = rhs.x;
	this->y = rhs.y;
	this->length = rhs.length;

	return *this;
}

Vector2& Vector2::operator+=( const Vector2& rhs )
{	
	x = x + rhs.x;
	y = y + rhs.y;

	return *this;
}

Vector2& Vector2::operator-=( const Vector2& rhs )
{
	x = x - rhs.x;
	y = y - rhs.y;

	return *this;
}

Vector2& Vector2::operator*=( const Vector2& rhs )
{
	x = x * rhs.x;
	y = y * rhs.y;

	return *this;
}

Vector2& Vector2::operator/=( const Vector2& rhs )
{
	x = x / rhs.x;
	y = y / rhs.y;

	return *this;
}

Vector2 Vector2::operator+( const Vector2& rhs )
{
	Vector2 sum;

	sum.x = x + rhs.x;
	sum.y = y + rhs.y;

	return sum;
}

Vector2 Vector2::operator-( const Vector2& rhs )
{
	Vector2 sub;
	sub.x = x - rhs.x;
	sub.y = y - rhs.y;

	return sub;
}

Vector2 Vector2::operator-()
{
	return Vector2( -x, -y );
}

Vector2 Vector2::operator*( const Vector2& rhs )
{
	Vector2 scaled;

	scaled.x = x * rhs.x;
	scaled.y = y * rhs.y;

	return scaled;
}

Vector2 Vector2::operator/( const Vector2& rhs )
{
	Vector2 inverseScaled;

	inverseScaled.x = x / rhs.x;
	inverseScaled.y = y / rhs.y;

	return inverseScaled;
}

bool Vector2::operator==( const Vector2& rhs )
{
	return 
		x == rhs.x && 
		y == rhs.y;
}

bool Vector2::operator!=( const Vector2& rhs )
{
	return 
		x != rhs.x || 
		y != rhs.y;
}

void Vector2::Abs()
{
	std::abs( x );
	std::abs( y );
}

float Vector2::AddFields() const
{
	return x + y;
}

// friend functions
Vector2 operator*( float scaleUnit, const Vector2& rhs )
{
	Vector2 scaled;
	scaled.x = scaleUnit * rhs.x;
	scaled.y = scaleUnit * rhs.y;

	return scaled;
}

Vector2 operator*( const Vector2& lhs, float scaleUnit )
{
	Vector2 scaled;
	scaled.x = scaleUnit * lhs.x;
	scaled.y = scaleUnit * lhs.y;
	
	return scaled;
}