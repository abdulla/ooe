/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <png.h>

#include <csetjmp>	// png header checks for prior inclusion

#include "foundation/image/png.hpp"
#include "foundation/io/file.hpp"
#include "foundation/io/memory.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/scoped.hpp"

namespace
{
	using namespace ooe;

	typedef tuple< const u8*, up_t > tuple_type;

	void OOE_NORETURN png_error_( png_struct* png_struct, const c8* string )
	{
		*static_cast< std::string* >( png_get_error_ptr( png_struct ) ) = string;
		std::longjmp( png_jmpbuf( png_struct ), 1 );
	}

	void png_warning_( png_struct*, const c8* string )
	{
		OOE_WARNING( "png", "Warning: " << string );
	}

	void png_read( png_struct* read_struct, u8* buffer, up_t size )
	{
		tuple_type& tuple = *static_cast< tuple_type* >( png_get_io_ptr( read_struct ) );
		up_t min = std::min( size, tuple._1 );
		std::memcpy( buffer, tuple._0, min );

		tuple._0 += min;
		tuple._1 -= min;
	}

	void png_write( png_struct* write_struct, u8* buffer, up_t size )
	{
		static_cast< ooe::file* >( png_get_io_ptr( write_struct ) )->write( buffer, size );
	}

	void png_sync( png_struct* write_struct )
	{
		static_cast< ooe::file* >( png_get_io_ptr( write_struct ) )->sync();
	}

	image::type png_image_type( u8 type )
	{
		switch ( type )
		{
		case PNG_COLOR_TYPE_GRAY:
			return image::y_u8;

		case PNG_COLOR_TYPE_GRAY_ALPHA:
			return image::ya_u8;

		case PNG_COLOR_TYPE_RGB:
			return image::rgb_u8;

		case PNG_COLOR_TYPE_RGB_ALPHA:
			return image::rgba_u8;

		default:
			throw error::runtime( "png: " ) << "Unsupported colour type";
		}
	}

	u8 png_colour_type( png_struct* write_struct, u8 type )
	{
		switch ( type )
		{
		case image::y_u8:
			return PNG_COLOR_TYPE_GRAY;

		case image::ya_u8:
			return PNG_COLOR_TYPE_GRAY_ALPHA;

		case image::bgr_u8:
			png_set_bgr( write_struct );
		case image::rgb_u8:
			return PNG_COLOR_TYPE_RGB;

		case image::bgra_u8:
			png_set_bgr( write_struct );
		case image::rgba_u8:
			return PNG_COLOR_TYPE_RGB_ALPHA;

		default:
			throw error::runtime( "png: " ) << "Unsupported image format";
		}
	}
}

namespace ooe
{
//--- png ----------------------------------------------------------------------
	uncompressed_image png::decode( const descriptor& desc )
	{
		memory memory( desc );
		return png::decode( memory.get(), memory.size() );
	}

	uncompressed_image png::decode( const void* data, up_t size )
	{
		std::string string;
		png_struct* read_struct =
			png_create_read_struct( PNG_LIBPNG_VER_STRING, &string, png_error_, png_warning_ );

		if ( !read_struct )
			throw error::runtime( "png: " ) << "Unable to create read structure";

		scoped< void ( png_struct**, png_info**, png_info** ) >
			scoped_read( png_destroy_read_struct, &read_struct, 0, 0 );
		scoped< void ( uncompressed_image* ) > scoped_image;

		if ( setjmp( png_jmpbuf( read_struct ) ) )
			throw error::runtime( "png: " ) << "Error: " << string;

		png_info* info_struct = png_create_info_struct( read_struct );

		if ( !info_struct )
			throw error::runtime( "png: " ) << "Unable to create info structure";

		tuple_type tuple( static_cast< const u8* >( data ), size );
		scoped_read.assign( png_destroy_read_struct, &read_struct, &info_struct, 0 );
		png_set_read_fn( read_struct, &tuple, png_read );
		png_read_info( read_struct, info_struct );

		png_set_expand( read_struct );
		png_set_packing( read_struct );
		png_read_update_info( read_struct, info_struct );

		if ( png_get_bit_depth( read_struct, info_struct ) != 8 )
			throw error::runtime( "png: " ) << "Unsupported bit depth";

		uncompressed_image image( png_get_image_width( read_struct, info_struct ),
			png_get_image_height( read_struct, info_struct ),
			png_image_type( png_get_color_type( read_struct, info_struct ) ) );
		scoped_image.assign( destruct< uncompressed_image >, &image );

		png_destroy_info_struct( read_struct, &info_struct );
		scoped_read.assign( png_destroy_read_struct, &read_struct, 0, 0 );

		u8* pointer = image.as< u8 >();
		up_t row_size = image.row_size();

		for ( u32 i = 0; i != image.height; ++i, pointer += row_size )
			png_read_row( read_struct, pointer, 0 );

		png_read_end( read_struct, 0 );
		scoped_image.clear();
		return image;
	}

	void png::encode( const uncompressed_image& image, const descriptor& desc )
	{
		std::string string;
		png_struct* write_struct =
			png_create_write_struct( PNG_LIBPNG_VER_STRING, &string, png_error_, png_warning_ );

		if ( !write_struct )
			throw error::runtime( "png: " ) << "Unable to create write structure";

		scoped< void ( png_struct**, png_info** ) >
			scoped( png_destroy_write_struct, &write_struct, 0 );
		file file( desc );

		if ( setjmp( png_jmpbuf( write_struct ) ) )
			throw error::runtime( "png: " ) << "Error: " << string;

		png_info* info_struct = png_create_info_struct( write_struct );

		if ( !info_struct )
			throw error::runtime( "png: " ) << "Unable to create info structure";

		scoped.assign( png_destroy_write_struct, &write_struct, &info_struct );
		u8 colour_type = png_colour_type( write_struct, image.format );
		png_set_IHDR( write_struct, info_struct, image.width, image.height, 8, colour_type,
			PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT );

		png_set_write_fn( write_struct, &file, png_write, png_sync );
		png_write_info( write_struct, info_struct );
		png_destroy_info_struct( write_struct, &info_struct );
		scoped.assign( png_destroy_write_struct, &write_struct, 0 );

		png_set_compression_level( write_struct, Z_BEST_COMPRESSION );
		u8* pointer = image.as< u8 >();
		up_t row_size = image.row_size();

		for ( u32 i = 0; i != image.height; ++i, pointer += row_size )
			png_write_row( write_struct, pointer );

		png_write_end( write_struct, 0 );
	}
}
