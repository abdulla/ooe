/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <csetjmp>

#include <jpeglib.h>

#include "foundation/executable/environment.hpp"
#include "foundation/image/jpeg.hpp"
#include "foundation/io/file.hpp"
#include "foundation/io/memory.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/scoped.hpp"

namespace
{
	using namespace ooe;

	typedef u8 buffer_type[ executable::static_page_size ];
	typedef tuple< std::jmp_buf, std::string, const u8*, up_t > read_tuple;
	typedef tuple< std::jmp_buf, std::string, buffer_type, file* > write_tuple;

	//--- error manager functions ----------------------------------------------
	template< typename type >
		void OOE_NORETURN jpeg_error( jpeg_common_struct* common )
	{
		c8 string[ JMSG_LENGTH_MAX ];
		common->err->format_message( common, string );

		type& tuple = *static_cast< type* >( common->client_data );
		tuple._1 << string;
		std::longjmp( tuple._0, 1 );
	}

	void jpeg_warning( jpeg_common_struct* common )
	{
		c8 string[ JMSG_LENGTH_MAX ];
		common->err->format_message( common, string );
		OOE_WARNING( "jpeg", "Warning: " << string );
	}

	//--- source manager functions ---------------------------------------------
	void jpeg_init( jpeg_decompress_struct* read_struct )
	{
		read_tuple& tuple = *static_cast< read_tuple* >( read_struct->client_data );
		read_struct->src->next_input_byte = tuple._2;
		read_struct->src->bytes_in_buffer = tuple._3;
	}

	s32 jpeg_fill( jpeg_decompress_struct* )
	{
		return true;
	}

	void jpeg_skip( jpeg_decompress_struct* read_struct, long size )
	{
		read_struct->src->next_input_byte += size;
		read_struct->src->bytes_in_buffer -= size;
	}

	void jpeg_term( jpeg_decompress_struct* )
	{
	}

	//--- destination manager functions ----------------------------------------
	void jpeg_init( jpeg_compress_struct* write_struct )
	{
		write_struct->dest->next_output_byte =
			static_cast< write_tuple* >( write_struct->client_data )->_2;
		write_struct->dest->free_in_buffer = executable::static_page_size;
	}

	void jpeg_write( jpeg_compress_struct* write_struct, up_t size )
	{
		write_tuple& tuple = *static_cast< write_tuple* >( write_struct->client_data );

		if ( tuple._3->write( tuple._2, size ) != size )
		{
			write_struct->err->msg_code = 0;	// will generate bogus error code
			write_struct->err->
				error_exit( reinterpret_cast< jpeg_common_struct* >( write_struct ) );
		}
	}

	s32 jpeg_empty( jpeg_compress_struct* write_struct )
	{
		jpeg_write( write_struct, executable::static_page_size );
		write_struct->dest->next_output_byte =
			static_cast< write_tuple* >( write_struct->client_data )->_2;
		write_struct->dest->free_in_buffer = executable::static_page_size;
		return true;
	}

	void jpeg_term( jpeg_compress_struct* write_struct )
	{
		jpeg_write
			( write_struct, executable::static_page_size - write_struct->dest->free_in_buffer );
	}

	//--- image format functions -----------------------------------------------
	uncompressed_image::type jpeg_image_type( J_COLOR_SPACE colour_space )
	{
		switch ( colour_space )
		{
		case JCS_GRAYSCALE:
			return uncompressed_image::y_u8;

		case JCS_RGB:
			return uncompressed_image::rgb_u8;

		default:
			throw error::runtime( "jpeg: " ) << "Unsupported colour type";
		}
	}

	J_COLOR_SPACE jpeg_colour_space( u8 type )
	{
		switch ( type )
		{
		case uncompressed_image::y_u8:
			return JCS_GRAYSCALE;

		case uncompressed_image::rgb_u8:
			return JCS_RGB;

		default:
			throw error::runtime( "jpeg: " ) << "Unsupported image format";
		}
	}

	uncompressed_image jpeg_swap_br( const uncompressed_image& in )
	{
		if ( in.format != uncompressed_image::bgr_u8 )
			return in;

		uncompressed_image out( in.width, in.height, uncompressed_image::rgb_u8 );
		u8* begin_i = in.as< u8 >();
		u8* begin_o = out.as< u8 >();

		for ( up_t i = 0, end = in.byte_size(); i != end; i += 3 )
		{
			u8* x = begin_i + i;
			u8* y = begin_o + i;

			y[ 0 ] = x[ 2 ];
			y[ 1 ] = x[ 1 ];
			y[ 2 ] = x[ 0 ];
		}

		return out;
	}
}

namespace ooe
{
//--- jpeg ---------------------------------------------------------------------
	uncompressed_image jpeg::decode( const descriptor& desc )
	{
		memory memory( desc );
		return jpeg::decode( memory.get(), memory.size() );
	}

	uncompressed_image jpeg::decode( const void* data, up_t size )
	{
		jpeg_decompress_struct read_struct;

		jpeg_error_mgr error_struct;
		read_struct.err = jpeg_std_error( &error_struct );
		error_struct.error_exit = jpeg_error< read_tuple >;
		error_struct.output_message = jpeg_warning;

		read_tuple tuple;
		tuple._2 = static_cast< const u8* >( data );
		tuple._3 = size;
		read_struct.client_data = &tuple;

		jpeg_create_decompress( &read_struct );
		scoped< void ( jpeg_decompress_struct* ) >
			scoped_read( jpeg_destroy_decompress, &read_struct );
		scoped< void ( uncompressed_image* ) > scoped_image;

		if ( setjmp( tuple._0 ) )
			throw error::runtime( "jpeg: " ) << "Error: " << tuple._1;

		jpeg_source_mgr source_struct;
		source_struct.init_source = jpeg_init;
		source_struct.fill_input_buffer = jpeg_fill;
		source_struct.skip_input_data = jpeg_skip;
		source_struct.resync_to_restart = jpeg_resync_to_restart;
		source_struct.term_source = jpeg_term;
		read_struct.src = &source_struct;

		jpeg_read_header( &read_struct, true );
		jpeg_start_decompress( &read_struct );

		uncompressed_image::type type = jpeg_image_type( read_struct.out_color_space );
		uncompressed_image image( read_struct.output_width, read_struct.output_height, type );
		scoped_image.assign( destruct< uncompressed_image >, &image );

		u8* pointer = image.as< u8 >();
		up_t row_size = image.row_size();

		for ( u32 i = 0; i != image.height; ++i, pointer += row_size )
			jpeg_read_scanlines( &read_struct, &pointer, 1 );

		jpeg_finish_decompress( &read_struct );
		scoped_image.clear();
		return image;
	}

	void jpeg::encode( const uncompressed_image& in, const descriptor& desc )
	{
		jpeg_compress_struct write_struct;

		jpeg_error_mgr error_struct;
		write_struct.err = jpeg_std_error( &error_struct );
		error_struct.error_exit = jpeg_error< write_tuple >;
		error_struct.output_message = jpeg_warning;

		file file( desc );
		write_tuple tuple;
		tuple._3 = &file;
		write_struct.client_data = &tuple;

		const uncompressed_image image = jpeg_swap_br( in );
		jpeg_create_compress( &write_struct );
		scoped< void ( jpeg_compress_struct* ) > scoped( jpeg_destroy_compress, &write_struct );

		if ( setjmp( tuple._0 ) )
			throw error::runtime( "jpeg: " ) << "Error: " << tuple._1;

		write_struct.image_width = image.width;
		write_struct.image_height = image.height;
		write_struct.input_components = image.channels();
		write_struct.in_color_space = jpeg_colour_space( image.format );

		jpeg_set_defaults( &write_struct );
		jpeg_set_quality( &write_struct, 80, true );

		jpeg_destination_mgr target_struct;
		target_struct.init_destination = jpeg_init;
		target_struct.empty_output_buffer = jpeg_empty;
		target_struct.term_destination = jpeg_term;
		write_struct.dest = &target_struct;

		jpeg_start_compress( &write_struct, true );
		u8* pointer = image.as< u8 >();
		up_t row_size = image.row_size();

		for ( u32 i = 0; i != image.height; ++i, pointer += row_size )
			jpeg_write_scanlines( &write_struct, &pointer, 1 );

		jpeg_finish_compress( &write_struct );
	}
}
