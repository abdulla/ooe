/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/general/maths.hpp"
#include "foundation/utility/convert.hpp"
#include "test/unit/assert.hpp"
#include "test/unit/group.hpp"

namespace
{
	using namespace ooe;

	struct data {};
	typedef unit::group< data, 5 > group_type;
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

			vec3 s = v - u;
			assert( "v - u == { -2, 0, 2 }", s == vec3( -2, 0, 2 ) );

			s = v + u;
			assert( "v + u == { 2, 2, 2 }", s == vec3( 2, 2, 2 ) );

			s = v / 2;
			assert( "v / 2 == { 0, .5, 1 }", s == vec3( 0, .5, 1 ) );

			s = v * 2;
			assert( "v * 2 == { 0, 2, 4 }", s == vec3( 0, 2, 4 ) );

			assert( "| v |\u00b2 == 5", is_equal( magnitude_squared( v ), 5 ) );
			assert( "| v | == 2.23606797", is_equal( magnitude( v ), 2.23606797 ) );
			assert( "\u03a3 v == 3", is_equal( sum( v ), 3 ) );
			assert( "v . u == 1", is_equal( dot( v, u ), 1 ) );
			assert( "v x u == { -2, 4, -2 }", cross( v, u ) == vec3( -2, 4, -2 ) );
			assert( "|| v || == { 0, 0.44721359, 0.89442719 }",
				normalise( v ) == vec3( 0, 0.44721359, 0.89442719 ) );

			f32 f[ 3 ];
			column( f, v );
			assert( "f[ 0 ] == 0", is_equal( f[ 0 ], 0 ) );
			assert( "f[ 1 ] == 1", is_equal( f[ 1 ], 1 ) );
			assert( "f[ 2 ] == 2", is_equal( f[ 2 ], 2 ) );

			f32 g[] = { 2, 1, 0 };
			s = column( g );
			assert( "u == { 2, 1, 0 }", u == s );
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
			assert( "m == { 0, 1, 2, 3, 4, 5, 6, 7, 8 }",
				m == mat3(
				0, 1, 2,
				3, 4, 5,
				6, 7, 8 ) );

			m = -m;
			assert( "m == { 0, -1, -2, -3, -4, -5, -6, -7, -8 }",
				m == mat3(
				0, -1, -2,
				-3, -4, -5,
				-6, -7, -8 ) );

			m = m + mat3( n );
			assert( "m == mat3::zero", m == mat3::zero );

			m = mat3( n ) / 2;
			assert( "m == { 0, .5, 1, 1.5, 2, 2.5, 3, 3.5, 4 }",
				m == mat3(
				0, .5, 1,
				1.5, 2, 2.5,
				3, 3.5, 4 ) );

			m = mat3( n ) * 2;
			assert( "m == { 0, 2, 4, 6, 8, 10, 12, 14, 16 }",
				m == mat3(
				0, 2, 4,
				6, 8, 10,
				12, 14, 16 ) );

			m = mat3( n ) * vec3( 0, 1, 2 );
			std::cerr <<
				m[ 0 ][ 0 ] << ", " << m[ 0 ][ 1 ] << ", " << m[ 0 ][ 2 ] << '\n' <<
				m[ 1 ][ 0 ] << ", " << m[ 1 ][ 1 ] << ", " << m[ 1 ][ 2 ] << '\n' <<
				m[ 2 ][ 0 ] << ", " << m[ 2 ][ 1 ] << ", " << m[ 2 ][ 2 ] << '\n';
			assert( "m == { 0, 0, 0, 3, 4, 5, 12, 14, 16 }",
				m == mat3(
				0, 0, 0,
				3, 4, 5,
				12, 14, 16 ) );

			m = mat3( n ) * mat3( n );
			assert( "m == { 15, 18, 21, 42, 54, 66, 69, 90, 111 }",
				m == mat3(
				15, 18, 21,
				42, 54, 66,
				69, 90, 111 ) );

			vec3 v = dot( mat3( n ), vec3( 0, 1, 2 ) );
			assert( "m . v == { 5, 14, 23 }", v == vec3( 5, 14, 23 ) );

			v = dot( vec3( 0, 1, 2 ), mat3( n ) );
			assert( "v . m == { 15, 18, 21 }", v == vec3( 15, 18, 21 ) );

			m = transpose( mat3( n ) );
			assert( "m T == { 0, 3, 6, 1, 4, 7, 2, 5, 8 }",
				m == mat3(
				0, 3, 6,
				1, 4, 7,
				2, 5, 8 ) );
		}
	}
}
