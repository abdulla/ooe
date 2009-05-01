/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <cstdlib>

#include "foundation/image/image.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
//--- image --------------------------------------------------------------------
	image::image( u32 width_, u32 height_, u8 format_, function_type byte_size )
		: width( width_ ), height( height_ ), format( format_ ),
		data( std::malloc( ( this->*byte_size )() ) )
	{
	}

//--- uncompressed_image -------------------------------------------------------
	uncompressed_image::uncompressed_image( u32 width_, u32 height_, type format_ )
		: image( width_, height_, format_,
		reinterpret_cast< function_type >( &uncompressed_image::byte_size ) )
	{
	}

	uncompressed_image::uncompressed_image( const image& image_ )
		: image( image_ )
	{
		if ( image_.format > uncompressed )
			throw error::runtime( "uncompressed_image: " ) << "Image is compressed";
	}

	u8 uncompressed_image::channels( void ) const
	{
		switch ( format )
		{
		case a_u8:
		case a_f16:
		case a_f32:
		case y_u8:
		case y_f16:
		case y_f32:
			return 1;

		case ya_u8:
		case ya_f16:
		case ya_f32:
			return 2;

		case bgr_u8:
		case rgb_u8:
		case rgb_f16:
		case rgb_f32:
			return 3;

		case bgra_u8:
		case rgba_u8:
		case rgba_f16:
		case rgba_f32:
			return 4;

		default:
			throw error::runtime( "uncompressed_image: " ) << "Unknown image type";
		}
	}

	u8 uncompressed_image::channel_size( void ) const
	{
		switch ( format )
		{
		case bgr_u8:
		case bgra_u8:
		case rgb_u8:
		case rgba_u8:
		case a_u8:
		case y_u8:
		case ya_u8:
			return 1;

		case rgb_f16:
		case rgba_f16:
		case a_f16:
		case y_f16:
		case ya_f16:
			return 2;

		case rgb_f32:
		case rgba_f32:
		case a_f32:
		case y_f32:
		case ya_f32:
			return 4;

		default:
			throw error::runtime( "uncompressed_image: " ) << "Unknown image type";
		}
	}

	u8 uncompressed_image::pixel_size( void ) const
	{
		return channels() * channel_size();
	}

	up_t uncompressed_image::row_size( void ) const
	{
		return width * pixel_size();
	}

	up_t uncompressed_image::byte_size( void ) const
	{
		return height * row_size();
	}

//--- compressed_image ---------------------------------------------------------
	compressed_image::compressed_image( u32 width_, u32 height_, type format_ )
		: image( width_, height_, format_,
		reinterpret_cast< function_type >( &compressed_image::byte_size ) )
	{
	}

	compressed_image::compressed_image( const image& image_ )
		: image( image_ )
	{
		if ( image_.format < compressed )
			throw error::runtime( "compressed_image: " ) << "Image is uncompressed";
	}

	u8 compressed_image::channels( void ) const
	{
		switch ( format )
		{
		case rgba_dxt1:
		case rgba_dxt3:
		case rgba_dxt5:
			return 4;

		default:
			throw error::runtime( "compressed_image: " ) << "Unknown image type";
		}
	}

	u8 compressed_image::block_size( void ) const
	{
		switch( format )
		{
		case rgba_dxt1:
			return 8;

		case rgba_dxt3:
		case rgba_dxt5:
			return 16;

		default:
			throw error::runtime( "compressed_image: " ) << "Unknown image type";
		}
	}

	up_t compressed_image::byte_size( void ) const
	{
		return ( ( width + 3 ) / 4 ) * ( ( height + 3 ) / 4 ) * block_size();
	}
}
