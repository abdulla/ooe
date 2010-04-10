/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/ui/font_source.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- font_source ----------------------------------------------------------------------------------
font_source::font_source( font::face& face_, u32 font_size )
	// incorrect, but temporary, size calculation
	: face( face_ ), size_( face.number( font::face::glyphs ) * font_size )
{
}

font_source::~font_source( void )
{
}

image::type font_source::format( void ) const
{
	return image::a_u8;
}

u32 font_source::size( void ) const
{
	return size_;
}

u16 font_source::page_size( void ) const
{
	return 256;
}

image font_source::read( u32, u32, u8 )
{
	return uncompressed_image( 256, 256, image::a_u8 );
}

OOE_NAMESPACE_END( ( ooe ) )
