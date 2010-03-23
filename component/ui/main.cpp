/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/io/memory.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/graphics.hpp"
#include "foundation/visual/view.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const u16 width = 1024;
const u16 height = 640;

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

	while ( !executable::has_signal() )
	{
		executable::yield();
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
