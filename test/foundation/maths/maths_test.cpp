/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/maths/maths.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

namespace
{
	using namespace ooe;

	typedef unit::group< empty_t, empty_t, 7 > group_type;
	typedef group_type::fixture_type fixture_type;
	group_type group( "maths" );
}

namespace ooe
{
	namespace unit
	{
		template<>
		template<>
			void fixture_type::test< 0 >( empty_t& )
		{
			std::cerr << "is_equal\n";

			check( "1 == 1", is_equal( 1, 1 ) );
			check( "1 != 2", !is_equal( 1, 2 ) );
		}

		template<>
		template<>
			void fixture_type::test< 1 >( empty_t& )
		{
			std::cerr << "radian\n";

			check( "1 rad == pi", is_equal( radian( maths::pi ), maths::pi ) );
			check( "180 deg == pi", is_equal( radian( degree( 180 ) ), maths::pi ) );
		}

		template<>
		template<>
			void fixture_type::test< 2 >( empty_t& )
		{
			std::cerr << "degree\n";

			check( "180 deg == 180", is_equal( degree( 180 ), 180 ) );
			check( "180 deg == pi", is_equal( degree( radian( maths::pi ) ), 180 ) );
		}

		template<>
		template<>
			void fixture_type::test< 3 >( empty_t& )
		{
			std::cerr << "vector< 3 >\n";

			vec3 v( 0, 1, 2 );
			check( "v.x == 0", is_equal( v.x, 0 ) );
			check( "v.y == 1", is_equal( v.y, 1 ) );
			check( "v.z == 2", is_equal( v.z, 2 ) );
			check( "v == v", v == v );

			vec3 u( 2, 1, 0 );
			check( "v != u", v != u );

			vec3 t = -v;
			check( "-v = { 0 -1 -2 }", t == vec3( 0, -1, -2 ) );

			t = v - u;
			check( "v - u == { -2 0 2 }", t == vec3( -2, 0, 2 ) );

			t = v + u;
			check( "v + u == { 2 2 2 }", t == vec3( 2, 2, 2 ) );

			t = v / 2;
			check( "v / 2 == { 0 .5 1 }", t == vec3( 0, .5, 1 ) );

			t = v * 2;
			check( "v * 2 == { 0 2 4 }", t == vec3( 0, 2, 4 ) );

			f32 f = magnitude_squared( v );
			check( "| v |\u00b2 == 5", is_equal( f, 5 ) );

			f = magnitude( v );
			check( "| v | == 2.23606797", is_equal( f, 2.23606797 ) );

			f = sum( v );
			check( "\u03a3 v == 3", is_equal( f, 3 ) );

			f = dot( v, u );
			check( "v . u == 1", is_equal( f, 1 ) );

			t = cross( v, u );
			check( "v x u == { -2 4 -2 }", t == vec3( -2, 4, -2 ) );

			t = normalise( v );
			check( "|| v || == { 0 0.44721359 0.89442719 }",
				t == vec3( 0, 0.44721359, 0.89442719 ) );

			f32 c[ 3 ];
			column( c, v );
			check( "c[ 0 ] == 0", is_equal( c[ 0 ], 0 ) );
			check( "c[ 1 ] == 1", is_equal( c[ 1 ], 1 ) );
			check( "c[ 2 ] == 2", is_equal( c[ 2 ], 2 ) );

			f32 d[] = { 2, 1, 0 };
			check( "u == { 2 1 0 }", u == column( d ) );
		}

		template<>
		template<>
			void fixture_type::test< 4 >( empty_t& )
		{
			std::cerr << "matrix< 3 >\n";

			mat4 n( 0, 1, 2, 9,
					3, 4, 5, 9,
					6, 7, 8, 9,
					9, 9, 9, 9 );

			mat3 m( n );
			check( "m == { 0 1 2 3 4 5 6 7 8 }",
				m == mat3(
				0, 1, 2,
				3, 4, 5,
				6, 7, 8 ) );

			m = -m;
			check( "-m == { 0 -1 -2 -3 -4 -5 -6 -7 -8 }",
				m == mat3(
				0, -1, -2,
				-3, -4, -5,
				-6, -7, -8 ) );

			m = m + mat3( n );
			check( "m - m == mat3::zero", m == mat3::zero );

			m = mat3( n ) / 2;
			check( "m / 2 == { 0 .5 1 1.5 2 2.5 3 3.5 4 }",
				m == mat3(
				0, .5, 1,
				1.5, 2, 2.5,
				3, 3.5, 4 ) );

			m = mat3( n ) * 2;
			check( "m * 2 == { 0 2 4 6 8 10 12 14 16 }",
				m == mat3(
				0, 2, 4,
				6, 8, 10,
				12, 14, 16 ) );

			m = mat3( n ) * vec3( 0, 1, 2 );
			check( "m * v == { 0 0 0 3 4 5 12 14 16 }",
				m == mat3(
				0, 0, 0,
				3, 4, 5,
				12, 14, 16 ) );

			m = mat3( n ) * mat3( n );
			check( "m * m == { 15 18 21 42 54 66 69 90,111 }",
				m == mat3(
				15, 18, 21,
				42, 54, 66,
				69, 90, 111 ) );

			vec3 v = dot( mat3( n ), vec3( 0, 1, 2 ) );
			check( "m . v == { 5 14 23 }", v == vec3( 5, 14, 23 ) );

			v = dot( vec3( 0, 1, 2 ), mat3( n ) );
			check( "v . m == { 15 18 21 }", v == vec3( 15, 18, 21 ) );

			m = transpose( mat3( n ) );
			check( "m T == { 0 3 6 1 4 7 2 5 8 }",
				m == mat3(
				0, 3, 6,
				1, 4, 7,
				2, 5, 8 ) );

			mat4 o = translate( mat3( n ), vec3( 0, 1, 2 ) );
			check( "translate m v == { 0 1 2 0 3 4 5 0 6 7 8 0 5 18 21 1 }",
				o == mat4(
				0, 1, 2, 0,
				3, 4, 5, 0,
				6, 7, 8, 0,
				15, 18, 21, 1 ) );

			m = axis_angle< mat3 >( vec3( .5, 1, 0 ), degree( 90 ) );
			check( "axis-angle v 90\u00b0 == { .25 .5 -1 .5 1 .5 1 -.5 0 }",
				m == mat3(
				.25, .5, -1,
				.5, 1, .5,
				1, -.5, 0 ) );

			m = euler_angle< mat3 >( degree( 90 ), degree( 90 ), degree( 90 ) );
			check( "euler-angle 90\u00b0 90\u00b0 90\u00b0 == { 0 1 0 1 0 0 0 0 -1 }",
				m == mat3(
				0, 1, 0,
				1, 0, 0,
				0, 0, -1 ) );
		}

		template<>
		template<>
			void fixture_type::test< 5 >( empty_t& )
		{
			std::cerr << "matrix< 4 >\n";

			mat3 n( 0, 1, 2,
					3, 4, 5,
					6, 7, 8 );

			mat4 m( n );
			check( "m == { 0 1 2 0 3 4 5 0 6 7 8 0 0 0 0 1 }",
				m == mat4(
				0, 1, 2, 0,
				3, 4, 5, 0,
				6, 7, 8, 0,
				0, 0, 0, 1 ) );

			m = -m;
			check( "-m = { 0 -1 -2 0 -3 -4 -5 0 -6 -7 -8 0 0 0 0 -1 }",
				m == mat4(
				0, -1, -2, 0,
				-3, -4, -5, 0,
				-6, -7, -8, 0,
				0, 0, 0, -1 ) );

			m = m + mat4( n );
			check( "m - m == mat4::zero", m == mat4::zero );

			m = mat4( n ) / 2;
			check( "m / 2 == { 0 .5 1 0 1.5 2 2.5 0 3 3.5 4 0 0 0 0 .5 }",
				m == mat4(
				0, .5, 1, 0,
				1.5, 2, 2.5, 0,
				3, 3.5, 4, 0,
				0, 0, 0, .5 ) );

			m = mat4( n ) * 2;
			check( "m * 2 == { 0 2 4 0 6 8 10 0 12 14 16 0 0 0 0 2 }",
				m == mat4(
				0, 2, 4, 0,
				6, 8, 10, 0,
				12, 14, 16, 0,
				0, 0, 0, 2 ) );

			m = mat4( n ) * mat4( n );
			check( "m * m == { 15 18 21 0 42 54 66 0 69 90 111 0 0 0 0 1 }",
				m == mat4(
				15, 18, 21, 0,
				42, 54, 66, 0,
				69, 90, 111, 0,
				0, 0, 0, 1 ) );

			vec3 v = dot( mat4( n ), vec3( 0, 1, 2 ) );
			check( "m . v == { 5 14 23 }", v == vec3( 5, 14, 23 ) );

			v = dot( vec3( 0, 1, 2 ), mat4( n ) );
			check( "v . m == { 15 18 21 }", v == vec3( 15, 18, 21 ) );

			m = transpose( mat4( n ) );
			check( "m T == { 0 3 6 0 1 4 7 0 2 5 8 0 0 0 0 1 }",
				m == mat4(
				0, 3, 6, 0,
				1, 4, 7, 0,
				2, 5, 8, 0,
				0, 0, 0, 1 ) );

			m = translate( mat4( n ), vec3( 0, 1, 2 ) );
			check( "translate m v == { 0 1 2 0 3 4 5 0 6 7 8 0 15 18 21 1 }",
				m == mat4(
				0, 1, 2, 0,
				3, 4, 5, 0,
				6, 7, 8, 0,
				15, 18, 21, 1 ) );

			m = scale( mat4( n ), vec3( 0, 1, 2 ) );
			check( "scale m v == { 0 1 4 0 0 4 10 0 0 7 16 0 0 0 0 1 }",
				m == mat4(
				0, 1, 4, 0,
				0, 4, 10, 0,
				0, 7, 16, 0,
				0, 0, 0, 1 ) );

			m = perspective( degree( 45 ), 4. / 3., 1, 100 );
			check( "perspective 45\u00b0 4/3 1 100 == "
				"{ 1.810660 0 0 0 0 2.414213 0 0 0 0 -1.020202 -1 0 0 -2.020202 0 }",
				m == mat4(
				1.810660, 0, 0, 0,
				0, 2.414213, 0, 0,
				0, 0, -1.020202, -1,
				0, 0, -2.020202, 0 ) );

			m = orthographic( -1, 1, -1, 1 );
			check( "orthographic -1 1 -1 1 == { 1 0 0 0 0 1 0 0 0 0 -1 0 0 0 0 1 }",
				m == mat4(
				1, 0, 0, 0,
				0, 1, 0, 0,
				0, 0, -1, 0,
				0, 0, 0, 1 ) );

			m = look_at( vec3::zero, vec3::unit_x, vec3::unit_y );
			check( "look-at v0 vx vy == { 0 0 -1 0 0 1 0 0 1 0 0 0 0 0 0 1 }",
				m == mat4(
				0, 0, -1, 0,
				0, 1, 0, 0,
				1, 0, 0, 0,
				0, 0, 0, 1 ) );
		}

		template<>
		template<>
			void fixture_type::test< 6 >( empty_t& )
		{
			std::cerr << "quaternion\n";

			quat q( 0, 1, 2, 3 );
			check( "q.x == 0", is_equal( q.x, 0 ) );
			check( "q.y == 1", is_equal( q.y, 1 ) );
			check( "q.z == 2", is_equal( q.z, 2 ) );
			check( "q.w == 3", is_equal( q.w, 3 ) );

			quat r( 3, 2, 1, 0 );
			check( "q != r", q != r );

			quat s = -q;
			check( "-q == { 0 -1 -2 -3 }", s == quat( 0, -1, -2, -3 ) );

			s = q - r;
			check( "q - r == { -3 -1 1 3 }", s == quat( -3, -1, 1, 3 ) );

			s = q + r;
			check( "q + r == { 3 3 3 3 }", s == quat( 3, 3, 3, 3 ) );

			s = q / 2;
			check( "q / 2 == { 0 .5 1 1.5 }", s == quat( 0, .5, 1, 1.5 ) );

			s = q * 2;
			check( "q * 2 == { 0 2 4 6 }", s == quat( 0, 2, 4, 6 ) );

			vec3 v = quat::identity * vec3( 0, 1, 2 );
			check( "quat::identity * v == { 0 1 2 }", v == vec3( 0, 1, 2 ) );

			s = q * quat::identity;
			check( "q * quat::identity == q", s == q );

			f32 f = dot( q, r );
			check( "q . r == 4", is_equal( f, 4 ) );

			f = magnitude_squared( q );
			check( "| q |\u00b22 == 14", is_equal( f, 14 ) );

			f = magnitude( q );
			check( "| q | == 3.74165738", is_equal( f, 3.74165738 ) );

			f = sum( q );
			check( "\u03a3 q == 6", is_equal( f, 6 ) );

			s = normalise( q );
			check( "|| q || == { 0, 0.26726124, 0.53452247, 0.80178368 }",
				s == quat( 0, 0.26726124, 0.53452247, 0.80178368 ) );

			s = conjugate( q );
			check( "q* == { 0 -1 -2 3 }", s == quat( 0, -1, -2, 3 ) );

			s = inverse( q );
			check( "q\u207b\u00b9 == { 0, -0.07142858, -0.14285715, 0.21428572 }",
				s == quat( 0, -0.07142858, -0.14285715, 0.21428572 ) );

			mat3 m = to_matrix( quat::z_180 );
			check( "to-matrix q == m",
				m == mat3(
				-1, 0, 0,
				0, -1, 0,
				0, 0, 1 ) );

			s = rotate( vec3::unit_x, vec3::unit_y );
			check( "rotate vec3::unit_x vec3::unit_y == { 0, 0, 0.70710677, 0.70710677 }",
				s == quat( 0, 0, 0.70710677, 0.70710677 ) );

			s = slerp( .5, quat::x_180, quat::y_180 );
			check( "slerp .5 quat::x_180 quat::y_180 == { 0.70710677, 0.70710677, 0, 0 }",
				s == quat( 0.70710677, 0.70710677, 0, 0 ) );

			s = squad( .5, quat::x_180, quat::y_180, quat::y_180, quat::z_180 );
			check( "squad .5 quat::x_180 quat::y_180 quat::x_180, quat::z_180 == "
				"{ 0.49999997, 0.70710677, 0.49999997, 0 }",
				s == quat( 0.49999997, 0.70710677, 0.49999997, 0 ) );

			s = axis_angle< quat >( vec3::unit_x, degree( 180 ) );
			check( "axis-angle vec3::unit_x 180\u00b0 == quat::x_180", s == quat::x_180 );

			s = euler_angle< quat >( degree( 180 ), 0, 0 );
			check( "euler-angle 180\u00b0 0 0 == quat::x_180", s == quat::x_180 );
		}
	}
}
