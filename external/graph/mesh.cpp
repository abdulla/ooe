/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/lua/table.hpp"
#include "foundation/lua/type.hpp"
#include "foundation/lua/vm.hpp"

#include "external/graph/mesh.hpp"

namespace
{
	using namespace ooe;

	buffer_type buffer_f( const video& video, buffer::type type )
	{
		return video.buffer( type );
	}

	void load_f( const buffer_type& buffer, const lua::binary_string& binary )
	{
		buffer->bind();
		buffer->load( binary.string, binary.size, buffer::static_draw );
	}

	template< typename type >
		s32 convert_f( lua::state* state )
	{
		lua::stack stack( state );
		lua::table table = lua::traits< lua::table >::to( stack, 1 );

		up_t size = table.size();
		scoped_array< type > string( new type[ size ] );
		lua::binary_string binary( string, size * sizeof( type ) );

		for ( up_t i = 0; i != size; ++i )
			string[ i ] = table[ i + 1 ];

		lua::traits< lua::binary_string >::push( stack, binary );
		return 1;
	}

	video_data process_f( const lua::table& table )
	{
		const lua::table layout = table[ "layout" ];

		video_data data;
		data.mode = table[ "mode" ];
		data.length = table[ "length" ];
		data.size = layout.size();
		data.layout = new video_data::layout_type[ data.size ];
		data.point = table[ "point" ];

		if ( table[ "index" ].type() == lua::id::userdata )
			data.index = table[ "index" ];

		for ( u32 i = 0; i != data.size; ++i )
			data.layout[ i ] = layout[ i + 1 ];

		return data;
	}
}

namespace ooe
{
//--- mesh ---------------------------------------------------------------------
	mesh::mesh( const descriptor& desc, const video& vid )
		: data()
	{
		lua::vm vm;
		lua::stack stack = vm.stack();

		lua::type< buffer_type >( stack )
			.insert( "load", load_f );

		lua::type< video >( stack )
			.insert( "buffer", buffer_f )
			.insert( "point", buffer::point )
			.insert( "index", buffer::index )
			.insert( "pixel_pack", buffer::pixel_pack )
			.insert( "pixel_unpack", buffer::pixel_unpack )

			.insert( "vertex", video_data::vertex_3 )
			.insert( "normal", video_data::normal )
			.insert( "colour", video_data::colour_4 )
			.insert( "texture", video_data::texture )
			.insert( "index_16", video_data::index_16 )
			.insert( "index_32", video_data::index_32 )

			.insert( "point_list", video_data::point_list )
			.insert( "line_list", video_data::line_list )
			.insert( "line_strip", video_data::line_strip )
			.insert( "triangle_list", video_data::triangle_list )
			.insert( "triangle_strip", video_data::triangle_strip )
			.insert( "triangle_fan", video_data::triangle_fan )
			.insert( "quad_list", video_data::quad_list );
			
		lua::table global( stack, lua::global );
		global[ "f32" ] = &convert_f< f32 >;
		global[ "u32" ] = &convert_f< u32 >;
		global[ "u16" ] = &convert_f< u16 >;

		vm.load( "mesh", desc );
		lua::function< lua::table ( const video& ) > load = global[ "load" ];
		data = process_f( load( vid ) );
	}

	const video_data& mesh::get( void ) const
	{
		return data;
	}
}
