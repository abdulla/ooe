/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#ifdef __APPLE__
	#include <OpenGL/glu.h>
#else
	#include <GL/glu.h>
#endif

#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/executable/timer.hpp"
#include "foundation/general/event_queue.hpp"
#include "foundation/general/maths.hpp"
#include "foundation/general/view.hpp"
#include "foundation/utility/guard.hpp"

namespace
{
	using namespace ooe;

	void print( const c8* label, const mat3& m )
	{
		std::cout << "--- " << label << " ---\n";

		for ( u8 i = 0; i != 3; ++i )
			std::cout << m[ 0 ][ i ] << "\t" << m[ 1 ][ i ] << "\t" << m[ 2 ][ i ] << '\n';

		std::cout << '\n';
	}

	void print( const c8* label, const mat4& m )
	{
		std::cout << "--- " << label << " ---\n";

		for ( u8 i = 0; i != 4; ++i )
			std::cout << m[ 0 ][ i ] << "\t" << m[ 1 ][ i ] << "\t" << m[ 2 ][ i ] << "\t" <<
				m[ 3 ][ i ] << '\n';

		std::cout << '\n';
	}

	void print( const c8* label, const vec3& v )
	{
		std::cout << "--- " << label << " ---\n";
		std::cout << v.x << "\t" << v.y << "\t" << v.z << '\n';
		std::cout << "magnitude: " << magnitude( v ) << "\n\n";
	}

	void compare( const mat4& m, const mat4& n )
	{
		for ( u8 i = 0; i != 4; ++i )
		{
			for ( u8 j = 0; j != 4; ++j )
			{
				if ( !is_equal( m[ i ][ j ], n[ i ][ j ] ) )
				{
					std::cout << "********** FAILED [ " << i << " ][ " << j << " ] " <<
						m[ i ][ j ] << " " << n[ i ][ j ] << " **********\n";
				}
			}
		}
	}

	mat4 make( void )
	{
		mat4 m;

		for ( u8 column = 0; column != 4; ++column )
		{
			for ( u8 row = 0; row != 4; ++row)
				m[ column ][ row ] = static_cast< f32 >( std::rand() % 8 );
		}

		return m;
	}

	void multiply( void )
	{
		mat4 a = make();
		mat4 b = make();
		mat4 z = a * b;
		print( "multiply: ooe", z );

		glMatrixMode( GL_PROJECTION );
		glLoadMatrixf( a[ 0 ] );
		glMultMatrixf( b[ 0 ] );
		mat4 c;
		glGetFloatv( GL_PROJECTION_MATRIX, c[ 0 ] );
		print( "multiply: gl", c );

		compare( c, z );
	}

	void translate( void )
	{
		glLoadIdentity();
		glTranslatef( 1, 2, 3 );
		mat4 c;
		glGetFloatv( GL_PROJECTION_MATRIX, c[ 0 ] );
		print( "translate: gl", c );
	}

	void scale( void )
	{
		mat4 a = make();
		mat4 b = scale( a, vec3( 8, 4, 2 ) );
		print( "scale: ooe", b );

		glMatrixMode( GL_PROJECTION );
		glLoadMatrixf( a[ 0 ] );
		glScalef( 8, 4, 2 );
		mat4 r;
		glGetFloatv( GL_PROJECTION_MATRIX, r[ 0 ] );
		print( "scale: gl", r );

		compare( b, r );
	}

	void rotate( void )
	{
		mat4 q( to_matrix( euler_angle< quat >( 0, 0, degree( 90 ) ) ) );
		print( "rotate: ooe (euler - quat)", q );

		mat4 r( axis_angle< mat3 >( vec3( 0, 0, 1 ), degree( 90 ) ) );
		print( "rotate: ooe (axis - matrix)", r );

		compare( q, r );

		print( "vector right", vec3( r[ 0 ][ 0 ], r[ 1 ][ 0 ], r[ 2 ][ 0 ] ) );

		glLoadIdentity();
		glRotatef( 90, 0, 0, 1 );
		mat4 c;
		glGetFloatv( GL_PROJECTION_MATRIX, c[ 0 ] );
		print( "rotate: gl", c );

		compare( c, r );
	}

	void euler_rotate( void )
	{
		mat4 m( euler_angle< mat3 >( 0, 0, degree( 90 ) ) );
		print( "rotate: ooe (euler - matrix)", m );

		glLoadIdentity();
		glRotatef( 90, 0, 0, 1 );
		mat4 c;
		glGetFloatv( GL_PROJECTION_MATRIX, c[ 0 ] );
		print( "rotate: gl", c );

		compare( c, m );
	}

	void rotate_translate( void )
	{
		mat4 s( to_matrix( euler_angle< quat >( 0, degree( 30 ), 0 ) ) );
		mat4 t( mat4::identity );
		column( t[ 3 ], vec3( 0, 0, -2 ) );
		print( "rotate + translate: ooe", s * t );

		glLoadIdentity();
		glRotatef( 30, 0, 1, 0 );
		glTranslatef( 0, 0, -2 );
		mat4 c;
		glGetFloatv( GL_PROJECTION_MATRIX, c[ 0 ] );
		print( "rotate + translate: gl", c );

		compare( s * t, c );
	}

	void translate_rotate( void )
	{
		mat4 s( to_matrix( euler_angle< quat >( 0, degree( 47.5 ), 0 ) ) );
		mat4 t( mat4::identity );
		column( t[ 3 ], vec3( 1, 2, 3 ) );
		print( "translate + rotate: ooe", t * s );

		glLoadIdentity();
		glTranslatef( 1, 2, 3 );
		glRotatef( 47.5, 0, 1, 0 );
		mat4 c;
		glGetFloatv( GL_PROJECTION_MATRIX, c[ 0 ] );
		print( "translate + rotate: gl", c );

		compare( t * s, c );
	}

	void perspective( void )
	{
		mat4 p( ooe::perspective( degree( 90 ), 1.33f, 100, 100000 ) );
		print( "perspective: ooe", p );

		glLoadIdentity();
		gluPerspective( 90, 1.33, 100, 100000 );
		mat4 c;
		glGetFloatv( GL_PROJECTION_MATRIX, c[ 0 ] );
		print( "perspective: gl", c );

		compare( p, c );
	}

	void orthographic( void )
	{
		mat4 o( ooe::orthographic( 0, 1280, 800, 0, -1, 1 ) );
		print( "orthographic: ooe", o );

		glLoadIdentity();
		glOrtho( 0, 1280, 800, 0, -1, 1 );
		mat4 c;
		glGetFloatv( GL_PROJECTION_MATRIX, c[ 0 ] );
		print( "orthographic: gl", c );

		compare( o, c );
	}

	void look_at( void )
	{
		const vec3 eye( 4, 4, 4 );
		const vec3 center( 1, 1, 1 );
		const vec3 up( 0, 1, 0 );
		mat4 l( look_at( eye, center, up ) );
		print( "look at: ooe", l );

		glLoadIdentity();
		gluLookAt( eye.x, eye.y, eye.z, center.x, center.y, center.z, up.x, up.y, up.z );
		mat4 c;
		glGetFloatv( GL_PROJECTION_MATRIX, c[ 0 ] );
		print( "look_at: gl", c );

		compare( l, c );

	}

	void test( void )
	{
		multiply();
		translate();
		scale();
		rotate();
		euler_rotate();
		rotate_translate();
		translate_rotate();
		perspective();
		orthographic();
		look_at();

		mat4 a( make() );
		mat3 w( a );
		mat4 u( w );
		vec3 v( 8, 4, 2 );
		print( "mat3 * vec3", w * v );
		print( "mat4 * vec3", u * v );

		print( "normal mat3", w );
		print( "transpose mat3", transpose( w ) );

		print( "normal mat4", a );
		print( "transpose mat4", transpose( a ) );

		quat q( euler_angle< quat >( degree( 30 ), degree( 60 ), degree( 90 ) ) );
		f32 x = static_cast< f32 >( std::rand() % 32 );
		f32 y = static_cast< f32 >( std::rand() % 32 );
		f32 z = static_cast< f32 >( std::rand() % 32 );
		vec3 vec( x, y, z );

		print( "camera short", translate( to_matrix( q ), vec ) );

		mat3 rot( to_matrix( q ) );
		vec3 trans( rot * vec );
		mat4 mat( rot );
		column( mat[ 3 ], trans );

		print( "camera long", mat );
	}

	struct interface
	{
		virtual ~interface( void )
		{
		}

		virtual void call( void ) = 0;
	};

	struct concrete
		: public interface
	{
		u32 value;

		concrete( void )
			: value( 0 )
		{
		}

		virtual ~concrete( void )
		{
		}

		virtual void call( void )
		{
			++value;
		}
	};

	void call_test( void )
	{
		concrete stack;
		interface& reference = stack;
		typedef void ( interface::* member_type )( void );
		member_type member = static_cast< member_type >( &concrete::call );
		timer timer;

		for ( up_t i = 0; i != 65536; ++i )
			reference.call();

		std::cout << "virtual call took " << timer.elapsed() << '\n';
		stack.value = 0;
		timer.elapsed();

		for ( up_t i = 0; i != 65536; ++i )
			( reference.*member )();

		std::cout << "pointer call took " << timer.elapsed() << '\n';
	}

	bool launch( const std::string& root, const std::string&, s32, c8** )
	{
		library library( root + "library/libopengl" OOE_EXTENSION, library::global_lazy );
		event_queue queue;
		view view( queue, 640, 480, false );

		typedef tuple< void*, void ( * )( void* ) > close_type;
		typedef close_type ( * open_type )( const view_data& );
		open_type open = reinterpret_cast< open_type >( library.find( "ooe_open" ).function );
		close_type close = open( view );
		guard< void ( void* ) > guard_video( close._1, close._0 );

		test();
		call_test();
		return true;
	}
}

//--- main ---------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
