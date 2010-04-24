/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cmath>

#include "component/ui/font_source.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/binary.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const image::type image_type = image::y_u8;
const u16 page_width = 256;

u32 get_width( const font::face& face, u32 font_size )
{
	u32 root = std::sqrt( face.number( font::face::glyphs ) );
	return bit_round_up( root ) * font_size;
}

void copy_partial( uncompressed_image& image, const font::bitmap& bitmap, u32 x, u32 y )
{
	if ( x >= bitmap.metric.width || y >= bitmap.metric.height )
		return;

	u8* target = image.as< u8 >();
	const u8* source = bitmap.data + x + y * bitmap.metric.width;

	u32 width = std::min< u32 >( page_width, bitmap.metric.width - x );
	u32 height = std::min< u32 >( page_width, bitmap.metric.height - y );

	for ( u32 i = 0; i != height; ++i, target += image.width, source += bitmap.metric.width )
		std::memcpy( target, source, width );
}

void copy_square( uncompressed_image& image, const font::bitmap& bitmap, u32 x, u32 y )
{
	u8* target = image.as< u8 >() + x + y * image.width;
	const u8* source = bitmap.data;

	for ( u32 i = 0; i != bitmap.metric.height;
		++i, target += image.width, source += bitmap.metric.width )
		std::memcpy( target, source, bitmap.metric.width );
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- font_source ----------------------------------------------------------------------------------
font_source::font_source( font::face& face_, u32 font_size_ )
	: mutex(), face( face_ ), font_size( font_size_ ), width( get_width( face, font_size ) ),
	first( face.number( font::face::first ) ), glyphs( face.number( font::face::glyphs ) )
{
}

font_source::~font_source( void )
{
}

u32 font_source::size( void ) const
{
	return width;
}

image::type font_source::format( void ) const
{
	return image_type;
}

u16 font_source::page_size( void ) const
{
	return page_width;
}

font_source::glyph_type font_source::glyph( up_t char_code ) const
{
	char_code = std::min< up_t >( 0, char_code - first );
	return glyph_type( ( char_code / width ) * font_size, ( char_code % width ) * font_size );
}

image font_source::read( u32 x, u32 y, u8 level )
{
	uncompressed_image image( page_width, page_width, image_type );
	std::memset( image.get(), 0, image.byte_size() );

	u32 resize = font_size >> level;
	u32 glyphs_per_row = width / font_size;
	u32 glyphs_per_page = page_width / resize;
	up_t char_code = ( x + y * glyphs_per_row / resize ) * page_width / resize + first;

	if ( char_code >= glyphs + first )
		return image;

	if ( resize >= page_width )
	{
		u32 pages_per_glyph = resize / page_width;
		u32 x_offset = ( x % pages_per_glyph ) * page_width;
		u32 y_offset = ( y % pages_per_glyph ) * page_width;

		lock lock( mutex );
		font::bitmap bitmap = face.character( char_code, resize );
		copy_partial( image, bitmap, x_offset, y_offset );
		return image;
	}

	for ( u32 j = 0; j != glyphs_per_page; ++j )
	{
		for ( u32 i = 0; i != glyphs_per_page; ++i )
		{
			u32 code = char_code + i + j * glyphs_per_row;

			if ( code >= glyphs + first )
				return image;

			lock lock( mutex );
			font::bitmap bitmap = face.character( code, resize );
			copy_square( image, bitmap, i * resize, j * resize );
		}
	}

	return image;
}

OOE_NAMESPACE_END( ( ooe ) )
