/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "component/ui/text_layout.hpp"
#include "foundation/utility/arithmetic.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const up_t index_size = 6;
const up_t point_size = 4 * 4;

void add_glyph( const font_source::glyph_type& glyph, f32* value, u32 size )
{
	const font::metric& metric = glyph._2;

	f32 x_min = 0;
	f32 x_max = metric.width;
	f32 y_min = 0;
	f32 y_max = metric.height;

	std::cout << "x_min: " << x_min << '\n';
	std::cout << "x_max: " << x_max << '\n';
	std::cout << "y_min: " << y_min << '\n';
	std::cout << "y_max: " << y_max << '\n';

	f32 u_min = divide( glyph._0, size );
	f32 u_max = divide( glyph._0 + metric.width, size );
	f32 v_min = divide( glyph._1, size );
	f32 v_max = divide( glyph._1 + metric.height, size );

	std::cout << "u_min: " << u_min << '\n';
	std::cout << "u_max: " << u_max << '\n';
	std::cout << "v_min: " << v_min << '\n';
	std::cout << "v_max: " << v_max << '\n';

	// top left
	value[ 0 ] = x_min;
	value[ 1 ] = y_max;
	value[ 2 ] = u_min;
	value[ 3 ] = v_max;

	// bottom left
	value[ 4 ] = x_min;
	value[ 5 ] = y_min;
	value[ 6 ] = u_min;
	value[ 7 ] = v_min;

	// top right
	value[ 8 ] = x_max;
	value[ 9 ] = y_max;
	value[ 10 ] = u_max;
	value[ 11 ] = v_max;

	// bottom right
	value[ 12 ] = x_max;
	value[ 13 ] = y_min;
	value[ 14 ] = u_max;
	value[ 15 ] = v_min;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- text_layout ----------------------------------------------------------------------------------
text_layout::text_layout( const device_type& device_, const font_source& source_ )
	: device( device_ ), source( source_ )
{
}

block_type text_layout::block( const program_type& program, const std::string& text, u8 level )
{
	buffer_type index = device->buffer( sizeof( u16 ) * index_size * text.size(), buffer::index );
	{
		map_type map = index->map( buffer::write );
		u16* value = static_cast< u16* >( map->data );

		for ( up_t i = 0, end = text.size() * 4; i != end; i += 4, value += index_size )
		{
			value[ 0 ] = 0 + i;
			value[ 1 ] = 1 + i;
			value[ 2 ] = 2 + i;
			value[ 3 ] = 2 + i;
			value[ 4 ] = 1 + i;
			value[ 5 ] = 3 + i;
		}
	}

	buffer_type point = device->buffer( sizeof( f32 ) * point_size * text.size(), buffer::point );
	{
		map_type map = point->map( buffer::write );
		f32* value = static_cast< f32* >( map->data );
		u32 size = source.size();

		for ( std::string::const_iterator i = text.begin(), end = text.end(); i != end;
			++i, value += point_size )
		{
			std::cout << "processing char: " << *i << '\n';
			add_glyph( source.glyph( *i, level ), value, size );
		}
	}

	block_type b = program->block( index );
	b->input( "vertex", 2, point );
	b->input( "coords", 2, point );
	return b;
}

OOE_NAMESPACE_END( ( ooe ) )
