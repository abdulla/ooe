/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "external/scene/cache.hpp"
#include "external/scene/chunked.hpp"
#include "external/scene/scheduler.hpp"
#include "foundation/executable/environment.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/executable/timer.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/video.hpp"
#include "foundation/visual/view.hpp"

namespace
{
	using namespace ooe;
	const u16 width = 1024;
	const u16 height = 640;

	void setup_video( video& video )
	{
		video.sync( true );
		video.clear();
		video.viewport( width, height );
		video.depth( video::less_equal );

		video.enable( video::blend_mode, false );
		video.enable( video::face_cull, false );
		video.enable( video::depth_test );
	}

	void setup_vfs( vfs& vfs, const std::string& root )
	{
		vfs.insert( root + "../resource", "/" );
		vfs.insert( root + "../resource/glsl", "shader" );
		vfs.insert( root + "../../data/show", "/" );
		vfs.insert( root + "../../data/chunked", "chunked" );
	}

	void setup_camera( camera& camera, const aabb& box )
	{
		vec3 center = aabb_center( box );
		vec3 viewer = vec3( -center.x, -center.y - aabb_extent( box ).y - 10, -center.z );
		camera.rotate( vec3( 0, maths::pi_half, 0 ) );
		camera.translate( viewer );
		camera.rotate( vec3( 0, maths::pi_half, 0 ) );
	}

	void setup_frame( video& video, chunked& chunked, render_queue& render_queue,
		camera& camera, vec3& rotate, vec3& translate )
	{
		camera.rotate( rotate * radian( degree( .15 ) ) );
		camera.translate( translate );
		camera_instance instance( camera );

		chunked.includes( instance, render_queue );
		render_queue.draw( instance, video );
		render_queue.clear();

		rotate = vec3::zero;
		translate = vec3::zero;
	}

	void setup_key( u32 value, bool press, vec3& translate, up_t& speed )
	{
		if ( !press )
			return;

		switch ( value )
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
			break;

		case 'q':
			executable::quit();

		default:
			break;
		}
	}

	void setup_input( event_queue& event_queue, vec3& rotate, vec3& translate, up_t& speed )
	{
		event event;

		for ( event::type type; ( type = event_queue.next_event( event ) ); )
		{
			switch ( type )
			{
			case event::motion_flag:
				rotate.y += event.motion.x;
				rotate.z += event.motion.y;
				break;

			case event::button_flag:
				break;

			case event::key_flag:
				setup_key( event.key.value, event.key.press, translate, speed );
				break;

			case event::exit_flag:
				executable::quit();
				break;

			default:
				break;
			}
		}
	}

	bool launch( const std::string& root, const std::string&, s32, c8** )
	{
		// video library must be preloaded for linux
		library libopengl( root + "../library/libopengl" + library::suffix, library::global_lazy );
		event_queue event_queue;
		view view( event_queue, width, height, false );

		typedef ooe::video* ( video_function )( const view_data& );
		scoped_ptr< ooe::video > video( libopengl.find< video_function >( "video_open" )( view ) );
		setup_video( *video );

		vfs vfs;
		setup_vfs( vfs, root );

		scheduler scheduler;
		cache cache( vfs, *video );
		chunked chunked( "chunked/puget", scheduler, cache, degree( 45 ), width, 2.5 );

		camera camera( degree( 45 ), width / static_cast< f32 >( height ) );
		setup_camera( camera, chunked.aabb() );

		vec3 rotate = vec3::zero;
		vec3 translate = vec3::zero;
		up_t speed = 1;
		render_queue render_queue;

		timer timer;

		while ( !executable::has_signal() )
		{
			video->clear();
			setup_frame( *video, chunked, render_queue, camera, rotate, translate );
			executable::yield();
			video->swap();

			setup_input( event_queue, rotate, translate, speed );
		}

		return true;
	}
}

//--- main ---------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
