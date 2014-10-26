#pragma once

#ifndef _VECTOR3_H_
#define _VECTOR3_H_

#include "Vector2.h"

class Vector3
{
public:
	// CTOR
	//
	Vector3();
	explicit Vector3( float initialValue );
	Vector3( const Vector3& other );
	explicit Vector3( float x, float y, float z = 0.0f );
	explicit Vector3( const Vector2& other );

	// ~DTOR
	//
	~Vector3();	

	// functions
	float	 Normalize();
	void	 SetXYZ( float x, float y, float z );
	float	 SetLength( float newLength );
	void	 SetUnitLengthAndYawPitchRadians( float yawRadians, float pitchRadians );
	void	 SetUnitLengthAndYawPitchDegrees( float yawDegress, float pitchDegrees );			
	void	 SetLengthAndYawPitchRadians( float length, float yawRadians, float pitchRadians );
	void	 SetLengthAndYawPitchDegrees( float length, float yawDegress, float pitchDegress );
	void	 Scale( float scaleUnit );	
	Vector3	 MultiplyComponents( const Vector3& rhs ) const;
	Vector3  ReflectOverNormal( const Vector3& normal ) const;
	Vector3& Reverse();	
	void	 GetXYZ( float& x, float& y, float& z ) const;
	float    CalcLength();
	float    CalcLengthSquared();
	void     CalcYawPitchRadians( float &yaw, float &pitch ) const;
	void     CalcYawPitchDegrees( float &yaw, float &pitch ) const;
	float    DotProduct( const Vector3& rhs );
	float    DotProduct( const Vector3& rhs ) const;	
	Vector3  CrossProduct( const Vector3& rhs );
	void	 Reset();
	Vector3  GetInverse() const;

	// operators
	Vector3& operator=( const Vector3& rhs );
	Vector3& operator+=( const Vector3& rhs );	
	Vector3& operator-=( const Vector3& rhs );
	Vector3& operator+=( const float rhs );	
	Vector3& operator-=( const float rhs );
	Vector3& operator*=( const Vector3& rhs );
	Vector3& operator/=( const Vector3& rhs );
	Vector3 operator+( const Vector3& rhs );
	Vector3 operator+( const float displacement );
	Vector3 operator-( const float displacement );
	Vector3 operator-( const Vector3& rhs );
	Vector3 operator-();
	Vector3 operator*( const Vector3& rhs );
	Vector3 operator/( const Vector3& rhs );
	bool operator==( const Vector3& rhs );
	bool operator!=( const Vector3& rhs );

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

	inline int GetMinorAxis() const
	{
		int axis = ( std::abs( x ) < std::abs( y )) ? 0 : 1;
		return ( std::abs( *( &x + axis ) ) < std::abs( z ) ) ? axis : 2;
	}

	inline int GetMajorAxis() const
	{
		int axis = ( std::abs( x ) > std::abs( y )) ? 0 : 1;
		return ( std::abs( *( &x + axis ) ) > std::abs( z ) ) ? axis : 2;
	}

	inline bool IsZero( float epsilon ) const
	{
		return std::abs( x ) <= epsilon && std::abs( y ) <= epsilon && std::abs( z ) <= epsilon;
	}
	
	inline void SnapToMajorAxis()
	{
		(&x)[ GetMinorAxis() ] = 0;
	}

	inline void SnapToMinorAxis()
	{
		(&x)[ GetMajorAxis() ] = 0;
	}

	// STATICS
	//
	static const Vector3 ZeroVector;
	static Vector3 CrossProduct( const Vector3& lhs, const Vector3& rhs );

	// FRIENDLIES
	//
	friend Vector3 operator*( float scaleUnit, const Vector3& rhs );
	friend Vector3 operator*( const Vector3& lhs, float scaleUnit );
	friend Vector3 operator+( float translateUnit, const Vector3& rhs );
	friend Vector3 operator+( const Vector3& lhs, float translateUnit );
	friend Vector3 operator/( const Vector3& lhs, float scaleUnit );
	
	friend Vector3 operator*( const Vector3& lhs, const Vector3& rhs );
	friend Vector3 operator+( const Vector3& lhs, const Vector3& rhs );
	friend Vector3 operator-( const Vector3& lhs, const Vector3& rhs );
	friend Vector3 operator/( const Vector3& lhs, const Vector3& rhs );

	// DATA
	//
	float x;
	float y;
	float z;
	float length;
};


#endif