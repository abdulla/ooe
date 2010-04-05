/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cstring>

#include "component/ui/font_source.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/maths/camera.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/view.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef tuple< vec3 /* rotate */, vec3 /* translate */ > camera_tuple;
const f32 width = 1024;
const f32 height = 640;

//--- process_motion -------------------------------------------------------------------------------
void process_motion( s32 x, s32 y, vec3& rotate )
{
	rotate.y += x;
	rotate.z += y;
}

//--- process_key ----------------------------------------------------------------------------------
void process_key( u32 value, bool press, vec3& translate )
{
	if ( !press )
		return;

	switch ( value )
	{
	case 's':
		translate.x -= 1;
		break;

	case 'w':
		translate.x += 1;
		break;

	case ' ':
		translate.y -= 1;
		break;

	case 'c':
		translate.y += 1;
		break;

	case 'a':
		translate.z += 1;
		break;

	case 'd':
		translate.z -= 1;
		break;

	case 'q':
		executable::quit();
		break;

	default:
		break;
	}
}

//--- process_events -------------------------------------------------------------------------------
camera_tuple process_events( event_queue& event_queue )
{
	camera_tuple tuple( vec3::zero, vec3::zero );
	event event;
	epoch_t timeout( 3600, 0 );

	for ( event::type type; ( type = event_queue.next_event( event, timeout ) );
		timeout = epoch_t( 0, 0 ) )
	{
		switch ( type )
		{
		case event::motion_flag:
			process_motion( event.motion.x, event.motion.y, tuple._0 );
			break;

		case event::button_flag:
			break;

		case event::key_flag:
			process_key( event.key.value, event.key.press, tuple._1 );
			break;

		case event::exit:
			executable::quit();
			break;

		default:
			break;
		}
	}

	return tuple;
}

shader_type make_shader( const device_type& device, const std::string& root, shader::type type,
	const std::string& name )
{
	return device->shader( name, root + "../resource/glsl/" + name, type );
}

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string& root, const std::string&, s32, c8** )
{
	// graphics library must be preloaded for linux
	library library( root + "../library/libopengl_next" + library::suffix, library::global_lazy );

	event_queue event_queue;
	view view( event_queue, width, height, false );
	device_type device = library.find< device_type ( const view_data& ) >( "device_open" )( view );

	font::library font_library;
	font::face face( font_library, descriptor( root + "../resource/font/vera.ttf" ) );
	font_source source( face, 1024 );
	virtual_texture virtual_texture( device, source, width, height );
	device->set( device::blend, true );

	shader_vector vector;
	vector.push_back( make_shader( device, root, shader::vertex, "null.vs" ) );
	vector.push_back( make_shader( device, root, shader::fragment, "null.fs" ) );
	program_type program = virtual_texture.program( device, vector );

	buffer_type point = device->buffer( sizeof( f32 ) * 4 * 4, buffer::point );
	{
		map_type map = point->map( buffer::write );
		f32* value = static_cast< f32* >( map->data );

		// top left
		value[ 0 ] = -1;
		value[ 1 ] = 1;
		value[ 2 ] = 0;
		value[ 3 ] = 0;

		// bottom left
		value[ 4 ] = -1;
		value[ 5 ] = -1;
		value[ 6 ] = 0;
		value[ 7 ] = 1;

		// top right
		value[ 8 ] = 1;
		value[ 9 ] = 1;
		value[ 10 ] = 1;
		value[ 11 ] = 0;

		// bottom right
		value[ 12 ] = 1;
		value[ 13 ] = -1;
		value[ 14 ] = 1;
		value[ 15 ] = 1;
	}

	buffer_type index = device->buffer( sizeof( u16 ) * 6, buffer::index );
	{
		map_type map = index->map( buffer::write );
		u16* value = static_cast< u16* >( map->data );

		value[ 0 ] = 0;
		value[ 1 ] = 1;
		value[ 2 ] = 2;
		value[ 3 ] = 2;
		value[ 4 ] = 1;
		value[ 5 ] = 3;
	}

	block_type block = virtual_texture.block( program, index );
	block->input( "vertex", 2, point );
	block->input( "coords", 2, point );

	frame_type default_frame = device->default_frame( width, height );
	frame_type frame = virtual_texture.frame( program );

	camera camera( degree( 45 ), width / height, 1, 100 );
	camera.rotate( vec3( 0, maths::pi_half, 0 ) );
	radian sensitivity( degree( .05 ) );

	while ( !executable::has_signal() )
	{
		frame->clear();
		block->input( "projection", camera.matrix() );
		device->draw( block, frame );
		default_frame->write( frame );
		device->swap();

		camera_tuple tuple = process_events( event_queue );
		camera.rotate( tuple._0 * sensitivity );
		camera.translate( tuple._1 );
	}

	return true;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
