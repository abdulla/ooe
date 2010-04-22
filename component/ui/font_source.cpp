/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

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

void write_glyphs( uncompressed_image& image, const font::bitmap& bitmap, u32 i, u32 j, u32 size )
{
	u8* image_end = image.as< u8 >() + image.byte_size();
	const u8* bitmap_end = bitmap.data + bitmap.metric.width * bitmap.metric.height;

	u8* image_ptr = image.as< u8 >() + i * size + j * page_width;
	const u8* bitmap_ptr = bitmap.data;

	for ( u32 y = 0; y != bitmap.metric.height; ++y,
		image_ptr += page_width, bitmap_ptr += bitmap.metric.width )
	{
		if ( image_ptr > image_end )
			std::cout << "image_ptr out of bounds (" << size << "): " << y << " : " <<
				up_t( image_ptr - image_end ) << '\n';

		if ( bitmap_ptr > bitmap_end )
			std::cout << "bitmap_ptr out of bounds (" << size << "): " << y << " : " <<
				up_t( bitmap_ptr - bitmap_end ) << '\n';

		std::memcpy( image_ptr, bitmap_ptr, bitmap.metric.width );
	}
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- font_source ----------------------------------------------------------------------------------
font_source::font_source( font::face& face_, u32 font_size_ )
	: mutex(), face( face_ ), font_size( font_size_ ), width( get_width( face, font_size ) )
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
	return glyph_type( ( char_code / width ) * font_size, ( char_code % width ) * font_size );
}

image font_source::read( u32 /*x*/, u32 /*y*/, u8 /*level*/ )
{
	uncompressed_image image( page_width, page_width, image_type );
/*	u32 resize = font_size >> level;
	u32 glyphs_per_row = width / font_size;
	up_t char_code = x * round_down< u32 >( page_width, font_size ) + y * glyphs_per_row;

	if ( resize >= page_width )
	{
		lock lock( mutex );
		std::cout << "too large, ignorning\n";
//		write_glyph( image, face.character( char_code, resize ), i, j );
		return image;
	}

	u32 end = round_up< u32 >( page_width, font_size );

	for ( u32 j = 0; j != end; ++j )
	{
		for ( u32 i = 0; i != end; ++i )
		{
			lock lock( mutex );
			u32 code = char_code + i + j * glyphs_per_row;
			write_glyphs( image, face.character( code, resize ), i, j, resize );
		}
	}
*/
	return image;
}

OOE_NAMESPACE_END( ( ooe ) )
