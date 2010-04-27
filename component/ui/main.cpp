/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cmath>

#include "component/ui/text_layout.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/view.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const f32 width = 1024;
const f32 height = 640;
u32 acceleration = 4;

//--- process_key ----------------------------------------------------------------------------------
void process_key( u32 value, bool press, vec3& translate, vec3& scale )
{
	if ( !press )
		return;

	switch ( value )
	{
	case 'd':
		translate.x -= acceleration;
		break;

	case 'a':
		translate.x += acceleration;
		break;

	case 's':
		translate.y -= acceleration;
		break;

	case 'w':
		translate.y += acceleration;
		break;

	case 'c':
		scale.x = scale.y -= acceleration;
		break;

	case ' ':
		scale.x = scale.y += acceleration;
		break;

	case '.':
		acceleration = acceleration ? acceleration << 1 : 1;
		break;

	case ',':
		acceleration = acceleration ? acceleration >> 1 : 1;
		break;

	case 'q':
		executable::quit();
		break;

	default:
		break;
	}
}

//--- process_events -------------------------------------------------------------------------------
void process_events( event_queue& event_queue, vec3& translate, vec3& scale, epoch_t timeout )
{
	event event;

	for ( event::type type; ( type = event_queue.next_event( event, timeout ) );
		timeout = epoch_t( 0, 0 ) )
	{
		switch ( type )
		{
		case event::motion_flag:
		case event::button_flag:
			break;

		case event::key_flag:
			process_key( event.key.value, event.key.press, translate, scale );
			break;

		case event::exit:
			executable::quit();
			break;

		default:
			break;
		}
	}
}

shader_type make_shader( const device_type& device, const std::string& root, shader::type type,
	const std::string& name )
{
	return device->shader( name, root + "../resource/glsl/" + name, type );
}

buffer_type point_buffer( const device_type& device )
{
	buffer_type point = device->buffer( sizeof( f32 ) * 4 * 4, buffer::point );
	{
		map_type map = point->map( buffer::write );
		f32* value = static_cast< f32* >( map->data );

		// top left
		value[ 0 ] = -1;
		value[ 1 ] = 1;
		value[ 2 ] = 0;
		value[ 3 ] = 1;

		// bottom left
		value[ 4 ] = -1;
		value[ 5 ] = -1;
		value[ 6 ] = 0;
		value[ 7 ] = 0;

		// top right
		value[ 8 ] = 1;
		value[ 9 ] = 1;
		value[ 10 ] = 1;
		value[ 11 ] = 1;

		// bottom right
		value[ 12 ] = 1;
		value[ 13 ] = -1;
		value[ 14 ] = 1;
		value[ 15 ] = 0;
	}
	return point;
}

buffer_type index_buffer( const device_type& device )
{
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
	return index;
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
	font::face font_face( font_library, root + "../resource/font/vera-sans.ttf" );
	font_source source( font_face, 512, root + "../cache" );
	thread_pool pool;
	virtual_texture vt( device, source, pool );

	shader_vector vector;
	vector.push_back( make_shader( device, root, shader::vertex, "null.vs" ) );
	vector.push_back( make_shader( device, root, shader::fragment, "null.fs" ) );
	vector.push_back( make_shader( device, root, shader::fragment, "virtual_texture.fs" ) );
	program_type program = device->program( vector );

	buffer_type point = point_buffer( device );
	block_type block = program->block( index_buffer( device ) );
	vt.input( "vt", block );
	block->input( "vertex", 2, point );
	block->input( "coords", 2, point );
	block->input( "projection", orthographic( 0, width, height, 0 ) );

	text_layout layout( device, vt, source );
	block_type text = layout.block( program, "stZ", 0 );
	vt.input( "vt", text );
	text->input( "projection", orthographic( 0, width, height, 0 ) );
	text->input( "model_view", mat4::identity );

	u16 page_size = source.page_size();

	for ( u32 size = source.size(), i = size / page_size; i; i >>= 1 )
		vt.load( 0, 0, size, source.font_size() * 2, log2( i ) );

	vt.write();

	frame_type frame = device->default_frame( width, height );
	vec3 translate( width / 2, height / 2, 0 );
	vec3 scale( page_size, page_size, 1 );

	while ( !executable::has_signal() )
	{
		block->input( "model_view",
			ooe::translate( mat4::identity, translate ) * ooe::scale( mat4::identity, scale ) );

		frame->clear();
		device->draw( block, frame );
		device->draw( text, frame );
		device->swap();

		up_t pending = vt.pending();
		process_events( event_queue, translate, scale, epoch_t( pending ? 0 : 3600, 0 ) );

		if ( pending )
			vt.write();
	}

	return true;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
