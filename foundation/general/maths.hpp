/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_GENERAL_MATHS_HPP
#define OOE_FOUNDATION_GENERAL_MATHS_HPP

#include <limits>

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"

namespace ooe
{
	namespace maths
	{
		const f32 pi = 3.1415926535897932384626433832795029f;
		const f32 pi_half = 1.5707963267948966192313216916397514f;
		const f32 pi_quarter = .7853981633974483096156608458198757f;
		const f32 pi_inverse = .3183098861837906715377675267450287f;
		const f32 epsilon = std::numeric_limits< f32 >::epsilon() * 10;
	}

	template< u8 >
		class vector;

	template< u8 >
		class matrix;

	class radian;
	class degree;
	struct quaternion;

	typedef vector< 3 > vec3;
	typedef matrix< 3 > mat3;
	typedef matrix< 4 > mat4;
	typedef quaternion quat;

	bool is_equal( f32, f32 ) OOE_VISIBLE;

	template< typename type >
		type axis_angle( const vec3&, const radian& );

	template< typename type >
		type euler_angle( const radian&, const radian&, const radian& );

//--- radian -------------------------------------------------------------------
	class OOE_VISIBLE radian
	{
	public:
		radian( void );
		radian( f32 );
		radian( const degree& );

		operator f32( void ) const;

	private:
		f32 r;
	};

//--- degree -------------------------------------------------------------------
	class OOE_VISIBLE degree
	{
	public:
		degree( void );
		degree( f32 );
		degree( const radian& );

		operator f32( void ) const;

	private:
		f32 d;
	};

//--- vector< 3 > --------------------------------------------------------------
	template<>
		struct OOE_VISIBLE vector< 3 >
	{
		static const vec3 zero;
		static const vec3 unit_x;
		static const vec3 unit_y;
		static const vec3 unit_z;

		f32 x;
		f32 y;
		f32 z;

		vector( void );
		vector( f32, f32, f32 );

		vec3& operator -=( const vec3& );
		vec3& operator +=( const vec3& );
		vec3& operator /=( f32 );
		vec3& operator *=( f32 );
	};

	vec3 operator -( const vec3& ) OOE_VISIBLE;
	vec3 operator -( const vec3&, const vec3& ) OOE_VISIBLE;
	vec3 operator +( const vec3&, const vec3& ) OOE_VISIBLE;
	vec3 operator /( const vec3&, f32 ) OOE_VISIBLE;
	vec3 operator *( const vec3&, f32 ) OOE_VISIBLE;

	f32 magnitude_squared( const vec3& ) OOE_VISIBLE;
	f32 magnitude( const vec3& ) OOE_VISIBLE;
	f32 sum( const vec3& ) OOE_VISIBLE;
	f32 dot( const vec3&, const vec3& ) OOE_VISIBLE;
	vec3 cross( const vec3&, const vec3& ) OOE_VISIBLE;
	vec3 normalise( const vec3& ) OOE_VISIBLE;
	vec3 column( const f32* ) OOE_VISIBLE;
	void column( f32*, const vec3& ) OOE_VISIBLE;

//--- matrix< 3 > --------------------------------------------------------------
	template<>
		class OOE_VISIBLE matrix< 3 >
	{
	public:
		static const mat3 identity;
		static const mat3 zero;

		typedef f32 column[ 3 ];

		matrix( void );
		matrix(	f32, f32, f32,		// column 0
				f32, f32, f32,		// column 1
				f32, f32, f32 );	// column 2

		explicit matrix( const mat4& );
		mat3& operator =( const mat4& );

		column& operator []( u8 );
		const column& operator []( u8 ) const;

		mat3& operator -=( const mat3& );
		mat3& operator +=( const mat3& );
		mat3& operator /=( f32 );
		mat3& operator *=( f32 );
		mat3& operator *=( const mat3& );

	private:
		// 3 by 3 column-major matrix
		column m[ 3 ];
	};

	mat3 operator -( const mat3& ) OOE_VISIBLE;
	mat3 operator -( const mat3&, const mat3& ) OOE_VISIBLE;
	mat3 operator +( const mat3&, const mat3& ) OOE_VISIBLE;
	mat3 operator /( const mat3&, f32 ) OOE_VISIBLE;
	mat3 operator *( const mat3&, f32 ) OOE_VISIBLE;
	mat3 operator *( const mat3&, const mat3& ) OOE_VISIBLE;
	vec3 operator *( const mat3&, const vec3& ) OOE_VISIBLE;

	mat3 transpose( const mat3& ) OOE_VISIBLE;
	mat4 translate( const mat3&, const vec3& ) OOE_VISIBLE;

	template<>
		mat3 axis_angle( const vec3&, const radian& ) OOE_VISIBLE;

	template<>
		mat3 euler_angle( const radian&, const radian&, const radian& ) OOE_VISIBLE;

//--- matrix< 4 > --------------------------------------------------------------
	template<>
		class OOE_VISIBLE matrix< 4 >
	{
	public:
		static const mat4 identity;
		static const mat4 zero;

		typedef f32 column[ 4 ];

		matrix( void );
		matrix(	f32, f32, f32, f32,		// column 0
				f32, f32, f32, f32,		// column 1
				f32, f32, f32, f32,		// column 2
				f32, f32, f32, f32 );	// column 3

		explicit matrix( const mat3& );
		mat4& operator =( const mat3& );

		column& operator []( u8 );
		const column& operator []( u8 ) const;

		mat4& operator -=( const mat4& );
		mat4& operator +=( const mat4& );
		mat4& operator /=( f32 );
		mat4& operator *=( f32 );
		mat4& operator *=( const mat4& );

	private:
		// 4 by 4 column-major matrix
		column m[ 4 ];
	};

	mat4 operator -( const mat4& ) OOE_VISIBLE;
	mat4 operator -( const mat4&, const mat4& ) OOE_VISIBLE;
	mat4 operator +( const mat4&, const mat4& ) OOE_VISIBLE;
	mat4 operator /( const mat4&, f32 ) OOE_VISIBLE;
	mat4 operator *( const mat4&, f32 ) OOE_VISIBLE;
	mat4 operator *( const mat4&, const mat4& ) OOE_VISIBLE;
	vec3 operator *( const mat4&, const vec3& ) OOE_VISIBLE;

	mat4 transpose( const mat4& ) OOE_VISIBLE;
	mat4 translate( const mat4&, const vec3& ) OOE_VISIBLE;
	mat4 scale( const mat4&, const vec3& ) OOE_VISIBLE;
	mat4 perspective( const radian&, f32, f32, f32 ) OOE_VISIBLE;
	mat4 orthographic( f32, f32, f32, f32, f32 = -1, f32 = 1 ) OOE_VISIBLE;
	mat4 look_at( const vec3&, const vec3&, const vec3& ) OOE_VISIBLE;

//--- quaternion ---------------------------------------------------------------
	struct OOE_VISIBLE quaternion
	{
		static const quat zero;
		static const quat identity;

		f32 x;
		f32 y;
		f32 z;
		f32 w;

		quaternion( void );
		quaternion( f32, f32, f32, f32 );

		quat& operator -=( const quat& );
		quat& operator +=( const quat& );
		quat& operator /=( f32 );
		quat& operator *=( f32 );
		quat& operator *=( const quat& );
	};

	quat operator -( const quat& ) OOE_VISIBLE;
	quat operator -( const quat&, const quat& ) OOE_VISIBLE;
	quat operator +( const quat&, const quat& ) OOE_VISIBLE;
	quat operator /( const quat&, f32 ) OOE_VISIBLE;
	quat operator *( const quat&, f32 ) OOE_VISIBLE;
	quat operator *( const quat&, const quat& ) OOE_VISIBLE;
	vec3 operator *( const quat&, const vec3& ) OOE_VISIBLE;

	f32 magnitude_squared( const quat& ) OOE_VISIBLE;
	f32 magnitude( const quat& ) OOE_VISIBLE;
	f32 sum( const quat& ) OOE_VISIBLE;
	quat normalise( const quat& ) OOE_VISIBLE;
	quat conjugate( const quat& ) OOE_VISIBLE;
	quat inverse( const quat& ) OOE_VISIBLE;
	mat3 to_matrix( const quat& ) OOE_VISIBLE;
	quat rotation( const vec3&, const vec3& ) OOE_VISIBLE;
	f32 dot( const quat&, const quat& ) OOE_VISIBLE;
	quat slerp( f32, const quat&, const quat& ) OOE_VISIBLE;
	quat squad( f32, const quat&, const quat&, const quat&, const quat& ) OOE_VISIBLE;

	template<>
		quat axis_angle( const vec3&, const radian& ) OOE_VISIBLE;

	template<>
		quat euler_angle( const radian&, const radian&, const radian& ) OOE_VISIBLE;
}

#endif	// OOE_FOUNDATION_GENERAL_MATHS_HPP
