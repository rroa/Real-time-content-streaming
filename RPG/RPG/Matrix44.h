#pragma once

#ifndef _MATRIX44_
#define _MATRIX44_

#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "MathUtilities.h"

class Matrix44
{
public:
	// CTOR
	//
	Matrix44();
	Matrix44( const float src[16] );
	Matrix44( float ix, float iy, float iz, float iw,
		float jx, float jy, float jz, float jw,
		float kx, float ky, float kz, float kw,
		float tx, float ty, float tz, float tw );
	Matrix44( Vector4 columnI, Vector4 columnJ, Vector4 columnK, Vector4 columnT );
	Matrix44( const Matrix44& rhs );
	Matrix44& operator=( const Matrix44& rhs );
	Matrix44& operator/=( const float& rhs );
	~Matrix44();

	// STATIC FUNCTIONS
	//
	static Matrix44 CreateYawRotationMatrixRadians( const float radians )
	{
		Matrix44 matrix;

		float sn = sinf( radians );
		float cs = cosf( radians );

		matrix.m_matrix[ 0 ] = cs;
		matrix.m_matrix[ 1 ] = sn;
		matrix.m_matrix[ 4 ] = -sn;		
		matrix.m_matrix[ 5 ] =  cs;

		return matrix;
	}

	static Matrix44 CreateYawRotationMatrixDegress( const float degrees )
	{
		Matrix44 matrix;
		float radians = ConvertToRadians( degrees );

		float sn = std::sin( radians );
		float cs = std::cos( radians );

		matrix.m_matrix[ 0 ] = cs;
		matrix.m_matrix[ 1 ] = sn;
		matrix.m_matrix[ 4 ] = -sn;		
		matrix.m_matrix[ 5 ] =  cs;

		return matrix;
	}

	static Matrix44 CreatePitchRotationMatrixRadians( const float radians )
	{
		Matrix44 matrix;

		float sn = sinf( radians );
		float cs = cosf( radians );

		matrix.m_matrix[ 0 ] = cs;
		matrix.m_matrix[ 2 ] = -sn;
		matrix.m_matrix[ 8 ]  = sn;
		matrix.m_matrix[ 10 ] = cs;
		
		return matrix;
	}

	static Matrix44 CreatePitchRotationMatrixDegrees( const float degrees )
	{
		Matrix44 matrix;
		float radians = ConvertToRadians( degrees );

		float sn = sinf( radians );
		float cs = cosf( radians );
		
		matrix.m_matrix[ 0 ] = cs;
		matrix.m_matrix[ 2 ] = -sn;
		matrix.m_matrix[ 8 ]  = sn;
		matrix.m_matrix[ 10 ] = cs;

		return matrix;
	}

	static Matrix44 CreateRollRotationMatrixRadians( const float radians )
	{
		Matrix44 matrix;

		float sn = sinf( radians );
		float cs = cosf( radians );
		
		matrix.m_matrix[ 5 ] = cs;		
		matrix.m_matrix[ 6 ] = sn;
		matrix.m_matrix[ 9 ]  = -sn;
		matrix.m_matrix[ 10 ] = cs;

		return matrix;
	}

	static Matrix44 CreateRollRotationMatrixDegrees( const float degrees )
	{
		Matrix44 matrix;
		float radians = ConvertToRadians( degrees );

		float sn = sinf( radians );
		float cs = cosf( radians );

		matrix.m_matrix[ 5 ] = cs;		
		matrix.m_matrix[ 6 ] = sn;
		matrix.m_matrix[ 9 ]  = -sn;
		matrix.m_matrix[ 10 ] = cs;

		return matrix;
	}

	static Matrix44 CreateUniformScaleMatrix( const float& scale )
	{
		Matrix44 matrix;

		matrix.m_matrix[ 0 ] = scale;
		matrix.m_matrix[ 5 ] = scale;
		matrix.m_matrix[ 10 ] = scale;

		return matrix;
	}

	static Matrix44 CreateNonUniformScaleMatrix( const Vector2& nonUniformScale )
	{
		Matrix44 matrix;

		matrix.m_matrix[ 0 ] = nonUniformScale.x;
		matrix.m_matrix[ 5 ] = nonUniformScale.y;

		return matrix;
	}

	static Matrix44 CreateNonUniformScaleMatrix( const Vector3& nonUniformScale )
	{
		Matrix44 matrix;

		matrix.m_matrix[ 0 ] = nonUniformScale.x;
		matrix.m_matrix[ 5 ] =  nonUniformScale.y;
		matrix.m_matrix[ 10 ] = nonUniformScale.z;	

		return matrix;
	}

	static Matrix44 CreateTranslationMatrix( const Vector2& offset )
	{
		Matrix44 matrix;

		matrix.m_matrix[ 12 ] = offset.x;
		matrix.m_matrix[ 13 ] = offset.y;

		return matrix;
	}

	static Matrix44 CreateTranslationMatrix( const Vector3& offset )
	{
		Matrix44 matrix;

		matrix.m_matrix[ 12 ] = offset.x;
		matrix.m_matrix[ 13 ] = offset.y;
		matrix.m_matrix[ 14 ] = offset.z;

		return matrix;
	}
	
	static Vector4 TransformPoint( const Matrix44& lhs, const Vector4& point )
	{
		Vector4 transformedPoint;

		transformedPoint.x = lhs.m_matrix[0] * point.x + lhs.m_matrix[4] * point.y + lhs.m_matrix[8]  * point.z + lhs.m_matrix[12] * point.w;
		transformedPoint.y = lhs.m_matrix[1] * point.x + lhs.m_matrix[5] * point.y + lhs.m_matrix[9]  * point.z + lhs.m_matrix[13] * point.w;
		transformedPoint.z = lhs.m_matrix[2] * point.x + lhs.m_matrix[6] * point.y + lhs.m_matrix[10] * point.z + lhs.m_matrix[14] * point.w;
		transformedPoint.w = lhs.m_matrix[3] * point.x + lhs.m_matrix[7] * point.y + lhs.m_matrix[11] * point.z + lhs.m_matrix[15] * point.w;

		return transformedPoint;
	}

	static Matrix44 Multiply( const Matrix44& lhs, const Matrix44& rhs )
	{
		Matrix44 matrix;

		// First row
		matrix.m_matrix[0] =  lhs.m_matrix[0] * rhs.m_matrix[0]  + lhs.m_matrix[4] * rhs.m_matrix[1]  + lhs.m_matrix[8] * rhs.m_matrix[2]  + lhs.m_matrix[12] * rhs.m_matrix[3];
		matrix.m_matrix[4] =  lhs.m_matrix[0] * rhs.m_matrix[4]  + lhs.m_matrix[4] * rhs.m_matrix[5]  + lhs.m_matrix[8] * rhs.m_matrix[6]  + lhs.m_matrix[12] * rhs.m_matrix[7];
		matrix.m_matrix[8] =  lhs.m_matrix[0] * rhs.m_matrix[8]  + lhs.m_matrix[4] * rhs.m_matrix[9]  + lhs.m_matrix[8] * rhs.m_matrix[10] + lhs.m_matrix[12] * rhs.m_matrix[11];
		matrix.m_matrix[12] =  lhs.m_matrix[0] * rhs.m_matrix[12] + lhs.m_matrix[4] * rhs.m_matrix[13] + lhs.m_matrix[8] * rhs.m_matrix[14] + lhs.m_matrix[12] * rhs.m_matrix[15];

		// Second row
		matrix.m_matrix[1] =  lhs.m_matrix[1] * rhs.m_matrix[0]  + lhs.m_matrix[5] * rhs.m_matrix[1]  + lhs.m_matrix[9] * rhs.m_matrix[2]  + lhs.m_matrix[13] * rhs.m_matrix[3];
		matrix.m_matrix[5] =  lhs.m_matrix[1] * rhs.m_matrix[4]  + lhs.m_matrix[5] * rhs.m_matrix[5]  + lhs.m_matrix[9] * rhs.m_matrix[6]  + lhs.m_matrix[13] * rhs.m_matrix[7];
		matrix.m_matrix[9] =  lhs.m_matrix[1] * rhs.m_matrix[8]  + lhs.m_matrix[5] * rhs.m_matrix[9]  + lhs.m_matrix[9] * rhs.m_matrix[10] + lhs.m_matrix[13] * rhs.m_matrix[11];
		matrix.m_matrix[13] =  lhs.m_matrix[1] * rhs.m_matrix[12] + lhs.m_matrix[5] * rhs.m_matrix[13] + lhs.m_matrix[9] * rhs.m_matrix[14] + lhs.m_matrix[13] * rhs.m_matrix[15];

		// Third row
		matrix.m_matrix[2] =  lhs.m_matrix[2] * rhs.m_matrix[0]  + lhs.m_matrix[6] * rhs.m_matrix[1]  + lhs.m_matrix[10] * rhs.m_matrix[2]  + lhs.m_matrix[14] * rhs.m_matrix[3];
		matrix.m_matrix[6] =  lhs.m_matrix[2] * rhs.m_matrix[4]  + lhs.m_matrix[6] * rhs.m_matrix[5]  + lhs.m_matrix[10] * rhs.m_matrix[6]  + lhs.m_matrix[14] * rhs.m_matrix[7];
		matrix.m_matrix[10] = lhs.m_matrix[2] * rhs.m_matrix[8]  + lhs.m_matrix[6] * rhs.m_matrix[9]  + lhs.m_matrix[10] * rhs.m_matrix[10] + lhs.m_matrix[14] * rhs.m_matrix[11];
		matrix.m_matrix[14] = lhs.m_matrix[2] * rhs.m_matrix[12] + lhs.m_matrix[6] * rhs.m_matrix[13] + lhs.m_matrix[10] * rhs.m_matrix[14] + lhs.m_matrix[14] * rhs.m_matrix[15];

		// Fourth row
		matrix.m_matrix[3] = lhs.m_matrix[3] * rhs.m_matrix[0]  + lhs.m_matrix[7] * rhs.m_matrix[1]  + lhs.m_matrix[11] * rhs.m_matrix[2]  + lhs.m_matrix[15] * rhs.m_matrix[3];
		matrix.m_matrix[7] = lhs.m_matrix[3] * rhs.m_matrix[4]  + lhs.m_matrix[7] * rhs.m_matrix[5]  + lhs.m_matrix[11] * rhs.m_matrix[6]  + lhs.m_matrix[15] * rhs.m_matrix[7];
		matrix.m_matrix[11] = lhs.m_matrix[3] * rhs.m_matrix[8]  + lhs.m_matrix[7] * rhs.m_matrix[9]  + lhs.m_matrix[11] * rhs.m_matrix[10] + lhs.m_matrix[15] * rhs.m_matrix[11];
		matrix.m_matrix[15] = lhs.m_matrix[3] * rhs.m_matrix[12] + lhs.m_matrix[7] * rhs.m_matrix[13] + lhs.m_matrix[11] * rhs.m_matrix[14] + lhs.m_matrix[15] * rhs.m_matrix[15];


		return matrix;
	}

	static Matrix44 FromEulerAngles( float pitch, float yaw, float roll = 0 )
	{
		Matrix44 matrix;

		//TODO: Fix this.
		Matrix44 kYMat = Matrix44::CreatePitchRotationMatrixDegrees(  yaw );

		Matrix44 kXMat  = Matrix44::CreateRollRotationMatrixDegrees (  pitch  );

		Matrix44 kZMat = Matrix44::CreateYawRotationMatrixDegress( roll );
		
		//matrix = Multiply( kYMat, Multiply( kXMat, kZMat ) );
		//matrix = kZMat * kYMat * kXMat;
		matrix = kYMat * ( kXMat * kZMat );
		return matrix;
	}

	static Matrix44 CalculateInverse( const Matrix44& matrix )
	{
		// Code taken from GLM
		//
		float coef00 = matrix.m_matrix[10] * matrix.m_matrix[15] - matrix.m_matrix[14] * matrix.m_matrix[11];
		float coef02 = matrix.m_matrix[6]  * matrix.m_matrix[15] - matrix.m_matrix[14] * matrix.m_matrix[7];
		float coef03 = matrix.m_matrix[6]  * matrix.m_matrix[11] - matrix.m_matrix[10] * matrix.m_matrix[7];

		float coef04 = matrix.m_matrix[9]  * matrix.m_matrix[15] - matrix.m_matrix[13] * matrix.m_matrix[11];
		float coef06 = matrix.m_matrix[5]  * matrix.m_matrix[15] - matrix.m_matrix[13] * matrix.m_matrix[7];
		float coef07 = matrix.m_matrix[5]  * matrix.m_matrix[11] - matrix.m_matrix[9]  * matrix.m_matrix[7];

		float coef08 = matrix.m_matrix[9]  * matrix.m_matrix[14] - matrix.m_matrix[13] * matrix.m_matrix[10];
		float coef10 = matrix.m_matrix[5]  * matrix.m_matrix[14] - matrix.m_matrix[13] * matrix.m_matrix[6];
		float coef11 = matrix.m_matrix[5]  * matrix.m_matrix[10] - matrix.m_matrix[9]  * matrix.m_matrix[6];

		float coef12 = matrix.m_matrix[8]  * matrix.m_matrix[15] - matrix.m_matrix[12] * matrix.m_matrix[11];
		float coef14 = matrix.m_matrix[4]  * matrix.m_matrix[15] - matrix.m_matrix[12] * matrix.m_matrix[7];
		float coef15 = matrix.m_matrix[4]  * matrix.m_matrix[11] - matrix.m_matrix[8]  * matrix.m_matrix[7];

		float coef16 = matrix.m_matrix[8]  * matrix.m_matrix[14] - matrix.m_matrix[12] * matrix.m_matrix[10];
		float coef18 = matrix.m_matrix[4]  * matrix.m_matrix[14] - matrix.m_matrix[12] * matrix.m_matrix[6];
		float coef19 = matrix.m_matrix[4]  * matrix.m_matrix[10] - matrix.m_matrix[8]  * matrix.m_matrix[6];

		float coef20 = matrix.m_matrix[8]  * matrix.m_matrix[13] - matrix.m_matrix[12] * matrix.m_matrix[9];
		float coef22 = matrix.m_matrix[4]  * matrix.m_matrix[13] - matrix.m_matrix[12] * matrix.m_matrix[5];
		float coef23 = matrix.m_matrix[4]  * matrix.m_matrix[9]  - matrix.m_matrix[8]  * matrix.m_matrix[5];

		Vector4 signA(  1.0f, -1.0f,  1.0f, -1.0f );
		Vector4 signB( -1.0f,  1.0f, -1.0f,  1.0f );

		Vector4 fac0( coef00, coef00, coef02, coef03 );
		Vector4 fac1( coef04, coef04, coef06, coef07 );
		Vector4 fac2( coef08, coef08, coef10, coef11 );
		Vector4 fac3( coef12, coef12, coef14, coef15 );
		Vector4 fac4( coef16, coef16, coef18, coef19 );
		Vector4 fac5( coef20, coef20, coef22, coef23 );

		Vector4 vec0( matrix.m_matrix[4], matrix.m_matrix[0], matrix.m_matrix[0], matrix.m_matrix[0] );
		Vector4 vec1( matrix.m_matrix[5], matrix.m_matrix[1], matrix.m_matrix[1], matrix.m_matrix[1] );
		Vector4 vec2( matrix.m_matrix[6], matrix.m_matrix[2], matrix.m_matrix[2], matrix.m_matrix[2] );
		Vector4 vec3( matrix.m_matrix[7], matrix.m_matrix[3], matrix.m_matrix[3], matrix.m_matrix[3] );

		Vector4 inv0 = signA * ( vec1 * fac0 - vec2 * fac1 + vec3 * fac2 );
		Vector4 inv1 = signB * ( vec0 * fac0 - vec2 * fac3 + vec3 * fac4 );
		Vector4 inv2 = signA * ( vec0 * fac1 - vec1 * fac3 + vec3 * fac5 );
		Vector4 inv3 = signB * ( vec0 * fac2 - vec1 * fac4 + vec2 * fac5 );

		Matrix44 inverse( inv0, inv1, inv2, inv3 );

		Vector4 row0( inverse.m_matrix[0], inverse.m_matrix[4], inverse.m_matrix[8], inverse.m_matrix[12] );
		Vector4 column0( matrix.m_matrix[0], matrix.m_matrix[1], matrix.m_matrix[2], matrix.m_matrix[3] );
		float determinant = Vector4::DotProduct( column0, row0 );

		inverse /= determinant;

		return inverse;
	}

	static Matrix44 MakeLookAt( float eyeX, float eyeY, float eyeZ, float centerX, float centerY, float centerZ, float upX, float upY, float upZ)
	{
		//
		Matrix44 lookAt;
		if( eyeX == centerX &&
			eyeY == centerY &&
			eyeZ == centerZ )
			return CreateTranslationMatrix(Vector3(-eyeX,-eyeY,-eyeZ));
		Vector3 ev( eyeX, eyeY, eyeZ ); // Position
		Vector3 cv( centerX, centerY, centerZ ); // Where to look at!
		Vector3 uv( upX, upY, upZ ); // Who's up? X, Y, Z?

		// Grant-Schmidt fix-up
		//
		Vector3 n = ( ev + cv.Reverse() );
		n.Normalize();

		Vector3 u = uv.CrossProduct( n );
		u.Normalize();

		// Since n and u are normalized cross product is already normalized
		//
		Vector3 v = n.CrossProduct( u );

		// Row 1
		//
		lookAt.m_matrix[ 0 ] = u[ 0 ];
		lookAt.m_matrix[ 1 ] = v[ 0 ];
		lookAt.m_matrix[ 2 ] = n[ 0 ];
		lookAt.m_matrix[ 3 ] = 0.0f;

		// Row 2
		//
		lookAt.m_matrix[ 4 ] = u[ 1 ];
		lookAt.m_matrix[ 5 ] = v[ 1 ];
		lookAt.m_matrix[ 6 ] = n[ 1 ];
		lookAt.m_matrix[ 7 ] = 0.0f;

		// Row 3
		//
		lookAt.m_matrix[ 8 ]  = u[ 2 ];
		lookAt.m_matrix[ 9 ]  = v[ 2 ];
		lookAt.m_matrix[ 10 ] = n[ 2 ];
		lookAt.m_matrix[ 11 ] = 0.0f;

		// Row 4
		//
		lookAt.m_matrix[ 12 ] = u.Reverse().DotProduct( ev );
		lookAt.m_matrix[ 13 ] = v.Reverse().DotProduct( ev );
		lookAt.m_matrix[ 14 ] = n.Reverse().DotProduct( ev );
		lookAt.m_matrix[ 15 ] = 1.0f;		

		return lookAt;
	}

	static Matrix44 MakePerspective( float fov, float aspect, float nearPlane, float farPlane ) 
	{
		Matrix44 perspective;
		float D2R = PI / 180.0f;
		float yScale = 1.0f / tan( D2R * fov / 2.0f );
		float xScale = yScale / aspect;
		float nearmfar = nearPlane - farPlane;
		float m[] = 
		{
			xScale, 0, 0, 0,
			0, yScale, 0, 0,
			0, 0, (farPlane + nearPlane) / nearmfar, -1,
			0, 0, 2*farPlane*nearPlane / nearmfar, 0 
		};

		memcpy( perspective.m_matrix, m, sizeof( float ) * 16 );

		return perspective;
	}

	static Matrix44 MakeOrtho( const float& left, const float& right, const float& bottom, const float& top, const float& zNear, const float& zFar )
	{		
		Matrix44 ortho;
		ortho.m_matrix[ 0 ] =  2 / ( right - left );
		ortho.m_matrix[ 5 ] =  2 / ( top - bottom );
		ortho.m_matrix[ 10 ] = - 2 / ( zFar - zNear );
		ortho.m_matrix[ 12 ] = - ( right + left ) / ( right - left );
		ortho.m_matrix[ 13 ] = - ( top + bottom ) / ( top - bottom );
		ortho.m_matrix[ 14 ] = - (zFar + zNear) / (zFar - zNear);
		return ortho;
	}

	static Matrix44 Rotate( const Matrix44& matrixToTransformFrom, float const& angle, Vector3& rotationVector, bool usingRadians = false )
	{
		Matrix44 rotatedMatrix;

		// Converting the angle if necessary
		//
		float radians = ( usingRadians ) ? angle : ConvertToRadians( angle );

		// Trigonometry calculation
		//
		float sn = sinf( radians );
		float cs = cosf( radians );

		// Normalizing the vector
		//
		rotationVector.Normalize();

		// Creating temps
		//
		Vector3 axis( rotationVector );
		Vector3 temp = ( 1 - cs ) * axis;

		// Calculating rotations
		//
		rotatedMatrix.m_matrix[ 0 ] = cs + temp[ 0 ] * axis[ 0 ];
		rotatedMatrix.m_matrix[ 1 ] = 0  + temp[ 0 ] * axis[ 1 ] + sn * axis[ 2 ];
		rotatedMatrix.m_matrix[ 2 ] = 0  + temp[ 0 ] * axis[ 2 ] - sn * axis[ 1 ];

		rotatedMatrix.m_matrix[ 4 ] = 0  + temp[ 1 ] * axis[ 0 ] - sn * axis[ 2 ];
		rotatedMatrix.m_matrix[ 5 ] = cs + temp[ 1 ] * axis[ 1 ];
		rotatedMatrix.m_matrix[ 6 ] = 0  + temp[ 1 ] * axis[ 2 ] + sn * axis[ 0 ];

		rotatedMatrix.m_matrix[ 8 ]  = 0  + temp[ 2 ] * axis[ 0 ] + sn * axis[ 1 ];
		rotatedMatrix.m_matrix[ 9 ]	 = 0  + temp[ 2 ] * axis[ 1 ] - sn * axis[ 0 ];
		rotatedMatrix.m_matrix[ 10 ] = cs + temp[ 2 ] * axis[ 2 ];

		// Calculating the resulting matrix
		//
		Matrix44 result = rotatedMatrix * matrixToTransformFrom;

		return result;
	}

	static Matrix44 Translate( Matrix44& matrixToTransformFrom, Vector3& translationVector )
	{
		Matrix44 result( matrixToTransformFrom );
		Vector3 translation = matrixToTransformFrom.TransformPosition3D( translationVector );

		result.m_matrix[ 12 ] = translation.x;
		result.m_matrix[ 13 ] = translation.y;
		result.m_matrix[ 14 ] = translation.z;

		return result;
	}

	static Matrix44 MakeIdentity()
	{		
		return Matrix44();
	}

	// FUNCTIONS
	//
	Vector2 TransformPosition2D( const Vector2& position );
	Vector2 TransformDirection2D( const Vector2& direction );
	Vector3 TransformPosition3D( const Vector3& position );
	Vector3 TransformDirection3D( const Vector3& direction );
	Vector4 TransformPoint( const Vector4& point );
	void Multiply( const Matrix44& rhs );
	Matrix44 operator*( const Matrix44& rhs );
	void Transpose();

	// GETTERS
	//
	const float * GetMatrix() const { return m_matrix; }
	inline const Vector4 GetColumnI() const { return Vector4( m_matrix[0],  m_matrix[1],  m_matrix[2],   m_matrix[3] ); }
	inline const Vector4 GetColumnJ() const { return Vector4( m_matrix[4],  m_matrix[5],  m_matrix[6],   m_matrix[7] ); }
	inline const Vector4 GetColumnK() const { return Vector4( m_matrix[8],  m_matrix[9],  m_matrix[10],  m_matrix[11] ); }
	inline const Vector4 GetColumnT() const { return Vector4( m_matrix[12], m_matrix[13], m_matrix[14],  m_matrix[15] ); }

	// FRIENDS
	//
	friend void Print( const Matrix44& matrix );
private:
	// DATA
	//
	float m_matrix[16];
};

#endif
