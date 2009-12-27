/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <algorithm>

#include <cctype>

#include "foundation/general/text.hpp"
#include "foundation/utility/binary.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/miscellany.hpp"

namespace
{
	using namespace ooe;

	void copy_square( u8* target, const u8* source, u32 width, const u32* xyab )
	{
		target += width * xyab[ 3 ] + xyab[ 2 ];

		for ( u32 i = 0; i != xyab[ 1 ]; ++i )
		{
			std::memcpy( target, source, xyab[ 0 ] );
			target += width;
			source += xyab[ 0 ];
		}
	}

	typedef tuple< u8 /* total */, u8 /* \n */, u8 /* \t */, u8 /* other */ > space_type;

	space_type find_space( const text::iterator& begin, const text::iterator& end )
	{
		u8 new_line = 0;
		u8 tab_space = 0;
		u8 other = 0;
		text::iterator i( begin );

		for ( ; i != end; ++i )
		{
			c8 value = *i;

			if ( !isspace( value ) )
				break;
			if ( value == '\n' )
				++new_line;
			else if ( value == '\t' )
				++tab_space;
			else
				++other;
		}

		u8 total = static_cast< u8 >( std::distance( begin, i ) );
		return space_type( total, new_line, tab_space, other );
	}

	text::iterator find_word( const text::iterator& begin, const text::iterator& end )
	{
		text::iterator i( begin );

		while ( i != end && !isspace( *i ) )
			++i;

		return i;
	}

	template< typename type >
		void add_glyph( const font::metric& glyph, scoped_array< type >& indices,
		scoped_array< f32 >& points, u32& point, u32& index, u32 i_off, u32& x_min, s32 y_base,
		u32 u_off, u32 v_off, f32 width_pixel, f32 height_pixel )
	{
		x_min += glyph.left;

		const u32 x_max = x_min + glyph.width;
		const s32 y_max = y_base + glyph.top;
		const s32 y_min = y_max - glyph.height;
		const f32 u_min = width_pixel * static_cast< f32 >( u_off );
		const f32 u_max = width_pixel * static_cast< f32 >( u_off + glyph.width );
		const f32 v_min = height_pixel * static_cast< f32 >( v_off + glyph.height );
		const f32 v_max = height_pixel * static_cast< f32 >( v_off );

		// indices
		indices[ index++ ] = static_cast< type >( 0 + i_off );
		indices[ index++ ] = static_cast< type >( 1 + i_off );
		indices[ index++ ] = static_cast< type >( 2 + i_off );
		indices[ index++ ] = static_cast< type >( 2 + i_off );
		indices[ index++ ] = static_cast< type >( 1 + i_off );
		indices[ index++ ] = static_cast< type >( 3 + i_off );

		// top left
		points[ point++ ] = static_cast< f32 >( x_min );
		points[ point++ ] = static_cast< f32 >( y_max );
		points[ point++ ] = u_min;
		points[ point++ ] = v_max;

		// bottom left
		points[ point++ ] = static_cast< f32 >( x_min );
		points[ point++ ] = static_cast< f32 >( y_min );
		points[ point++ ] = u_min;
		points[ point++ ] = v_min;

		x_min += glyph.width;

		// top right
		points[ point++ ] = static_cast< f32 >( x_max );
		points[ point++ ] = static_cast< f32 >( y_max );
		points[ point++ ] = u_max;
		points[ point++ ] = v_max;

		// bottom right
		points[ point++ ] = static_cast< f32 >( x_max );
		points[ point++ ] = static_cast< f32 >( y_min );
		points[ point++ ] = u_max;
		points[ point++ ] = v_min;
	}

	template< typename type >
		text::build_type make_mesh( const video& video, const texture_ptr& texture,
		scoped_array< f32 >& points, scoped_array< type >& indices, u32 point, u32 index )
	{
		buffer_ptr point_buffer( video.buffer( buffer::point ) );
		point_buffer->bind();
		point_buffer->load( points, point * sizeof( f32 ), buffer::static_draw );
		delete[] points.release();

		buffer_ptr index_buffer( video.buffer( buffer::index ) );
		index_buffer->bind();
		index_buffer->load( indices, index * sizeof( type ), buffer::static_draw );
		delete[] indices.release();

		text::build_type tuple( texture, video_data() );
		video_data& data = tuple._1;
		data.mode = video_data::triangle_list;
		data.length = index;
		data.size = 3;
		data.layout = new video_data::layout_type[ 3 ];
		data.point = point_buffer;
		data.index = index_buffer;

		data.layout[ 0 ] = video_data::vertex_2;
		data.layout[ 1 ] = video_data::texture;
		data.layout[ 2 ] = sizeof( type ) == 2 ? video_data::index_16 : video_data::index_32;

		return tuple;
	}
}

namespace ooe
{
//--- text ---------------------------------------------------------------------
	text::text( const ooe::video& video_, font::face& face, u8 size_ )
		: video( video_ ), texture( video.texture() ),
		metric( new font::metric[ face.number( font::face::glyphs ) ] ), size( size_ ),
		width( video.maximum( video::textures ) ),
		height( bit_round( face.number( font::face::glyphs ) * size * size / width + 1 ) )
	{
		if ( height > width )
			throw error::runtime( "text: " ) << "Exceeded maximum texture size";

		const u32 blank_size = width * height;
		scoped_array< u8 > blank( new u8[ blank_size ] );
		std::memset( blank, 0, blank_size );

		face.size( size );
		font::bitmap bitmap;
		const u32 columns = width / size;
		u32 xyab[ 4 ];

		for ( u32 i = 0, end = face.number( font::face::glyphs ); i != end; ++i )
		{
			if ( !face.character( bitmap, i ) )
				continue;

			metric[ i ] = bitmap;
			xyab[ 0 ] = bitmap.width;
			xyab[ 1 ] = bitmap.height;
			xyab[ 2 ] = ( i % columns ) * size;
			xyab[ 3 ] = ( i / columns ) * size;
			copy_square( blank, bitmap.pointer, width, xyab );
		}

		texture->bind();
		texture->load( blank, width, height, uncompressed_image::a_u8 );
	}

	text::bound_type text::bound( const iterator& begin, const iterator& end ) const
	{
		s32 x_min = 0;
		s32 x_max = 0;
		s32 y_min = 0;
		s32 y_max = 0;

		for ( iterator i( begin ); i != end; ++i )
		{
			const s32 value = *i;
			const font::metric& current = metric[ value ];

			if ( isspace( value ) )
			{
				x_min += current.x;
				continue;
			}

			x_min += current.left;
			x_max = x_min + current.width;

			const s32 y_min_now = current.top - current.height;
			const s32 y_max_now = current.top;
			y_min = std::min( y_min_now, y_min );
			y_max = std::max( y_max_now, y_max );

			x_min += current.width;
		}

		return bound_type( 0, x_max, y_min, y_max );
	}

	text::build_type text::build( const std::string& string, u32 right ) const
	{
		const u32 string_length = string.size();
		const u32 index_length = string_length * 6;
		const bool bit_16 = index_length < u32( ~u16( 0 ) );

		// 4 points per glyph, 4 floats per point: vertex_2, texture
		scoped_array< f32 > points( new f32[ string_length * 4 * 4 ] );
		scoped_array< u16 > indices_16( bit_16 ? new u16[ index_length ] : 0 );
		scoped_array< u32 > indices_32( bit_16 ? 0 : new u32[ index_length ] );

		const f32 width_pixel = divide( 1, width );
		const f32 height_pixel = divide( 1, height );
		const u32 per_row = width / size;
		u32 index = 0;
		u32 point = 0;
		u32 skip = 0;
		u32 x_min = 0;
		s32 y_min = 0;

		const iterator begin( string.begin() );
		const iterator end( string.end() );

		for ( iterator i( begin ); i != end; )
		{
			space_type space = find_space( i, end );
			u8 total = space._0;
			u8 lines = space._1;
			u8 tabs = space._2;
			u8 other = space._3;
			skip += total;
			i += total;

			if ( lines || tabs )
			{
				x_min = metric[ '\t' ].x * tabs * 2;
				y_min -= size * lines;
			}

			const iterator j( find_word( i, end ) );
			const bound_type boundary( bound( i, j ) );

			if ( boundary._1 + x_min > right )
			{
				if ( x_min != 0 )
				{
					x_min = 0;
					y_min -= size;
				}
			}
			else
				x_min += metric[ ' ' ].x * other;

			for ( ; i != j; ++i )
			{
				const s32 value = *i;
				const font::metric& glyph = metric[ value ];
				const u32 u_off = ( value % per_row ) * size;
				const u32 v_off = ( value / per_row ) * size;
				const u32 i_off = ( std::distance( begin, i ) - skip ) * 4;

				if ( bit_16 )
					add_glyph( glyph, indices_16, points, point, index, i_off, x_min, y_min,
						u_off, v_off, width_pixel, height_pixel );
				else
					add_glyph( glyph, indices_32, points, point, index, i_off, x_min, y_min,
						u_off, v_off, width_pixel, height_pixel );
			}
		}

		if ( bit_16 )
			return make_mesh( video, texture, points, indices_16, point, index );
		else
			return make_mesh( video, texture, points, indices_32, point, index );
	}
}
