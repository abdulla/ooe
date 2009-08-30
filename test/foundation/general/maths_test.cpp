/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/general/maths.hpp"
#include "test/unit/assert.hpp"
#include "test/unit/group.hpp"

namespace
{
	using namespace ooe;

	struct data {};
	typedef unit::group< data, 7 > group_type;
	typedef group_type::fixture_type fixture_type;
	group_type group( "maths" );
}

namespace ooe
{
	namespace unit
	{
		template<>
		template<>
			void fixture_type::test< 0 >( void )
		{
			std::cerr << "is_equal\n";

			assert( "1 == 1", is_equal( 1, 1 ) );
			assert( "1 != 2", !is_equal( 1, 2 ) );
		}

		template<>
		template<>
			void fixture_type::test< 1 >( void )
		{
			std::cerr << "radian\n";

			assert( "1 rad == pi", is_equal( radian( maths::pi ), maths::pi ) );
			assert( "180 deg == pi", is_equal( radian( degree( 180 ) ), maths::pi ) );
		}

		template<>
		template<>
			void fixture_type::test< 2 >( void )
		{
			std::cerr << "degree\n";

			assert( "180 deg == 180", is_equal( degree( 180 ), 180 ) );
			assert( "180 deg == pi", is_equal( degree( radian( maths::pi ) ), 180 ) );
		}

		template<>
		template<>
			void fixture_type::test< 3 >( void )
		{
			std::cerr << "vector< 3 >\n";

			vec3 v( 0, 1, 2 );
			assert( "v.x == 0", is_equal( v.x, 0 ) );
			assert( "v.y == 1", is_equal( v.y, 1 ) );
			assert( "v.z == 2", is_equal( v.z, 2 ) );
			assert( "v == v", v == v );

			vec3 u( 2, 1, 0 );
			assert( "v != u", v != u );

			vec3 t = -v;
			assert( "-v = { 0 -1 -2 }", t == vec3( 0, -1, -2 ) );

			t = v - u;
			assert( "v - u == { -2 0 2 }", t == vec3( -2, 0, 2 ) );

			t = v + u;
			assert( "v + u == { 2 2 2 }", t == vec3( 2, 2, 2 ) );

			t = v / 2;
			assert( "v / 2 == { 0 .5 1 }", t == vec3( 0, .5, 1 ) );

			t = v * 2;
			assert( "v * 2 == { 0 2 4 }", t == vec3( 0, 2, 4 ) );

			f32 f = magnitude_squared( v );
			assert( "| v |\u00b2 == 5", is_equal( f, 5 ) );

			f = magnitude( v );
			assert( "| v | == 2.23606797", is_equal( f, 2.23606797 ) );

			f = sum( v );
			assert( "\u03a3 v == 3", is_equal( f, 3 ) );

			f = dot( v, u );
			assert( "v . u == 1", is_equal( f, 1 ) );

			t = cross( v, u );
			assert( "v x u == { -2 4 -2 }", t == vec3( -2, 4, -2 ) );

			t = normalise( v );
			assert( "|| v || == { 0 0.44721359 0.89442719 }",
				t == vec3( 0, 0.44721359, 0.89442719 ) );

			f32 c[ 3 ];
			column( c, v );
			assert( "c[ 0 ] == 0", is_equal( c[ 0 ], 0 ) );
			assert( "c[ 1 ] == 1", is_equal( c[ 1 ], 1 ) );
			assert( "c[ 2 ] == 2", is_equal( c[ 2 ], 2 ) );

			f32 d[] = { 2, 1, 0 };
			assert( "u == { 2 1 0 }", u == column( d ) );
		}

		template<>
		template<>
			void fixture_type::test< 4 >( void )
		{
			std::cerr << "matrix< 3 >\n";

			mat4 n( 0, 1, 2, 9,
					3, 4, 5, 9,
					6, 7, 8, 9,
					9, 9, 9, 9 );

			mat3 m( n );
			assert( "m == { 0 1 2 3 4 5 6 7 8 }",
				m == mat3(
				0, 1, 2,
				3, 4, 5,
				6, 7, 8 ) );

			m = -m;
			assert( "-m == { 0 -1 -2 -3 -4 -5 -6 -7 -8 }",
				m == mat3(
				0, -1, -2,
				-3, -4, -5,
				-6, -7, -8 ) );

			m = m + mat3( n );
			assert( "m - m == mat3::zero", m == mat3::zero );

			m = mat3( n ) / 2;
			assert( "m / 2 == { 0 .5 1 1.5 2 2.5 3 3.5 4 }",
				m == mat3(
				0, .5, 1,
				1.5, 2, 2.5,
				3, 3.5, 4 ) );

			m = mat3( n ) * 2;
			assert( "m * 2 == { 0 2 4 6 8 10 12 14 16 }",
				m == mat3(
				0, 2, 4,
				6, 8, 10,
				12, 14, 16 ) );

			m = mat3( n ) * vec3( 0, 1, 2 );
			assert( "m * v == { 0 0 0 3 4 5 12 14 16 }",
				m == mat3(
				0, 0, 0,
				3, 4, 5,
				12, 14, 16 ) );

			m = mat3( n ) * mat3( n );
			assert( "m * m == { 15 18 21 42 54 66 69 90,111 }",
				m == mat3(
				15, 18, 21,
				42, 54, 66,
				69, 90, 111 ) );

			vec3 v = dot( mat3( n ), vec3( 0, 1, 2 ) );
			assert( "m . v == { 5 14 23 }", v == vec3( 5, 14, 23 ) );

			v = dot( vec3( 0, 1, 2 ), mat3( n ) );
			assert( "v . m == { 15 18 21 }", v == vec3( 15, 18, 21 ) );

			m = transpose( mat3( n ) );
			assert( "m T == { 0 3 6 1 4 7 2 5 8 }",
				m == mat3(
				0, 3, 6,
				1, 4, 7,
				2, 5, 8 ) );

			mat4 o = translate( mat3( n ), vec3( 0, 1, 2 ) );
			assert( "translate m v == { 0 1 2 0 3 4 5 0 6 7 8 0 5 18 21 1 }",
				o == mat4(
				0, 1, 2, 0,
				3, 4, 5, 0,
				6, 7, 8, 0,
				15, 18, 21, 1 ) );

			m = axis_angle< mat3 >( vec3( .5, 1, 0 ), degree( 90 ) );
			assert( "axis-angle v 90\u00b0 == { .25 .5 -1 .5 1 .5 1 -.5 0 }",
				m == mat3(
				.25, .5, -1,
				.5, 1, .5,
				1, -.5, 0 ) );

			m = euler_angle< mat3 >( degree( 90 ), degree( 90 ), degree( 90 ) );
			assert( "euler-angle 90\u00b0 90\u00b0 90\u00b0 == { 0 1 0 1 0 0 0 0 -1 }",
				m == mat3(
				0, 1, 0,
				1, 0, 0,
				0, 0, -1 ) );
		}

		template<>
		template<>
			void fixture_type::test< 5 >( void )
		{
			std::cerr << "matrix< 4 >\n";

			mat3 n( 0, 1, 2,
					3, 4, 5,
					6, 7, 8 );

			mat4 m( n );
			assert( "m == { 0 1 2 0 3 4 5 0 6 7 8 0 0 0 0 1 }",
				m == mat4(
				0, 1, 2, 0,
				3, 4, 5, 0,
				6, 7, 8, 0,
				0, 0, 0, 1 ) );

			m = -m;
			assert( "-m = { 0 -1 -2 0 -3 -4 -5 0 -6 -7 -8 0 0 0 0 -1 }",
				m == mat4(
				0, -1, -2, 0,
				-3, -4, -5, 0,
				-6, -7, -8, 0,
				0, 0, 0, -1 ) );

			m = m + mat4( n );
			assert( "m - m == mat4::zero", m == mat4::zero );

			m = mat4( n ) / 2;
			assert( "m / 2 == { 0 .5 1 0 1.5 2 2.5 0 3 3.5 4 0 0 0 0 .5 }",
				m == mat4(
				0, .5, 1, 0,
				1.5, 2, 2.5, 0,
				3, 3.5, 4, 0,
				0, 0, 0, .5 ) );

			m = mat4( n ) * 2;
			assert( "m * 2 == { 0 2 4 0 6 8 10 0 12 14 16 0 0 0 0 2 }",
				m == mat4(
				0, 2, 4, 0,
				6, 8, 10, 0,
				12, 14, 16, 0,
				0, 0, 0, 2 ) );

			m = mat4( n ) * mat4( n );
			assert( "m * m == { 15 18 21 0 42 54 66 0 69 90 111 0 0 0 0 1 }",
				m == mat4(
				15, 18, 21, 0,
				42, 54, 66, 0,
				69, 90, 111, 0,
				0, 0, 0, 1 ) );

			vec3 v = dot( mat4( n ), vec3( 0, 1, 2 ) );
			assert( "m . v == { 5 14 23 }", v == vec3( 5, 14, 23 ) );

			v = dot( vec3( 0, 1, 2 ), mat4( n ) );
			assert( "v . m == { 15 18 21 }", v == vec3( 15, 18, 21 ) );

			m = transpose( mat4( n ) );
			assert( "m T == { 0 3 6 0 1 4 7 0 2 5 8 0 0 0 0 1 }",
				m == mat4(
				0, 3, 6, 0,
				1, 4, 7, 0,
				2, 5, 8, 0,
				0, 0, 0, 1 ) );

			m = translate( mat4( n ), vec3( 0, 1, 2 ) );
			assert( "translate m v == { 0 1 2 0 3 4 5 0 6 7 8 0 15 18 21 1 }",
				m == mat4(
				0, 1, 2, 0,
				3, 4, 5, 0,
				6, 7, 8, 0,
				15, 18, 21, 1 ) );

			m = scale( mat4( n ), vec3( 0, 1, 2 ) );
			assert( "scale m v == { 0 1 4 0 0 4 10 0 0 7 16 0 0 0 0 1 }",
				m == mat4(
				0, 1, 4, 0,
				0, 4, 10, 0,
				0, 7, 16, 0,
				0, 0, 0, 1 ) );

			m = perspective( degree( 45 ), 4. / 3., 1, 100 );
			assert( "perspective 45\u00b0 4/3 1 100 == "
				"{ 1.810660 0 0 0 0 2.414213 0 0 0 0 -1.020202 -1 0 0 -2.020202 0 }",
				m == mat4(
				1.810660, 0, 0, 0,
				0, 2.414213, 0, 0,
				0, 0, -1.020202, -1,
				0, 0, -2.020202, 0 ) );

			m = orthographic( -1, 1, -1, 1 );
			assert( "orthographic -1 1 -1 1 == { 1 0 0 0 0 1 0 0 0 0 -1 0 0 0 0 1 }",
				m == mat4(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, -1, 0,
				0, 0, 0, 1 ) );

			m = look_at( vec3::zero, vec3::unit_x, vec3::unit_y );
			assert( "look-at v0 vx vy == { 0 0 -1 0 0 1 0 0 1 0 0 0 0 0 0 1 }",
				m == mat4(
				0, 0, -1, 0,
				0, 1, 0, 0,
				1, 0, 0, 0,
				0, 0, 0, 1 ) );
		}

		template<>
		template<>
			void fixture_type::test< 6 >( void )
		{
			std::cerr << "quaternion\n";

			quat q( 0, 1, 2, 3 );
			assert( "q.x == 0", is_equal( q.x, 0 ) );
			assert( "q.y == 1", is_equal( q.y, 1 ) );
			assert( "q.z == 2", is_equal( q.z, 2 ) );
			assert( "q.w == 3", is_equal( q.w, 3 ) );

			quat r( 3, 2, 1, 0 );
			assert( "q != r", q != r );

			quat s = -q;
			assert( "-q == { 0 -1 -2 -3 }", s == quat( 0, -1, -2, -3 ) );

			s = q - r;
			assert( "q - r == { -3 -1 1 3 }", s == quat( -3, -1, 1, 3 ) );

			s = q + r;
			assert( "q + r == { 3 3 3 3 }", s == quat( 3, 3, 3, 3 ) );

			s = q / 2;
			assert( "q / 2 == { 0 .5 1 1.5 }", s == quat( 0, .5, 1, 1.5 ) );

			s = q * 2;
			assert( "q * 2 == { 0 2 4 6 }", s == quat( 0, 2, 4, 6 ) );

			vec3 v = quat::identity * vec3( 0, 1, 2 );
			assert( "quat::identity * v == { 0 1 2 }", v == vec3( 0, 1, 2 ) );

			s = q * quat::identity;
			assert( "q * quat::identity == q", s == q );

			f32 f = dot( q, r );
			assert( "q . r == 4", is_equal( f, 4 ) );

			f = magnitude_squared( q );
			assert( "| q |\u00b22 == 14", is_equal( f, 14 ) );

			f = magnitude( q );
			assert( "| q | == 3.74165738", is_equal( f, 3.74165738 ) );

			f = sum( q );
			assert( "\u03a3 q == 6", is_equal( f, 6 ) );

			s = normalise( q );
			assert( "|| q || == { 0, 0.26726124, 0.53452247, 0.80178368 }",
				s == quat( 0, 0.26726124, 0.53452247, 0.80178368 ) );

			s = conjugate( q );
			assert( "q* == { 0 -1 -2 3 }", s == quat( 0, -1, -2, 3 ) );

			s = inverse( q );
			assert( "q\u207b\u00b9 == { 0, -0.07142858, -0.14285715, 0.21428572 }",
				s == quat( 0, -0.07142858, -0.14285715, 0.21428572 ) );

			mat3 m = to_matrix( quat::z_180 );
			assert( "to-matrix q == m",
				m == mat3(
				-1, 0, 0,
				0, -1, 0,
				0, 0, 1 ) );

			s = rotate( vec3::unit_x, vec3::unit_y );
			assert( "rotate vec3::unit_x vec3::unit_y == { 0, 0, 0.70710677, 0.70710677 }",
				s == quat( 0, 0, 0.70710677, 0.70710677 ) );

			s = slerp( .5, quat::x_180, quat::y_180 );
			assert( "slerp .5 quat::x_180 quat::y_180 == { 0.70710677, 0.70710677, 0, 0 }",
				s == quat( 0.70710677, 0.70710677, 0, 0 ) );

			s = squad( .5, quat::x_180, quat::y_180, quat::y_180, quat::z_180 );
			assert( "squad .5 quat::x_180 quat::y_180 quat::x_180, quat::z_180 == "
				"{ 0.49999997, 0.70710677, 0.49999997, 0 }",
				s == quat( 0.49999997, 0.70710677, 0.49999997, 0 ) );

			s = axis_angle< quat >( vec3::unit_x, degree( 180 ) );
			assert( "axis-angle vec3::unit_x 180\u00b0 == quat::x_180", s == quat::x_180 );

			s = euler_angle< quat >( degree( 180 ), 0, 0 );
			assert( "euler-angle 180\u00b0 0 0 == quat::x_180", s == quat::x_180 );
		}
	}
}
