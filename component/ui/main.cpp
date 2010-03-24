/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/io/memory.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/graphics.hpp"
#include "foundation/visual/view.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const f32 width = 1024;
const f32 height = 640;

//--- process_events -------------------------------------------------------------------------------
void process_events( event_queue& event_queue )
{
	event event;

	for ( event::type type; ( type = event_queue.next_event( event, false ) ); )
	{
		switch ( type )
		{
		case event::motion_flag:
		case event::button_flag:
			break;

		case event::key_flag:
			if ( event.key.value == 'q' )
				executable::quit();

			break;

		case event::exit_flag:
			executable::quit();
			break;

		default:
			break;
		}
	}
}

std::string source( const std::string root, const std::string name )
{
	memory memory( descriptor( root + "../resource/glsl/" + name ) );
	return std::string( memory.as< c8 >(), memory.size() );
}

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string& root, const std::string&, s32, c8** )
{
	// graphics library must be preloaded for linux
	library library( root + "../library/libopengl_next" + library::suffix, library::global_lazy );

	event_queue event_queue;
	view view( event_queue, width, height, false );
	device_type device = library.find< device_type ( const view_data& ) >( "device_open" )( view );

	shader_type vertex = device->shader( source( root, "null.vs" ), shader::vertex );
	shader_type fragment = device->shader( source( root, "null.fs" ), shader::fragment );
	shader_vector vector;
	vector.push_back( vertex );
	vector.push_back( fragment );
	program_type program = device->program( vector );

	buffer_type point = device->buffer( sizeof( f32 ) * 4 * 2, buffer::point );
	{
		map_type map = point->map( buffer::write );
		f32* value = static_cast< f32* >( map->data );

		// top left
		value[ 0 ] = -1;
		value[ 1 ] = 1;

		// bottom left
		value[ 2 ] = -1;
		value[ 3 ] = -1;

		// top right
		value[ 4 ] = 1;
		value[ 5 ] = 1;

		// bottom right
		value[ 6 ] = 1;
		value[ 7 ] = -1;
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

	block_type block = program->block( index );
	block->input( "vertex", 2, point );
	block->input( "projection", perspective( degree( 45 ), width / height, 1, 100 ) );

	while ( !executable::has_signal() )
	{
		executable::yield();
		device->draw( block );
		device->swap();

		process_events( event_queue );
	}

	return true;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
