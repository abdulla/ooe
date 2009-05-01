/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/program.hpp"
#include "foundation/scene/chunked.hpp"

#include "external/engine/dynamic.hpp"
#include "external/engine/interface.hpp"
#include "external/service/input.hpp"
#include "external/service/service.hpp"

namespace
{
	using namespace ooe;

//--- slots --------------------------------------------------------------------
	struct slots
	{
		frame_slot frame;

		motion_slot motion;
		button_slot button;
		key_slot key;
	};

//--- driver -------------------------------------------------------------------
	class driver
	{
	public:
		driver( core_type&, const settings&, service& );

		void frame( f32 );

		void motion( s32, s32 );
		void button( u32, bool );
		void key( u32, bool );

	private:
		slots slot;
		ooe::cache cache;

		vec3 rotate;
		vec3 translate;
		u32 speed;

		ooe::camera camera;
		ooe::render_queue queue;
		ooe::chunked chunked;
	};

	driver::driver( core_type& core, const settings& set, service& service )
		: slot(), cache( core.vfs, core.video ), rotate( vec3::zero ), translate( vec3::zero ),
		speed( 1 ), camera( degree( 45 ), divide( set.view.width, set.view.height ) ), queue(),
		chunked( "chunked/puget", core.schedule, cache, degree( 45 ), set.view.width, 2.5 )
	{
		slot.frame = make_function( *this, &driver::frame );
		slot.motion = make_function( *this, &driver::motion );
		slot.button = make_function( *this, &driver::button );
		slot.key = make_function( *this, &driver::key );

		connect( service[ "frame" ], slot.frame );
		connect( service[ "motion" ], slot.motion );
		connect( service[ "button" ], slot.button );
		connect( service[ "key" ], slot.key );

		aabb box = chunked.aabb();
		vec3 center = aabb_center( box );
		vec3 viewer = vec3( center.x, -center.y - aabb_extent( box ).y - 10, center.z );
		camera.rotate( vec3( 0, maths::pi_half, 0 ) );
		camera.translate( viewer );
		camera.rotate( vec3( 0, maths::pi_half, 0 ) );
	}

	void driver::frame( f32 )
	{
		camera.rotate( rotate * radian( degree( .15f ) ) );
		camera.translate( translate );
		camera_instance instance( camera );

		chunked.includes( instance, queue );
		queue.draw( instance, cache.video );
		queue.clear();

		rotate = vec3::zero;
		translate = vec3::zero;
	}

	void driver::motion( s32 x, s32 y )
	{
		rotate.y -= static_cast< f32 >( x );	// y motion rotates around the x axis
		rotate.z -= static_cast< f32 >( y );	// x motion rotates around the y axis
	}

	void driver::button( u32, bool )
	{
	}

	void driver::key( u32 key_, bool press )
	{
		if ( !press )
		{
			if ( key_ == 'q' )
				executable::quit();

			return;
		}

		switch ( key_ )
		{
		case '.':
			speed = speed ? speed << 1 : 1;
			break;

		case ',':
			speed = speed ? speed >> 1 : 1;
			break;

		case 's':
			translate.x -= static_cast< f32 >( speed );
			break;

		case 'w':
			translate.x += static_cast< f32 >( speed );
			break;

		case ' ':
			translate.y -= static_cast< f32 >( speed );
			break;

		case 'c':
			translate.y += static_cast< f32 >( speed );
			break;

		case 'a':
			translate.z += static_cast< f32 >( speed );
			break;

		case 'd':
			translate.z -= static_cast< f32 >( speed );
		default:
			break;
		}
	}

	void close( void* pointer )
	{
		delete static_cast< driver* >( pointer );
	}
}

extern "C" dynamic::close_type ooe_open( core_type&, const settings&, service& ) OOE_VISIBLE;
extern "C" dynamic::close_type ooe_open( core_type& core, const settings& set, service& service )
{
	return dynamic::close_type( new driver( core, set, service ), close );
}
