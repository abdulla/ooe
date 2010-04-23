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

void copy_square( uncompressed_image& image, const font::bitmap& bitmap, u32 x, u32 y )
{
	u8* target_end = image.as< u8 >() + image.byte_size();
	const u8* source_end = bitmap.data + bitmap.metric.width * bitmap.metric.height;

	u8* target = image.as< u8 >() + x + y * image.width;
	const u8* source = bitmap.data;

	for ( u32 i = 0; i != bitmap.metric.height;
		++i, target += image.width, source += bitmap.metric.width )
	{
		if ( target + bitmap.metric.width > target_end )
		{
			std::cout << "target > target_end: " << i << " - " << x << ", " << y << " - " <<
				bitmap.metric.height << '\n';
			break;
		}

		if ( source + bitmap.metric.width > source_end )
		{
			std::cout << "source > source_end: " << i << " - " << x << ", " << y << " - " <<
				bitmap.metric.height << '\n';
			break;
		}

		std::memcpy( target, source, bitmap.metric.width );
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

image font_source::read( u32 x, u32 y, u8 level )
{
	uncompressed_image image( page_width, page_width, image_type );
	std::memset( image.get(), 0, image.byte_size() );

	u32 resize = font_size >> level;
	u32 glyphs_per_row = width / font_size;
	u32 glyphs_per_page = page_width / resize;
	up_t char_code = x * glyphs_per_page + y * glyphs_per_row;

	if ( char_code > face.number( font::face::glyphs ) )
	{
		std::cout << "glyph " << char_code << " out of range " << face.number( font::face::glyphs )
			<< '\n';
		return image;
	}

	if ( resize >= page_width )
	{
		lock lock( mutex );
		std::cout << "ignoring larger glyph (" << resize << ", " << page_width << ")\n";
		return image;
	}

	for ( u32 j = 0; j < resize; ++j )
	{
		for ( u32 i = 0; i < resize; ++i )
		{
			lock lock( mutex );
			u32 code = char_code + i + j * glyphs_per_row;
			std::cout << "attempting to render character code (level " << u32( level ) << "): " <<
				code << " of " << face.number( font::face::glyphs ) << '\n';
			font::bitmap bitmap = face.character( code, resize );
			copy_square( image, bitmap, i * glyphs_per_page, j * glyphs_per_page );
		}
	}

	return image;
}

OOE_NAMESPACE_END( ( ooe ) )
