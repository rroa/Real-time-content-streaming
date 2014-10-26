#include "stdafx.h"
#include "Vector3.h"
#include "MathUtilities.h"
#include <cmath>
#include "Vector4.h"

const Vector3 Vector3::ZeroVector( 0.0f, 0.0f, 0.0f );

// constructors & destructor
Vector3::Vector3()
	: x( 0.0f )
	, y( 0.0f )
	, z( 0.0f )
	, length( 1.0f )
{}

Vector3::Vector3( float x, float y, float z )
{
	this->x = x;
	this->y = y;
	this->z = z;
}

Vector3::Vector3( const Vector3& other )
	: x( other.x )
	, y( other.y )
	, z( other.z )
	, length( other.length )
{}

Vector3::Vector3( const Vector2& other )
	: x( other.x )
	, y( other.y )
	, z( 0.f )
	, length( other.length )
{}

Vector3::Vector3( float initialValue )
	: x( initialValue )
	, y( initialValue )
	, z( initialValue )	
{
	CalcLength();
}

Vector3::~Vector3()
{}

// functions
float Vector3::Normalize()
{
	// Calculating the length
	CalcLength();

	if( length <= 0) return 0;
	float inverseScale = 1.0f / length;
	x *= inverseScale;
	y *= inverseScale;
	z *= inverseScale;

	return length;
}

void Vector3::SetXYZ( float x, float y, float z )
{
	this->x = x;
	this->y = y;	
	this->z = z;	
}

float Vector3::SetLength( float newLength )
{
	float oldLen = length;
	float lenSquared = x * x + y * y + z * z;
	if( lenSquared == 0 ) return 0.0f;

	// Calculating new length
	length = sqrtf( lenSquared );

	float inverseScale = newLength / length;
	x *= inverseScale;
	y *= inverseScale;
	z *= inverseScale;

	return oldLen;
}

void Vector3::SetUnitLengthAndYawPitchRadians( float yawRadians, float pitchRadians )
{
	length = 1;

	float csYaw = cosf( yawRadians );
	float csPitch = cosf( pitchRadians );
	float snYaw = sinf( yawRadians );
	float snPitch = sinf( pitchRadians );

	x = csYaw * csPitch;
	y = snYaw * csPitch;
	z = snPitch;
}

void Vector3::SetUnitLengthAndYawPitchDegrees( float yawDegress, float pitchDegrees )
{
	float yawRadians = ConvertToRadians( yawDegress );
	float pitchRadians = ConvertToRadians( pitchDegrees );
	SetUnitLengthAndYawPitchRadians( yawRadians, pitchRadians );
}

void Vector3::SetLengthAndYawPitchRadians( float length, float yawRadians, float pitchRadians )
{
	this->length = length;

	float csYaw = cosf( yawRadians );
	float csPitch = cosf( pitchRadians );
	float snYaw = sinf( yawRadians );
	float snPitch = sinf( pitchRadians );

	x = length * csYaw * csPitch;
	y = length * snYaw * csPitch;
	z = length * snPitch;
}

void Vector3::SetLengthAndYawPitchDegrees( float length, float yawDegress, float pitchDegress )
{
	float yawRadians = ConvertToRadians( yawDegress );
	float pitchRadians = ConvertToRadians( pitchDegress );
	SetLengthAndYawPitchRadians( length, yawRadians, pitchRadians );
}

void Vector3::Scale( float scaleUnit )
{
	x *= scaleUnit;
	y *= scaleUnit;
	z *= scaleUnit;
}

Vector3 Vector3::MultiplyComponents( const Vector3& rhs ) const
{
	Vector3 nonUniform;
	nonUniform.x = x * rhs.x;
	nonUniform.y = y * rhs.y;
	nonUniform.y = z * rhs.z;

	return nonUniform;
}

Vector3 Vector3::ReflectOverNormal( const Vector3& normal ) const
{
	Vector3 reflected = *this;
	reflected -= ( 2 * normal );

	return reflected;
}

Vector3& Vector3::Reverse()
{	
	x = -x;
	y = -y;
	z = -z;

	return *this;
}

void Vector3::GetXYZ( float& x, float& y, float& z ) const
{
	x = this->x;
	y = this->y;
	z = this->z;
}

float Vector3::CalcLength()
{
	float lenSquared = x * x + y * y + z * z;
	if( lenSquared == 0 ) 
	{
		length = 0.0f;
		return length;
	}

	length = sqrtf( lenSquared );

	return length;
}

float Vector3::CalcLengthSquared()
{
	float lenSquared = x * x + y * y + z * z;
	
	return lenSquared;
}

void Vector3::CalcYawPitchRadians( float &yaw, float &pitch ) const
{
	yaw = atan2( y, x );
	pitch = atan2( z, sqrtf( x * x + y * y ) );
}

void Vector3::CalcYawPitchDegrees( float &yaw, float &pitch ) const
{
	float yawRadians = atan2( y, x );
	float pitchRadians = atan2( z, sqrtf( x * x + y * y ) );
	yaw = ConvertToDegrees( yawRadians );
	pitch = ConvertToDegrees( pitchRadians );
}

float Vector3::DotProduct( const Vector3& rhs )
{
	return ( this->x * rhs.x ) + ( this->y * rhs.y ) + ( this->z * rhs.z );
}

float Vector3::DotProduct( const Vector3& rhs ) const
{
	return const_cast< Vector3* >( this )->DotProduct( rhs );
}

Vector3 Vector3::CrossProduct( const Vector3& rhs )
{
	return Vector3(y * rhs.z - z * rhs.y,
		z * rhs.x - x * rhs.z,
		x * rhs.y - y * rhs.x);
}

Vector3 Vector3::CrossProduct( const Vector3& lhs, const Vector3& rhs )
{
	return Vector3(lhs.y * rhs.z - lhs.z * rhs.y,
		lhs.z * rhs.x - lhs.x * rhs.z,
		lhs.x * rhs.y - lhs.y * rhs.x);
}

// operators
Vector3& Vector3::operator=( const Vector3& rhs )
{
	// Prevent self assignment. Two objects
	// are equal if their memory address are equal.
	if( this == &rhs )
	{
		return *this;
	}

	this->x = rhs.x;
	this->y = rhs.y;
	this->z = rhs.z;
	this->length = rhs.length;

	return *this;
}

Vector3& Vector3::operator+=( const Vector3& rhs )
{	
	x = x + rhs.x;
	y = y + rhs.y;
	z = z + rhs.z;

	return *this;
}

Vector3& Vector3::operator-=( const Vector3& rhs )
{
	x = x - rhs.x;
	y = y - rhs.y;
	z = z - rhs.z;

	return *this;
}

Vector3& Vector3::operator+=( const float rhs )
{
	x = x + rhs;
	y = y + rhs;
	z = z + rhs;

	return *this;
}

//Vector3& Vector3::operator+=( const Vector3& rhs ) const
//{
//	return const_cast< Vector3* >( this )->operator+( rhs );
//}

Vector3& Vector3::operator-=( const float rhs )
{
	x = x - rhs;
	y = y - rhs;
	z = z - rhs;

	return *this;
}

Vector3& Vector3::operator*=( const Vector3& rhs )
{
	x = x * rhs.x;
	y = y * rhs.y;
	y = z * rhs.z;

	return *this;
}

Vector3& Vector3::operator/=( const Vector3& rhs )
{
	x = ( rhs.x > 0 ) ? x / rhs.x : 0;
	y = ( rhs.y > 0 ) ? y / rhs.y : 0;
	y = ( rhs.z > 0 ) ? z / rhs.z : 0;

	return *this;
}

Vector3 Vector3::operator+( const Vector3& rhs )
{
	Vector3 sum;

	sum.x = x + rhs.x;
	sum.y = y + rhs.y;
	sum.z = z + rhs.z;

	return sum;
}

Vector3 Vector3::operator+( const float displacement )
{
	Vector3 sum;
	sum.x = x + displacement;
	sum.y = y + displacement;
	sum.z = z + displacement;

	return sum;
}

Vector3 Vector3::operator-( const float displacement )
{
	Vector3 sub;
	sub.x = x - displacement;
	sub.y = y - displacement;
	sub.z = z - displacement;

	return sub;
}

Vector3 Vector3::operator-( const Vector3& rhs )
{
	Vector3 sub;
	sub.x = x - rhs.x;
	sub.y = y - rhs.y;
	sub.z = z - rhs.z;

	return sub;
}

Vector3 Vector3::operator-()
{
	return Vector3( -x, -y, -z );
}

Vector3 Vector3::operator*( const Vector3& rhs )
{
	Vector3 scaled;

	scaled.x = x * rhs.x;
	scaled.y = y * rhs.y;
	scaled.z = z * rhs.z;

	return scaled;
}

Vector3 Vector3::operator/( const Vector3& rhs )
{
	Vector3 inverseScaled;

	inverseScaled.x = x / rhs.x;
	inverseScaled.y = y / rhs.y;
	inverseScaled.z = y / rhs.z;

	return inverseScaled;
}

bool Vector3::operator==( const Vector3& rhs )
{
	return 
		x == rhs.x && 
		y == rhs.y &&
		z == rhs.z;
}

bool Vector3::operator!=( const Vector3& rhs )
{
	return 
		x != rhs.x || 
		y != rhs.y ||
		z != rhs.z;
}

void Vector3::Reset()
{
	*this = ZeroVector;
}

Vector3 Vector3::GetInverse() const
{
	return Vector3( -x, -y, -z );
}

// friend functions
Vector3 operator*( float scaleUnit, const Vector3& rhs )
{
	Vector3 scaled;
	scaled.x = scaleUnit * rhs.x;
	scaled.y = scaleUnit * rhs.y;
	scaled.z = scaleUnit * rhs.z;

	return scaled;
}

Vector3 operator*( const Vector3& lhs, float scaleUnit )
{
	Vector3 scaled;
	scaled.x = scaleUnit * lhs.x;
	scaled.y = scaleUnit * lhs.y;
	scaled.z = scaleUnit * lhs.z;

	return scaled;
}

Vector3 operator+( const Vector3& lhs, float translateUnit )
{
	Vector3 translated;
	translated.x = translateUnit + lhs.x;
	translated.y = translateUnit + lhs.y;
	translated.z = translateUnit + lhs.z;

	return translated;
}

Vector3 operator+( float translateUnit, const Vector3& rhs )
{
	Vector3 translated;
	translated.x = translateUnit + rhs.x;
	translated.y = translateUnit + rhs.y;
	translated.z = translateUnit + rhs.z;

	return translated;
}

Vector3 operator/( const Vector3& lhs, float scaleUnit )
{
	if( scaleUnit == 0 ) return Vector3::ZeroVector;

	Vector3 scaled;
	scaled.x = lhs.x / scaleUnit ;
	scaled.y = lhs.y / scaleUnit ;
	scaled.z = lhs.z / scaleUnit ;

	return scaled;
}

Vector3 operator*( const Vector3& lhs, const Vector3& rhs )
{
	Vector3 multiplied( lhs.x * rhs.x, 
					    lhs.y * rhs.y, 
					    lhs.z * rhs.z );
	return multiplied;
}

Vector3 operator+( const Vector3& lhs, const Vector3& rhs )
{
	Vector3 sum( lhs.x + rhs.x, 
				 lhs.y + rhs.y, 
				 lhs.z + rhs.z );
	return sum;
}

Vector3 operator-( const Vector3& lhs, const Vector3& rhs )
{
	Vector3 substraction( lhs.x - rhs.x, 
					      lhs.y - rhs.y, 
						  lhs.z - rhs.z );
	return substraction;
}

Vector3 operator/( const Vector3& lhs, const Vector3& rhs )
{
	if( rhs.x == 0 || rhs.y == 0 || rhs.z == 0 ) return Vector3::ZeroVector;

	Vector3 division(	lhs.x / rhs.x, 
						lhs.y / rhs.y, 
						lhs.z / rhs.z );
	return division;
}