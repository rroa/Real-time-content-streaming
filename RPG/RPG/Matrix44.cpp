#include "stdafx.h"
#include "Matrix44.h"
#include <iostream>
#include "ReportErrors.h"

Matrix44::Matrix44()
{
	m_matrix[0] = 1; m_matrix[4] = 0; m_matrix[8]  = 0;  m_matrix[12] = 0;
	m_matrix[1] = 0; m_matrix[5] = 1; m_matrix[9]  = 0;  m_matrix[13] = 0;
	m_matrix[2] = 0; m_matrix[6] = 0; m_matrix[10] = 1;  m_matrix[14] = 0;
	m_matrix[3] = 0; m_matrix[7] = 0; m_matrix[11] = 0;  m_matrix[15] = 1;
}

Matrix44::Matrix44( const Matrix44& rhs )
{
	m_matrix[0] = rhs.m_matrix[0]; m_matrix[4] = rhs.m_matrix[4]; m_matrix[8]  = rhs.m_matrix[8];   m_matrix[12] = rhs.m_matrix[12];
	m_matrix[1] = rhs.m_matrix[1]; m_matrix[5] = rhs.m_matrix[5]; m_matrix[9]  = rhs.m_matrix[9];   m_matrix[13] = rhs.m_matrix[13];
	m_matrix[2] = rhs.m_matrix[2]; m_matrix[6] = rhs.m_matrix[6]; m_matrix[10] = rhs.m_matrix[10];  m_matrix[14] = rhs.m_matrix[14];
	m_matrix[3] = rhs.m_matrix[3]; m_matrix[7] = rhs.m_matrix[7]; m_matrix[11] = rhs.m_matrix[11];  m_matrix[15] = rhs.m_matrix[15];
}

Matrix44::Matrix44( const float src[16] )
{
	m_matrix[ 0 ]  = src[ 0 ]; m_matrix[ 4 ]  = src[ 4 ]; m_matrix[ 8 ]  = src[ 8 ];  m_matrix[ 12 ] = src[ 12 ];
	m_matrix[ 1 ]  = src[ 1 ]; m_matrix[ 5 ]  = src[ 5 ]; m_matrix[ 9 ]  = src[ 9 ];  m_matrix[ 13 ] = src[ 13 ];
	m_matrix[ 2 ]  = src[ 2 ]; m_matrix[ 6 ]  = src[ 6 ]; m_matrix[ 10 ] = src[ 10 ]; m_matrix[ 14 ] = src[ 14 ];
	m_matrix[ 3 ]  = src[ 3 ]; m_matrix[ 7 ]  = src[ 7 ]; m_matrix[ 11 ] = src[ 11 ]; m_matrix[ 15 ] = src[ 15 ];
}

Matrix44::Matrix44( float ix, float iy, float iz, float iw, 
					float jx, float jy, float jz, float jw, 
					float kx, float ky, float kz, float kw, 
					float tx, float ty, float tz, float tw )
{
	m_matrix[ 0 ] = ix; m_matrix[ 4 ] = jx; m_matrix[ 8 ]  = kx;  m_matrix[ 12 ] = tx;
	m_matrix[ 1 ] = iy; m_matrix[ 5 ] = jy; m_matrix[ 9 ]  = ky;  m_matrix[ 13 ] = ty;
	m_matrix[ 2 ] = iz; m_matrix[ 6 ] = jz; m_matrix[ 10 ] = kz;  m_matrix[ 14 ] = tz;
	m_matrix[ 3 ] = iw; m_matrix[ 7 ] = jw; m_matrix[ 11 ] = kw;  m_matrix[ 15 ] = tw;
}

Matrix44::Matrix44( Vector4 columnI, Vector4 columnJ, Vector4 columnK, Vector4 columnT )
{
	m_matrix[0] = columnI.x; m_matrix[4] = columnJ.x; m_matrix[8]  = columnK.x; m_matrix[12] = columnT.x;
	m_matrix[1] = columnI.y; m_matrix[5] = columnJ.y; m_matrix[9]  = columnK.y; m_matrix[13] = columnT.y;
	m_matrix[2] = columnI.z; m_matrix[6] = columnJ.z; m_matrix[10] = columnK.z; m_matrix[14] = columnT.z;
	m_matrix[3] = columnI.w; m_matrix[7] = columnJ.w; m_matrix[11] = columnK.w; m_matrix[15] = columnT.w;
}

Matrix44& Matrix44::operator=( const Matrix44& rhs )
{
	m_matrix[0] = rhs.m_matrix[0]; m_matrix[4] = rhs.m_matrix[4]; m_matrix[8]  = rhs.m_matrix[8];   m_matrix[12] = rhs.m_matrix[12];
	m_matrix[1] = rhs.m_matrix[1]; m_matrix[5] = rhs.m_matrix[5]; m_matrix[9]  = rhs.m_matrix[9];   m_matrix[13] = rhs.m_matrix[13];
	m_matrix[2] = rhs.m_matrix[2]; m_matrix[6] = rhs.m_matrix[6]; m_matrix[10] = rhs.m_matrix[10];  m_matrix[14] = rhs.m_matrix[14];
	m_matrix[3] = rhs.m_matrix[3]; m_matrix[7] = rhs.m_matrix[7]; m_matrix[11] = rhs.m_matrix[11];  m_matrix[15] = rhs.m_matrix[15];

	return *this;
}

Matrix44& Matrix44::operator/=( const float& rhs )
{
	Matrix44 tmp = *this;

	// First row.
	//
	m_matrix[ 0 ] = ( rhs > 0 ) ? tmp.m_matrix[ 0 ] / rhs : 0.0f;
	m_matrix[ 1 ] = ( rhs > 0 ) ? tmp.m_matrix[ 1 ] / rhs : 0.0f;
	m_matrix[ 2 ] = ( rhs > 0 ) ? tmp.m_matrix[ 2 ] / rhs : 0.0f;
	m_matrix[ 3 ] = ( rhs > 0 ) ? tmp.m_matrix[ 3 ] / rhs : 0.0f;

	// Second row.						    
	//									    
	m_matrix[ 4 ] = ( rhs > 0 ) ? tmp.m_matrix[ 4 ] / rhs : 0.0f;
	m_matrix[ 5 ] = ( rhs > 0 ) ? tmp.m_matrix[ 5 ] / rhs : 0.0f;
	m_matrix[ 6 ] = ( rhs > 0 ) ? tmp.m_matrix[ 6 ] / rhs : 0.0f;
	m_matrix[ 7 ] = ( rhs > 0 ) ? tmp.m_matrix[ 7 ] / rhs : 0.0f;

	// Third row.
	//
	m_matrix[ 8 ]  = ( rhs > 0 ) ? tmp.m_matrix[ 8 ]  / rhs : 0.0f;
	m_matrix[ 9 ]  = ( rhs > 0 ) ? tmp.m_matrix[ 9 ]  / rhs : 0.0f;
	m_matrix[ 10 ] = ( rhs > 0 ) ? tmp.m_matrix[ 10 ] / rhs : 0.0f;
	m_matrix[ 11 ] = ( rhs > 0 ) ? tmp.m_matrix[ 11 ] / rhs : 0.0f;

	// Fourth row.
	//
	m_matrix[ 12 ] = ( rhs > 0 ) ? tmp.m_matrix[ 12 ] / rhs : 0.0f;
	m_matrix[ 13 ] = ( rhs > 0 ) ? tmp.m_matrix[ 13 ] / rhs : 0.0f;
	m_matrix[ 14 ] = ( rhs > 0 ) ? tmp.m_matrix[ 14 ] / rhs : 0.0f;
	m_matrix[ 15 ] = ( rhs > 0 ) ? tmp.m_matrix[ 15 ] / rhs : 0.0f;

	return *this;
}

Matrix44::~Matrix44()
{}

Vector2 Matrix44::TransformPosition2D( const Vector2& position )
{
	Vector4 position4D( position, false );

	Vector4 transformedPoint = TransformPoint( position4D );

	return Vector2( transformedPoint.x, transformedPoint.y );
}

Vector2 Matrix44::TransformDirection2D( const Vector2& direction )
{
	Vector4 position4D( direction, true );

	Vector4 transformedPoint = TransformPoint( position4D );

	return Vector2( transformedPoint.x, transformedPoint.y );
}

Vector3 Matrix44::TransformPosition3D( const Vector3& position )
{
	Vector4 position4D( position, false );

	Vector4 transformedPoint = TransformPoint( position4D );

	return Vector3( transformedPoint.x, transformedPoint.y, transformedPoint.z );
}

Vector3 Matrix44::TransformDirection3D( const Vector3& direction )
{
	Vector4 position4D( direction, true );

	Vector4 transformedPoint = TransformPoint( position4D );

	return Vector3( transformedPoint.x, transformedPoint.y, transformedPoint.z );
}

Vector4 Matrix44::TransformPoint( const Vector4& point )
{
	Vector4 transformedPoint;

	transformedPoint.x = m_matrix[0] * point.x + m_matrix[4] * point.y + m_matrix[8]  * point.z + m_matrix[12] * point.w;
	transformedPoint.y = m_matrix[1] * point.x + m_matrix[5] * point.y + m_matrix[9]  * point.z + m_matrix[13] * point.w;
	transformedPoint.z = m_matrix[2] * point.x + m_matrix[6] * point.y + m_matrix[10] * point.z + m_matrix[14] * point.w;
	transformedPoint.w = m_matrix[3] * point.x + m_matrix[7] * point.y + m_matrix[11] * point.z + m_matrix[15] * point.w;

	return transformedPoint;
}

void Matrix44::Multiply( const Matrix44& rhs )
{
	// First row
	m_matrix[0]  =  m_matrix[0] * rhs.m_matrix[0]  + m_matrix[4] * rhs.m_matrix[1]  + m_matrix[8] * rhs.m_matrix[2]  + m_matrix[12] * rhs.m_matrix[3];
	m_matrix[4]  =  m_matrix[0] * rhs.m_matrix[4]  + m_matrix[4] * rhs.m_matrix[5]  + m_matrix[8] * rhs.m_matrix[6]  + m_matrix[12] * rhs.m_matrix[7];
	m_matrix[8]  =  m_matrix[0] * rhs.m_matrix[8]  + m_matrix[4] * rhs.m_matrix[9]  + m_matrix[8] * rhs.m_matrix[10] + m_matrix[12] * rhs.m_matrix[11];
	m_matrix[12] =  m_matrix[0] * rhs.m_matrix[12] + m_matrix[4] * rhs.m_matrix[13] + m_matrix[8] * rhs.m_matrix[14] + m_matrix[12] * rhs.m_matrix[15];

	// Second row
	m_matrix[1]  =  m_matrix[1] * rhs.m_matrix[0]  + m_matrix[5] * rhs.m_matrix[1]  + m_matrix[9] * rhs.m_matrix[2]  + m_matrix[13] * rhs.m_matrix[3];
	m_matrix[5]  =  m_matrix[1] * rhs.m_matrix[4]  + m_matrix[5] * rhs.m_matrix[5]  + m_matrix[9] * rhs.m_matrix[6]  + m_matrix[13] * rhs.m_matrix[7];
	m_matrix[9]  =  m_matrix[1] * rhs.m_matrix[8]  + m_matrix[5] * rhs.m_matrix[9]  + m_matrix[9] * rhs.m_matrix[10] + m_matrix[13] * rhs.m_matrix[11];
	m_matrix[13] =  m_matrix[1] * rhs.m_matrix[12] + m_matrix[5] * rhs.m_matrix[13] + m_matrix[9] * rhs.m_matrix[14] + m_matrix[13] * rhs.m_matrix[15];

	// Third row
	m_matrix[2]  =  m_matrix[2] * rhs.m_matrix[0]  + m_matrix[6] * rhs.m_matrix[1]  + m_matrix[10] * rhs.m_matrix[2]  + m_matrix[14] * rhs.m_matrix[3];
	m_matrix[6]  =  m_matrix[2] * rhs.m_matrix[4]  + m_matrix[6] * rhs.m_matrix[5]  + m_matrix[10] * rhs.m_matrix[6]  + m_matrix[14] * rhs.m_matrix[7];
	m_matrix[10] = m_matrix[2] * rhs.m_matrix[8]  + m_matrix[6] * rhs.m_matrix[9]  + m_matrix[10] * rhs.m_matrix[10] + m_matrix[14] * rhs.m_matrix[11];
	m_matrix[14] = m_matrix[2] * rhs.m_matrix[12] + m_matrix[6] * rhs.m_matrix[13] + m_matrix[10] * rhs.m_matrix[14] + m_matrix[14] * rhs.m_matrix[15];

	// Fourth row
	m_matrix[3]  = m_matrix[3] * rhs.m_matrix[0]  + m_matrix[7] * rhs.m_matrix[1]  + m_matrix[11] * rhs.m_matrix[2]  + m_matrix[15] * rhs.m_matrix[3];
	m_matrix[7]  = m_matrix[3] * rhs.m_matrix[4]  + m_matrix[7] * rhs.m_matrix[5]  + m_matrix[11] * rhs.m_matrix[6]  + m_matrix[15] * rhs.m_matrix[7];
	m_matrix[11] = m_matrix[3] * rhs.m_matrix[8]  + m_matrix[7] * rhs.m_matrix[9]  + m_matrix[11] * rhs.m_matrix[10] + m_matrix[15] * rhs.m_matrix[11];
	m_matrix[15] = m_matrix[3] * rhs.m_matrix[12] + m_matrix[7] * rhs.m_matrix[13] + m_matrix[11] * rhs.m_matrix[14] + m_matrix[15] * rhs.m_matrix[15];
}

Matrix44 Matrix44::operator*( const Matrix44& rhs )
{
	Matrix44 result;

	// First row
	result.m_matrix[0]  =  m_matrix[0] * rhs.m_matrix[0]  + m_matrix[4] * rhs.m_matrix[1]  + m_matrix[8] * rhs.m_matrix[2]  + m_matrix[12] * rhs.m_matrix[3];
	result.m_matrix[4]  =  m_matrix[0] * rhs.m_matrix[4]  + m_matrix[4] * rhs.m_matrix[5]  + m_matrix[8] * rhs.m_matrix[6]  + m_matrix[12] * rhs.m_matrix[7];
	result.m_matrix[8]  =  m_matrix[0] * rhs.m_matrix[8]  + m_matrix[4] * rhs.m_matrix[9]  + m_matrix[8] * rhs.m_matrix[10] + m_matrix[12] * rhs.m_matrix[11];
	result.m_matrix[12] =  m_matrix[0] * rhs.m_matrix[12] + m_matrix[4] * rhs.m_matrix[13] + m_matrix[8] * rhs.m_matrix[14] + m_matrix[12] * rhs.m_matrix[15];

	// Second row
	result.m_matrix[1] =   m_matrix[1] * rhs.m_matrix[0]  + m_matrix[5] * rhs.m_matrix[1]  + m_matrix[9] * rhs.m_matrix[2]  + m_matrix[13] * rhs.m_matrix[3];
	result.m_matrix[5] =   m_matrix[1] * rhs.m_matrix[4]  + m_matrix[5] * rhs.m_matrix[5]  + m_matrix[9] * rhs.m_matrix[6]  + m_matrix[13] * rhs.m_matrix[7];
	result.m_matrix[9] =   m_matrix[1] * rhs.m_matrix[8]  + m_matrix[5] * rhs.m_matrix[9]  + m_matrix[9] * rhs.m_matrix[10] + m_matrix[13] * rhs.m_matrix[11];
	result.m_matrix[13] =  m_matrix[1] * rhs.m_matrix[12] + m_matrix[5] * rhs.m_matrix[13] + m_matrix[9] * rhs.m_matrix[14] + m_matrix[13] * rhs.m_matrix[15];

	// Third row
	result.m_matrix[2]  =  m_matrix[2] * rhs.m_matrix[0]  + m_matrix[6] * rhs.m_matrix[1]  + m_matrix[10] * rhs.m_matrix[2]  + m_matrix[14] * rhs.m_matrix[3];
	result.m_matrix[6]  =  m_matrix[2] * rhs.m_matrix[4]  + m_matrix[6] * rhs.m_matrix[5]  + m_matrix[10] * rhs.m_matrix[6]  + m_matrix[14] * rhs.m_matrix[7];
	result.m_matrix[10] =  m_matrix[2] * rhs.m_matrix[8]  + m_matrix[6] * rhs.m_matrix[9]  + m_matrix[10] * rhs.m_matrix[10] + m_matrix[14] * rhs.m_matrix[11];
	result.m_matrix[14] =  m_matrix[2] * rhs.m_matrix[12] + m_matrix[6] * rhs.m_matrix[13] + m_matrix[10] * rhs.m_matrix[14] + m_matrix[14] * rhs.m_matrix[15];

	// Fourth row
	result.m_matrix[3]  = m_matrix[3] * rhs.m_matrix[0]  + m_matrix[7] * rhs.m_matrix[1]  + m_matrix[11] * rhs.m_matrix[2]  + m_matrix[15] * rhs.m_matrix[3];
	result.m_matrix[7]  = m_matrix[3] * rhs.m_matrix[4]  + m_matrix[7] * rhs.m_matrix[5]  + m_matrix[11] * rhs.m_matrix[6]  + m_matrix[15] * rhs.m_matrix[7];
	result.m_matrix[11] = m_matrix[3] * rhs.m_matrix[8]  + m_matrix[7] * rhs.m_matrix[9]  + m_matrix[11] * rhs.m_matrix[10] + m_matrix[15] * rhs.m_matrix[11];
	result.m_matrix[15] = m_matrix[3] * rhs.m_matrix[12] + m_matrix[7] * rhs.m_matrix[13] + m_matrix[11] * rhs.m_matrix[14] + m_matrix[15] * rhs.m_matrix[15];

	return result;
}

void Matrix44::Transpose()
{
	Matrix44 tmp = *this;

	// First row.
	//
	m_matrix[ 0 ] = tmp.m_matrix[ 0 ];
	m_matrix[ 1 ] = tmp.m_matrix[ 4 ];
	m_matrix[ 2 ] = tmp.m_matrix[ 8 ];
	m_matrix[ 3 ] = tmp.m_matrix[ 12 ];

	// Second row.
	//
	m_matrix[ 4 ] = tmp.m_matrix[ 1 ];
	m_matrix[ 5 ] = tmp.m_matrix[ 5 ];
	m_matrix[ 6 ] = tmp.m_matrix[ 9 ];
	m_matrix[ 7 ] = tmp.m_matrix[ 13 ];

	// Third row.
	//
	m_matrix[ 8 ]  = tmp.m_matrix[ 2 ];
	m_matrix[ 9 ]  = tmp.m_matrix[ 6 ];
	m_matrix[ 10 ] = tmp.m_matrix[ 10 ];
	m_matrix[ 11 ] = tmp.m_matrix[ 14 ];

	// Fourth row.
	//
	m_matrix[ 12 ] = tmp.m_matrix[ 3 ];
	m_matrix[ 13 ] = tmp.m_matrix[ 7 ];
	m_matrix[ 14 ] = tmp.m_matrix[ 11 ];
	m_matrix[ 15 ] = tmp.m_matrix[ 15 ];
}

// FRIENDS
//
void Print( const Matrix44& matrix )
{
	DEBUG( matrix.m_matrix[0] << " " << matrix.m_matrix[4] << " " << matrix.m_matrix[8]  << " " << matrix.m_matrix[12] );
	DEBUG( matrix.m_matrix[1] << " " << matrix.m_matrix[5] << " " << matrix.m_matrix[9]  << " " << matrix.m_matrix[13] );
	DEBUG( matrix.m_matrix[2] << " " << matrix.m_matrix[6] << " " << matrix.m_matrix[10] << " " << matrix.m_matrix[14] );
	DEBUG( matrix.m_matrix[3] << " " << matrix.m_matrix[7] << " " << matrix.m_matrix[11] << " " << matrix.m_matrix[15] );
}