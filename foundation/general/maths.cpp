/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <cmath>

#include "foundation/general/maths.hpp"

namespace ooe
{
//--- is_equal -----------------------------------------------------------------
	bool is_equal( f32 x, f32 y )
	{
		return std::abs( x - y ) < maths::epsilon;
	}

//--- radian -------------------------------------------------------------------
	radian::radian( void )
		: r()
	{
	}

	radian::radian( f32 i )
		: r( i )
	{
	}

	radian::radian( const degree& i )
		: r( i * ( maths::pi / 180 ) )
	{
	}

	radian::operator f32( void ) const
	{
		return r;
	}

//--- degree -------------------------------------------------------------------
	degree::degree( void )
		: d()
	{
	}

	degree::degree( f32 i )
		: d( i )
	{
	}

	degree::degree( const radian& i )
		: d ( i * ( 180 / maths::pi ) )
	{
	}

	degree::operator f32( void ) const
	{
		return d;
	}

//--- vector< 3 > --------------------------------------------------------------
	const vec3 vec3::zero( 0, 0, 0 );
	const vec3 vec3::unit_x( 1, 0, 0 );
	const vec3 vec3::unit_y( 0, 1, 0 );
	const vec3 vec3::unit_z( 0, 0, 1 );

	vec3::vector( void )
		: x(), y(), z()
	{
	}

	vec3::vector( f32 x_, f32 y_, f32 z_ )
		: x( x_ ), y( y_ ), z( z_ )
	{
	}

	vec3& vec3::operator -=( const vector& v )
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	vec3& vec3::operator +=( const vector& v )
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	vec3& vec3::operator /=( f32 f )
	{
		x /= f;
		y /= f;
		z /= f;
		return *this;
	}

	vec3& vec3::operator *=( f32 f )
	{
		x *= f;
		y *= f;
		z *= f;
		return *this;
	}

	vec3 operator -( const vec3& v )
	{
		return vec3( -v.x, -v.y, -v.z );
	}

	vec3 operator -( const vec3& v, const vec3& u )
	{
		return vec3( v ) -= u;
	}

	vec3 operator +( const vec3& v, const vec3& u )
	{
		return vec3( v ) += u;
	}

	vec3 operator /( const vec3& v, f32 u )
	{
		return vec3( v ) /= u;
	}

	vec3 operator *( const vec3& v, f32 u )
	{
		return vec3( v ) *= u;
	}

	f32 magnitude_squared( const vec3& v )
	{
		return v.x * v.x + v.y * v.y + v.z * v.z;
	}

	f32 magnitude( const vec3& v )
	{
		return std::sqrt( magnitude_squared( v ) );
	}

	f32 sum( const vec3& v )
	{
		return v.x + v.y + v.z;
	}

	f32 dot( const vec3& v, const vec3& u )
	{
		return v.x * u.x + v.y * u.y + v.z * u.z;
	}

	vec3 cross( const vec3& v, const vec3& u )
	{
		return vec3( v.y * u.z - v.z * u.y, v.z * u.x - v.x * u.z, v.x * u.y - v.y * u.x );
	}

	vec3 normalise( const vec3& v )
	{
		return v / magnitude( v );
	}

	vec3 column( const f32* c )
	{
		return vec3( c[ 0 ], c[ 1 ], c[ 2 ] );
	}

	void column( f32* c, const vec3& v )
	{
		c[ 0 ] = v.x;
		c[ 1 ] = v.y;
		c[ 2 ] = v.z;
	}

//--- matrix< 3 > --------------------------------------------------------------
	const mat3 mat3::identity(
		1, 0, 0,
		0, 1, 0,
		0, 0, 1 );

	const mat3 mat3::zero(
		0, 0, 0,
		0, 0, 0,
		0, 0, 0 );

	mat3::matrix( void )
		: m()
	{
	}

	mat3::matrix(
		f32 _00, f32 _01, f32 _02,
		f32 _10, f32 _11, f32 _12,
		f32 _20, f32 _21, f32 _22 )
		: m()
	{
		m[ 0 ][ 0 ] = _00;	m[ 0 ][ 1 ] = _01;	m[ 0 ][ 2 ] = _02;
		m[ 1 ][ 0 ] = _10;	m[ 1 ][ 1 ] = _11;	m[ 1 ][ 2 ] = _12;
		m[ 2 ][ 0 ] = _20;	m[ 2 ][ 1 ] = _21;	m[ 2 ][ 2 ] = _22;
	}

	mat3::matrix( const mat4& i )
		: m()
	{
		*this = i;
	}

	mat3& mat3::operator =( const mat4& n )
	{
		m[ 0 ][ 0 ] = n[ 0 ][ 0 ];
		m[ 0 ][ 1 ] = n[ 0 ][ 1 ];
		m[ 0 ][ 2 ] = n[ 0 ][ 2 ];

		m[ 1 ][ 0 ] = n[ 1 ][ 0 ];
		m[ 1 ][ 1 ] = n[ 1 ][ 1 ];
		m[ 1 ][ 2 ] = n[ 1 ][ 2 ];

		m[ 2 ][ 0 ] = n[ 2 ][ 0 ];
		m[ 2 ][ 1 ] = n[ 2 ][ 1 ];
		m[ 2 ][ 2 ] = n[ 2 ][ 2 ];
		return *this;
	}

	mat3::column& mat3::operator []( u8 i )
	{
		return m[ i ];
	}

	const mat3::column& mat3::operator []( u8 i ) const
	{
		return m[ i ];
	}

	mat3& mat3::operator -=( const matrix& i )
	{
		return *this += -i;
	}

	mat3& mat3::operator +=( const matrix& i )
	{
		m[ 0 ][ 0 ] += i[ 0 ][ 0 ];
		m[ 0 ][ 1 ] += i[ 0 ][ 1 ];
		m[ 0 ][ 2 ] += i[ 0 ][ 2 ];

		m[ 1 ][ 0 ] += i[ 1 ][ 0 ];
		m[ 1 ][ 1 ] += i[ 1 ][ 1 ];
		m[ 1 ][ 2 ] += i[ 1 ][ 2 ];

		m[ 2 ][ 0 ] += i[ 2 ][ 0 ];
		m[ 2 ][ 1 ] += i[ 2 ][ 1 ];
		m[ 2 ][ 2 ] += i[ 2 ][ 2 ];
		return *this;
	}

	mat3& mat3::operator /=( f32 f )
	{
		return *this *= 1 / f;
	}

	mat3& mat3::operator *=( f32 f )
	{
		m[ 0 ][ 0 ] *= f;	m[ 0 ][ 1 ] *= f;	m[ 0 ][ 2 ] *= f;
		m[ 1 ][ 0 ] *= f;	m[ 1 ][ 1 ] *= f;	m[ 1 ][ 2 ] *= f;
		m[ 2 ][ 0 ] *= f;	m[ 2 ][ 1 ] *= f;	m[ 2 ][ 2 ] *= f;
		return *this;
	}

	mat3& mat3::operator *=( const matrix& i )
	{
		return *this = matrix(
			// i column 0
			m[ 0 ][ 0 ] * i[ 0 ][ 0 ] + m[ 1 ][ 0 ] * i[ 0 ][ 1 ] +
			m[ 2 ][ 0 ] * i[ 0 ][ 2 ],
			m[ 0 ][ 1 ] * i[ 0 ][ 0 ] + m[ 1 ][ 1 ] * i[ 0 ][ 1 ] +
			m[ 2 ][ 1 ] * i[ 0 ][ 2 ],
			m[ 0 ][ 2 ] * i[ 0 ][ 0 ] + m[ 1 ][ 2 ] * i[ 0 ][ 1 ] +
			m[ 2 ][ 2 ] * i[ 0 ][ 2 ],

			// i column 1
			m[ 0 ][ 0 ] * i[ 1 ][ 0 ] + m[ 1 ][ 0 ] * i[ 1 ][ 1 ] +
			m[ 2 ][ 0 ] * i[ 1 ][ 2 ],
			m[ 0 ][ 1 ] * i[ 1 ][ 0 ] + m[ 1 ][ 1 ] * i[ 1 ][ 1 ] +
			m[ 2 ][ 1 ] * i[ 1 ][ 2 ],
			m[ 0 ][ 2 ] * i[ 1 ][ 0 ] + m[ 1 ][ 2 ] * i[ 1 ][ 1 ] +
			m[ 2 ][ 2 ] * i[ 1 ][ 2 ],

			// i column 2
			m[ 0 ][ 0 ] * i[ 2 ][ 0 ] + m[ 1 ][ 0 ] * i[ 2 ][ 1 ] +
			m[ 2 ][ 0 ] * i[ 2 ][ 2 ],
			m[ 0 ][ 1 ] * i[ 2 ][ 0 ] + m[ 1 ][ 1 ] * i[ 2 ][ 1 ] +
			m[ 2 ][ 1 ] * i[ 2 ][ 2 ],
			m[ 0 ][ 2 ] * i[ 2 ][ 0 ] + m[ 1 ][ 2 ] * i[ 2 ][ 1 ] +
			m[ 2 ][ 2 ] * i[ 2 ][ 2 ] );
	}

	mat3 operator -( const mat3& m )
	{
		return mat3(
			-m[ 0 ][ 0 ],	-m[ 0 ][ 1 ],	-m[ 0 ][ 2 ],
			-m[ 1 ][ 0 ],	-m[ 1 ][ 1 ],	-m[ 1 ][ 2 ],
			-m[ 2 ][ 0 ],	-m[ 2 ][ 1 ],	-m[ 2 ][ 2 ] );
	}

	mat3 operator -( const mat3& one, const mat3& two )
	{
		return mat3( one ) -= two;
	}

	mat3 operator +( const mat3& one, const mat3& two )
	{
		return mat3( one ) += two;
	}

	mat3 operator /( const mat3& one, f32 two )
	{
		return mat3( one ) /= two;
	}

	mat3 operator *( const mat3& one, f32 two )
	{
		return mat3( one ) *= two;
	}

	mat3 operator *( const mat3& one, const mat3& two )
	{
		return mat3( one ) *= two;
	}

	vec3 operator *( const mat3& m, const vec3& v )
	{
		return vec3(
			m[ 0 ][ 0 ] * v.x + m[ 1 ][ 0 ] * v.y + m[ 2 ][ 0 ] * v.z,
			m[ 0 ][ 1 ] * v.x + m[ 1 ][ 1 ] * v.y + m[ 2 ][ 1 ] * v.z,
			m[ 0 ][ 2 ] * v.x + m[ 1 ][ 2 ] * v.y + m[ 2 ][ 2 ] * v.z );
	}

	mat3 transpose( const mat3& m )
	{
		return mat3(
			m[ 0 ][ 0 ], m[ 1 ][ 0 ], m[ 2 ][ 0 ],
			m[ 0 ][ 1 ], m[ 1 ][ 1 ], m[ 2 ][ 1 ],
			m[ 0 ][ 2 ], m[ 1 ][ 2 ], m[ 2 ][ 2 ] );
	}

	mat4 translate( const mat3& m, const vec3& v )
	{
		return mat4(
			m[ 0 ][ 0 ], m[ 0 ][ 1 ], m[ 0 ][ 2 ], 0,
			m[ 1 ][ 0 ], m[ 1 ][ 1 ], m[ 1 ][ 2 ], 0,
			m[ 2 ][ 0 ], m[ 2 ][ 1 ], m[ 2 ][ 2 ], 0,

			m[ 0 ][ 0 ] * v.x + m[ 1 ][ 0 ] * v.y + m[ 2 ][ 0 ] * v.z,
			m[ 0 ][ 1 ] * v.x + m[ 1 ][ 1 ] * v.y + m[ 2 ][ 1 ] * v.z,
			m[ 0 ][ 2 ] * v.x + m[ 1 ][ 2 ] * v.y + m[ 2 ][ 2 ] * v.z,
			1 );
	}

	mat4 scale( const mat4& m, const vec3& v )
	{
		mat4 n(
			v.x, 0, 0, 0,
			0, v.y, 0, 0,
			0, 0, v.z, 0,
			0, 0, 0, 1 );

		return m * n;
	}

	template<>
		mat3 axis_angle( const vec3& v, const radian& r )
	{
		const f32 sine = std::sin( r );
		const f32 cosine = std::cos( r );
		const f32 minus =  1 - cosine;

		const f32 squared[] =
		{
			v.x * v.x,
			v.y * v.y,
			v.z * v.z
		};

		const f32 minused[] =
		{
			v.x * v.y * minus,
			v.x * v.z * minus,
			v.y * v.z * minus
		};

		const f32 sined[] =
		{
			v.x * sine,
			v.y * sine,
			v.z * sine
		};

		return mat3(
			squared[ 0 ] * minus + cosine, minused[ 0 ] + sined[ 2 ], minused[ 1 ] - sined[ 1 ],
			minused[ 0 ] - sined[ 2 ], squared[ 1 ] * minus + cosine, minused[ 2 ] + sined[ 0 ],
			minused[ 1 ] + sined[ 1 ], minused[ 2 ] - sined[ 0 ], squared[ 2 ] * minus + cosine );
	}

	template<>
		mat3 euler_angle( const radian& x, const radian& y, const radian& z )
	{
		f32 x_sin = std::sin( x );
		f32 x_cos = std::cos( x );
		f32 y_sin = std::sin( y );
		f32 y_cos = std::cos( y );
		f32 z_sin = std::sin( z );
		f32 z_cos = std::cos( z );

		return mat3(
			y_cos * z_cos,
			z_sin,
			-y_sin * z_cos,

			x_sin * y_sin - x_cos * y_cos * z_sin,
			x_cos * z_cos,
			x_sin * y_cos + x_cos * y_sin * z_sin,

			x_cos * y_sin + x_sin * y_cos * z_sin,
			x_sin * -z_cos,
			x_cos * y_cos - x_sin * y_sin * z_sin );
	}

//--- matrix< 4 > --------------------------------------------------------------
	const mat4 mat4::identity(
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1 );

	const mat4 mat4::zero(
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0,
		0, 0, 0, 0 );

	mat4::matrix( void )
		: m()
	{
	}

	mat4::matrix(
		f32 _00, f32 _01, f32 _02, f32 _03,
		f32 _10, f32 _11, f32 _12, f32 _13,
		f32 _20, f32 _21, f32 _22, f32 _23,
		f32 _30, f32 _31, f32 _32, f32 _33 )
		: m()
	{
		m[ 0 ][ 0 ] = _00;	m[ 0 ][ 1 ] = _01;	m[ 0 ][ 2 ] = _02;	m[ 0 ][ 3 ] = _03;
		m[ 1 ][ 0 ] = _10;	m[ 1 ][ 1 ] = _11;	m[ 1 ][ 2 ] = _12;	m[ 1 ][ 3 ] = _13;
		m[ 2 ][ 0 ] = _20;	m[ 2 ][ 1 ] = _21;	m[ 2 ][ 2 ] = _22;	m[ 2 ][ 3 ] = _23;
		m[ 3 ][ 0 ] = _30;	m[ 3 ][ 1 ] = _31;	m[ 3 ][ 2 ] = _32;	m[ 3 ][ 3 ] = _33;
	}

	mat4::matrix( const mat3& i )
		: m()
	{
		m[ 3 ][ 2 ] = m[ 3 ][ 1 ] = m[ 3 ][ 0 ] = m[ 2 ][ 3 ] = m[ 1 ][ 3 ] = m[ 0 ][ 3 ] = 0;
		m[ 3 ][ 3 ] = 1;
		*this = i;
	}

	mat4& mat4::operator =( const mat3& n )
	{
		m[ 0 ][ 0 ] = n[ 0 ][ 0 ];
		m[ 0 ][ 1 ] = n[ 0 ][ 1 ];
		m[ 0 ][ 2 ] = n[ 0 ][ 2 ];

		m[ 1 ][ 0 ] = n[ 1 ][ 0 ];
		m[ 1 ][ 1 ] = n[ 1 ][ 1 ];
		m[ 1 ][ 2 ] = n[ 1 ][ 2 ];

		m[ 2 ][ 0 ] = n[ 2 ][ 0 ];
		m[ 2 ][ 1 ] = n[ 2 ][ 1 ];
		m[ 2 ][ 2 ] = n[ 2 ][ 2 ];
		return *this;
	}

	mat4::column& mat4::operator []( u8 i )
	{
		return m[ i ];
	}

	const mat4::column& mat4::operator []( u8 i ) const
	{
		return m[ i ];
	}

	mat4& mat4::operator -=( const matrix& i )
	{
		return *this += -i;
	}

	mat4& mat4::operator +=( const matrix& i )
	{
		m[ 0 ][ 0 ] += i[ 0 ][ 0 ];	m[ 0 ][ 1 ] += i[ 0 ][ 1 ];
		m[ 0 ][ 2 ] += i[ 0 ][ 2 ];	m[ 0 ][ 3 ] += i[ 0 ][ 3 ];

		m[ 1 ][ 0 ] += i[ 1 ][ 0 ];	m[ 1 ][ 1 ] += i[ 1 ][ 1 ];
		m[ 1 ][ 2 ] += i[ 1 ][ 2 ];	m[ 1 ][ 3 ] += i[ 1 ][ 3 ];

		m[ 2 ][ 0 ] += i[ 2 ][ 0 ];	m[ 2 ][ 1 ] += i[ 2 ][ 1 ];
		m[ 2 ][ 2 ] += i[ 2 ][ 2 ];	m[ 2 ][ 3 ] += i[ 2 ][ 3 ];

		m[ 3 ][ 0 ] += i[ 3 ][ 0 ];	m[ 3 ][ 1 ] += i[ 3 ][ 1 ];
		m[ 3 ][ 2 ] += i[ 3 ][ 2 ];	m[ 3 ][ 3 ] += i[ 3 ][ 3 ];
		return *this;
	}

	mat4& mat4::operator /=( f32 f )
	{
		return *this *= 1 / f;
	}

	mat4& mat4::operator *=( f32 f )
	{
		m[ 0 ][ 0 ] *= f;	m[ 0 ][ 1 ] *= f;
		m[ 0 ][ 2 ] *= f;	m[ 0 ][ 3 ] *= f;

		m[ 1 ][ 0 ] *= f;	m[ 1 ][ 1 ] *= f;
		m[ 1 ][ 2 ] *= f;	m[ 1 ][ 3 ] *= f;

		m[ 2 ][ 0 ] *= f;	m[ 2 ][ 1 ] *= f;
		m[ 2 ][ 2 ] *= f;	m[ 2 ][ 3 ] *= f;

		m[ 3 ][ 0 ] *= f;	m[ 3 ][ 1 ] *= f;
		m[ 3 ][ 2 ] *= f;	m[ 3 ][ 3 ] *= f;
		return *this;
	}

	mat4& mat4::operator *=( const matrix& i )
	{
		return *this = matrix(
			// i column 0
			m[ 0 ][ 0 ] * i[ 0 ][ 0 ] + m[ 1 ][ 0 ] * i[ 0 ][ 1 ] +
			m[ 2 ][ 0 ] * i[ 0 ][ 2 ] + m[ 3 ][ 0 ] * i[ 0 ][ 3 ],

			m[ 0 ][ 1 ] * i[ 0 ][ 0 ] + m[ 1 ][ 1 ] * i[ 0 ][ 1 ] +
			m[ 2 ][ 1 ] * i[ 0 ][ 2 ] + m[ 3 ][ 1 ] * i[ 0 ][ 3 ],

			m[ 0 ][ 2 ] * i[ 0 ][ 0 ] + m[ 1 ][ 2 ] * i[ 0 ][ 1 ] +
			m[ 2 ][ 2 ] * i[ 0 ][ 2 ] + m[ 3 ][ 2 ] * i[ 0 ][ 3 ],

			m[ 0 ][ 3 ] * i[ 0 ][ 0 ] + m[ 1 ][ 3 ] * i[ 0 ][ 1 ] +
			m[ 2 ][ 3 ] * i[ 0 ][ 2 ] + m[ 3 ][ 3 ] * i[ 0 ][ 3 ],

			// i column 1
			m[ 0 ][ 0 ] * i[ 1 ][ 0 ] + m[ 1 ][ 0 ] * i[ 1 ][ 1 ] +
			m[ 2 ][ 0 ] * i[ 1 ][ 2 ] + m[ 3 ][ 0 ] * i[ 1 ][ 3 ],

			m[ 0 ][ 1 ] * i[ 1 ][ 0 ] + m[ 1 ][ 1 ] * i[ 1 ][ 1 ] +
			m[ 2 ][ 1 ] * i[ 1 ][ 2 ] + m[ 3 ][ 1 ] * i[ 1 ][ 3 ],

			m[ 0 ][ 2 ] * i[ 1 ][ 0 ] + m[ 1 ][ 2 ] * i[ 1 ][ 1 ] +
			m[ 2 ][ 2 ] * i[ 1 ][ 2 ] + m[ 3 ][ 2 ] * i[ 1 ][ 3 ],

			m[ 0 ][ 3 ] * i[ 1 ][ 0 ] + m[ 1 ][ 3 ] * i[ 1 ][ 1 ] +
			m[ 2 ][ 3 ] * i[ 1 ][ 2 ] + m[ 3 ][ 3 ] * i[ 1 ][ 3 ],

			// i column 2
			m[ 0 ][ 0 ] * i[ 2 ][ 0 ] + m[ 1 ][ 0 ] * i[ 2 ][ 1 ] +
			m[ 2 ][ 0 ] * i[ 2 ][ 2 ] + m[ 3 ][ 0 ] * i[ 2 ][ 3 ],

			m[ 0 ][ 1 ] * i[ 2 ][ 0 ] + m[ 1 ][ 1 ] * i[ 2 ][ 1 ] +
			m[ 2 ][ 1 ] * i[ 2 ][ 2 ] + m[ 3 ][ 1 ] * i[ 2 ][ 3 ],

			m[ 0 ][ 2 ] * i[ 2 ][ 0 ] + m[ 1 ][ 2 ] * i[ 2 ][ 1 ] +
			m[ 2 ][ 2 ] * i[ 2 ][ 2 ] + m[ 3 ][ 2 ] * i[ 2 ][ 3 ],

			m[ 0 ][ 3 ] * i[ 2 ][ 0 ] + m[ 1 ][ 3 ] * i[ 2 ][ 1 ] +
			m[ 2 ][ 3 ] * i[ 2 ][ 2 ] + m[ 3 ][ 3 ] * i[ 2 ][ 3 ],

			// i column 3
			m[ 0 ][ 0 ] * i[ 3 ][ 0 ] + m[ 1 ][ 0 ] * i[ 3 ][ 1 ] +
			m[ 2 ][ 0 ] * i[ 3 ][ 2 ] + m[ 3 ][ 0 ] * i[ 3 ][ 3 ],

			m[ 0 ][ 1 ] * i[ 3 ][ 0 ] + m[ 1 ][ 1 ] * i[ 3 ][ 1 ] +
			m[ 2 ][ 1 ] * i[ 3 ][ 2 ] + m[ 3 ][ 1 ] * i[ 3 ][ 3 ],

			m[ 0 ][ 2 ] * i[ 3 ][ 0 ] + m[ 1 ][ 2 ] * i[ 3 ][ 1 ] +
			m[ 2 ][ 2 ] * i[ 3 ][ 2 ] + m[ 3 ][ 2 ] * i[ 3 ][ 3 ],

			m[ 0 ][ 3 ] * i[ 3 ][ 0 ] + m[ 1 ][ 3 ] * i[ 3 ][ 1 ] +
			m[ 2 ][ 3 ] * i[ 3 ][ 2 ] + m[ 3 ][ 3 ] * i[ 3 ][ 3 ] );
	}

	mat4 operator -( const mat4& m )
	{
		return mat4(
			-m[ 0 ][ 0 ],	-m[ 0 ][ 1 ],	-m[ 0 ][ 2 ],	-m[ 0 ][ 3 ],
			-m[ 1 ][ 0 ],	-m[ 1 ][ 1 ],	-m[ 1 ][ 2 ],	-m[ 1 ][ 3 ],
			-m[ 2 ][ 0 ],	-m[ 2 ][ 1 ],	-m[ 2 ][ 2 ],	-m[ 2 ][ 3 ],
			-m[ 3 ][ 0 ],	-m[ 3 ][ 1 ],	-m[ 3 ][ 2 ],	-m[ 3 ][ 3 ] );
	}

	mat4 operator -( const mat4& one, const mat4& two )
	{
		return mat4( one ) -= two;
	}

	mat4 operator +( const mat4& one, const mat4& two )
	{
		return mat4( one ) += two;
	}

	mat4 operator /( const mat4& one, f32 two )
	{
		return mat4( one ) /= two;
	}

	mat4 operator *( const mat4& one, f32 two )
	{
		return mat4( one ) *= two;
	}

	mat4 operator *( const mat4& m, const mat4& n )
	{
		return mat4( m ) *= n;
	}

	vec3 operator *( const mat4& m, const vec3& v )
	{
		const f32 w_inv =
			1 / ( m[ 0 ][ 3 ] * v.x + m[ 1 ][ 3 ] * v.y + m[ 2 ][ 3 ] * v.z + m[ 3 ][ 3 ] );

		f32 x = m[ 0 ][ 0 ] * v.x + m[ 1 ][ 0 ] * v.y + m[ 2 ][ 0 ] * v.z + m[ 3 ][ 0 ];
		f32 y = m[ 0 ][ 1 ] * v.x + m[ 1 ][ 1 ] * v.y + m[ 2 ][ 1 ] * v.z + m[ 3 ][ 1 ];
		f32 z = m[ 0 ][ 2 ] * v.x + m[ 1 ][ 2 ] * v.y + m[ 2 ][ 2 ] * v.z + m[ 3 ][ 2 ];

		return vec3( x * w_inv, y * w_inv, z * w_inv );
	}

	mat4 transpose( const mat4& m )
	{
		return mat4(
			m[ 0 ][ 0 ], m[ 1 ][ 0 ], m[ 2 ][ 0 ], m[ 3 ][ 0 ],
			m[ 0 ][ 1 ], m[ 1 ][ 1 ], m[ 2 ][ 1 ], m[ 3 ][ 1 ],
			m[ 0 ][ 2 ], m[ 1 ][ 2 ], m[ 2 ][ 2 ], m[ 3 ][ 2 ],
			m[ 0 ][ 3 ], m[ 1 ][ 3 ], m[ 2 ][ 3 ], m[ 3 ][ 3 ] );
	}

	mat4 translate( const mat4& m, const vec3& v )
	{
		return mat4(
			m[ 0 ][ 0 ], m[ 0 ][ 1 ], m[ 0 ][ 2 ], m[ 0 ][ 3 ],
			m[ 1 ][ 0 ], m[ 1 ][ 1 ], m[ 1 ][ 2 ], m[ 1 ][ 3 ],
			m[ 2 ][ 0 ], m[ 2 ][ 1 ], m[ 2 ][ 2 ], m[ 2 ][ 3 ],

			m[ 0 ][ 0 ] * v.x + m[ 1 ][ 0 ] * v.y + m[ 2 ][ 0 ] * v.z + m[ 3 ][ 0 ],
			m[ 0 ][ 1 ] * v.x + m[ 1 ][ 1 ] * v.y + m[ 2 ][ 1 ] * v.z + m[ 3 ][ 1 ],
			m[ 0 ][ 2 ] * v.x + m[ 1 ][ 2 ] * v.y + m[ 2 ][ 2 ] * v.z + m[ 3 ][ 2 ],
			m[ 0 ][ 3 ] * v.x + m[ 1 ][ 3 ] * v.y + m[ 2 ][ 3 ] * v.z + m[ 3 ][ 3 ] );
	}

	mat4 perspective( const radian& fov, f32 aspect, f32 near, f32 far )
	{
		// possibly more precise?
		/*const f32 fov_half = fov * .5;
		const f32 cotangent = std::cos( fov_half ) / std::sin( fov_half );*/
		const f32 cotangent = 1 / std::tan( fov * .5f );
		const f32 delta = -1 / ( far - near );

		return mat4(
			cotangent / aspect, 0, 0, 0,
			0, cotangent, 0, 0,
			0, 0, ( far + near ) * delta, -1,
			0, 0, 2 * far * near * delta, 0 );
	}

	mat4 orthographic( f32 left, f32 right, f32 bottom, f32 top, f32 near, f32 far )
	{
		const f32 mid[ 3 ] =
		{
			1 / ( right - left ),
			1 / ( top - bottom ),
			1 / ( far - near )
		};

		const f32 column[ 3 ] =
		{
			-( right + left ) * mid[ 0 ],
			-( top + bottom ) * mid[ 1 ],
			-( far + near ) * mid[ 2 ]
		};

		return mat4(
			2 * mid[ 0 ], 0, 0, 0,
			0, 2 * mid[ 1 ], 0, 0,
			0, 0, -2 * mid[ 2 ], 0,
			column[ 0 ], column[ 1 ], column[ 2 ], 1 );
	}

	mat4 look_at( const vec3& eye, const vec3& look, const vec3& up )
	{
		const vec3 forward = normalise( look - eye );
		const vec3 side = normalise( cross( forward, up ) );
		const vec3 re_up = cross( side, forward );

		mat3 m(
			side.x, re_up.x, -forward.x,
			side.y, re_up.y, -forward.y,
			side.z, re_up.z, -forward.z );

		return translate( m, -eye );
	}

//--- quaternion ---------------------------------------------------------------
	const quat quat::zero( 0, 0, 0, 0 );
	const quat quat::identity( 0, 0, 0, 1 );

	quat::quaternion( void )
		: x(), y(), z(), w()
	{
	}

	quat::quaternion( f32 x_, f32 y_, f32 z_, f32 w_ )
		: x( x_ ), y( y_ ), z( z_ ), w( w_ )
	{
	}

	quat& quat::operator -=( const quat& q )
	{
		x -= q.x;
		y -= q.y;
		z -= q.z;
		w -= q.w;
		return *this;
	}

	quat& quat::operator +=( const quat& q )
	{
		x += q.x;
		y += q.y;
		z += q.z;
		w += q.w;
		return *this;
	}

	quat& quat::operator /=( f32 f )
	{
		x /= f;
		y /= f;
		z /= f;
		w /= f;
		return *this;
	}

	quat& quat::operator *=( f32 f )
	{
		x *= f;
		y *= f;
		z *= f;
		w *= f;
		return *this;
	}

	quat& quat::operator *=( const quat& q )
	{
		// 16 multiplies, 12 additions
		 x = w * q.x + x * q.w + y * q.z - z * q.y;
		 y = w * q.y - x * q.z + y * q.w + z * q.x;
		 z = w * q.z + x * q.y - y * q.x + z * q.w;
		 w = w * q.w - x * q.x - y * q.y - z * q.z;

		// 12 multiplies, 32 additions
		/*const f32 j[ 8 ] =
		{
			( w + x ) * ( q.w + q.x ),
			( z - y ) * ( q.y - q.z ),
			( w - x ) * ( q.y + q.z ),
			( y + z ) * ( q.w - q.x ),
			( x + z ) * ( q.x + q.y ),
			( x - z ) * ( q.x - q.y ),
			( w + y ) * ( q.w - q.z ),
			( w - y ) * ( q.w + q.z )
		};

		x = j[ 0 ] - ( ( j[ 4 ] + j[ 5 ] + j[ 6 ] + j[ 7 ] ) * .5 );
		y = j[ 2 ] + ( ( j[ 4 ] - j[ 5 ] + j[ 6 ] - j[ 7 ] ) * .5 );
		z = j[ 3 ] + ( ( j[ 4 ] - j[ 5 ] - j[ 6 ] + j[ 7 ] ) * .5 );
		w = j[ 1 ] - ( ( j[ 4 ] + j[ 5 ] - j[ 6 ] - j[ 7 ] ) * .5 );*/

		return *this;
	}

	quat operator -( const quat& q )
	{
		return quat( -q.x, -q.y, -q.z, -q.w );
	}

	quat operator -( const quat& q, const quat& r )
	{
		return quat( q ) -= r;
	}

	quat operator +( const quat& q, const quat& r )
	{
		return quat( q ) += r;
	}

	quat operator /( const quat& q, f32 f )
	{
		return quat( q ) /= f;
	}

	quat operator *( const quat& q, f32 f )
	{
		return quat( q ) *= f;
	}

	quat operator *( const quat& q, const quat& r )
	{
		return quat( q ) *= r;
	}

	vec3 operator *( const quat& q, const vec3& v )
	{
		vec3 x( q.x, q.y, q.z );
		vec3 y = cross( x, v );
		vec3 z = cross( x, y );
		y *= 2 * q.w;
		z *= 2;
		return v + y + z;
	}

	f32 magnitude_squared( const quat& q )
	{
		return q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w;
	}

	f32 magnitude( const quat& q )
	{
		return std::sqrt( magnitude_squared( q ) );
	}

	f32 sum( const quat& q )
	{
		return q.x + q.y + q.z + q.w;
	}

	quat normalise( const quat& q )
	{
		return q / magnitude( q );
	}

	quat conjugate( const quat& q )
	{
		return quat( -q.x, -q.y, -q.z, q.w );
	}

	quat inverse( const quat& q )
	{
		const f32 mag = magnitude_squared( q );

		if ( mag <= 0 )
			return quat::zero;

		return conjugate( q ) / mag;
	}

	mat3 to_matrix( const quat& q )
	{
		const f32 x = q.x + q.x;
		const f32 y = q.y + q.y;
		const f32 z = q.z + q.z;

		const f32 a[ 9 ] =
		{
			q.w * x, q.w * y, q.w * z,
			q.x * x, q.x * y, q.x * z,
			q.y * y, q.y * z, q.z * z
		};

		return mat3(
			1 - ( a[ 6 ] + a[ 8 ] ), a[ 4 ] + a[ 2 ], a[ 5 ] - a[ 1 ],
			a[ 4 ] - a[ 2 ], 1 - ( a[ 3 ] + a[ 8 ] ), a[ 7 ] + a[ 0 ],
			a[ 5 ] + a[ 1 ], a[ 7 ] - a[ 0 ], 1 - ( a[ 3 ] + a[ 6 ] ) );
	}

	quat rotation( const vec3& u, const vec3& v )
	{
		vec3 x = normalise( u );
		const vec3 y = normalise( v );
		const f32 dot_product = dot( x, y );

		if ( dot_product > 1 )
			return quat::identity;

		const f32 root = std::sqrt( 2 + dot_product + dot_product );
		const f32 inv = 1 / root;
		x = cross( x, y );
		return quat( x.x * inv, x.y * inv, x.z * inv, root * .5f );
	}

	f32 dot( const quat& q, const quat& r )
	{
		return q.x * r.x + q.y * r.y + q.z * r.z + q.w * r.w;
	}

	quat slerp( f32 f, const quat& q, const quat& r )
	{
		const f32 angle = std::acos( dot( q, r ) );

		if ( std::abs( angle ) < maths::epsilon )
			return q;

		const f32 sine_inv = 1 / std::sin( angle );
		const f32 displace_q = std::sin( ( 1 - f ) * angle ) * sine_inv;
		const f32 displace_r = std::sin( f * angle ) * sine_inv;
		return q * displace_q + r * displace_r;
	}

	quat squad( f32 f, const quat& q, const quat& r, const quat& s, const quat& t )
	{
		const f32 g = 2 * f * ( 1 - f );
		quat a = slerp( f, q, t );
		quat b = slerp( f, r, s );
		return slerp( g, a, b );
	}

	template<>
		quat axis_angle( const vec3& v, const radian& angle )
	{
		const f32 cosine = std::cos( angle * .5f );
		const f32 sine = std::sin( angle * .5f );
		return quat( v.x * sine, v.y * sine, v.z * sine, cosine );
	}

	template<>
		quat euler_angle( const radian& x, const radian& y, const radian& z )
	{
		const f32 mid[ 6 ] =
		{
			std::sin( x * .5f ),	// sin( roll )
			std::sin( y * .5f ),	// sin( pitch )
			std::sin( z * .5f ),	// sin( yaw )

			std::cos( x * .5f ),	// cos( roll )
			std::cos( y * .5f ),	// cos( pitch )
			std::cos( z * .5f )		// cos( yaw )
		};

		const f32 mix[ 4 ] =
		{
			mid[ 1 ] * mid[ 2 ],	// sin( pitch ), sin( yaw )
			mid[ 4 ] * mid[ 5 ],	// cos( pitch ), cos( yaw )
			mid[ 1 ] * mid[ 5 ],	// sin( pitch ), cos( yaw )
			mid[ 2 ] * mid[ 4 ]		// cos( pitch ), sin( yaw )
		};

		return quat(
			mid[ 0 ] * mix[ 1 ] - mid[ 3 ] * mix[ 0 ],
			mid[ 3 ] * mix[ 2 ] + mid[ 0 ] * mix[ 3 ],
			mid[ 3 ] * mix[ 3 ] - mid[ 0 ] * mix[ 2 ],
			mid[ 0 ] * mix[ 0 ] + mid[ 3 ] * mix[ 1 ] );
	}
}
