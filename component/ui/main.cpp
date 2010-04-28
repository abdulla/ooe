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
void process_key( u32 value, bool press, vec3& translate, vec3& scale, std::string& string )
{
	if ( !press )
		return;

	switch ( value )
	{
	case key_left:
		translate.x += acceleration;
		break;

	case key_right:
		translate.x -= acceleration;
		break;

	case key_up:
		translate.y += acceleration;
		break;

	case key_down:
		translate.y -= acceleration;
		break;

	case '=':
		scale.x = scale.y += acceleration;
		break;

	case '-':
		scale.x = scale.y -= acceleration;
		break;

	case '.':
		acceleration = acceleration ? acceleration << 1 : 1;
		break;

	case ',':
		acceleration = acceleration ? acceleration >> 1 : 1;
		break;

	case key_escape:
		executable::quit();
		break;

	default:
		string += value;
		break;
	}
}

//--- process_events -------------------------------------------------------------------------------
std::string process_events
	( event_queue& event_queue, vec3& translate, vec3& scale, epoch_t timeout )
{
	std::string string;
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
			process_key( event.key.value, event.key.press, translate, scale, string );
			break;

		case event::exit:
			executable::quit();
			break;

		default:
			break;
		}
	}

	return string;
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
	font::face font_face( font_library, root + "../resource/font/times-serif.ttf" );
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

	u32 source_size = source.size();
	u16 page_size = source.page_size();
	u32 font_size = source.font_size();
	vt.load( 0, 0, source_size, font_size, log2( source_size / page_size ) - 1 );
	vt.write();

	std::string string;
	text_layout layout( device, vt, source );
	block_type text;

	frame_type frame = device->default_frame( width, height );
	vec3 translate( width / 2, height / 2, 0 );
	vec3 scale( page_size, page_size, 1 );

	while ( !executable::has_signal() )
	{
		block->input( "model_view",
			ooe::translate( mat4::identity, translate ) * ooe::scale( mat4::identity, scale ) );

		frame->clear();
		device->draw( block, frame );

		if ( text )
			device->draw( text, frame );

		device->swap();

		up_t pending = vt.pending();
		std::string suffix =
			process_events( event_queue, translate, scale, epoch_t( pending ? 0 : 3600, 0 ) );

		if ( !suffix.empty() )
		{
			string += suffix;
			text = layout.block( program, string, 4 );
			vt.input( "vt", text );
			text->input( "projection", orthographic( 0, width, height, 0 ) );
			text->input( "model_view", mat4::identity );
		}

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
