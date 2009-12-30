/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <squish.h>

#include "foundation/image/dds.hpp"
#include "foundation/io/file.hpp"
#include "foundation/io/memory.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/miscellany.hpp"

namespace
{
	using namespace ooe;

	struct OOE_PACKED dds_pixel_format
	{
		enum
		{
			use_fourcc		= 0x00000004
		};

		u32 size;	// sizeof( dds_pixel_format ) = 32
		u32 flags;
		u32 fourcc;
		u32 rgb_bits;
		u32 r_mask;
		u32 g_mask;
		u32 b_mask;
		u32 a_mask;
	};

	struct OOE_PACKED dds_header
	{
		enum
		{
			use_texture		= 0x00001007,
			use_linear_size	= 0x00080000
		};

		u32 size;	// sizeof( dds_header ) = 124
		u32 flags;
		u32 height;
		u32 width;
		u32 linear_size;
		u32 depth;
		u32 mipmaps;
		u32 reserved_1[11];
		dds_pixel_format pixel_format;
		u32 surface_flags;
		u32 cubemap_flags;
		u32 reserved_2[ 3 ];
	};

	compressed_image::type dds_compressed_image_type( u32 fourcc )
	{
		switch ( fourcc )
		{
		case OOE_FOURCC( 'D', 'X', 'T', '1' ):
			return compressed_image::rgba_dxt1;

		case OOE_FOURCC( 'D', 'X', 'T', '3' ):
			return compressed_image::rgba_dxt3;

		case OOE_FOURCC( 'D', 'X', 'T', '5' ):
			return compressed_image::rgba_dxt5;

		default:
			throw error::runtime( "dds: " ) << "Unsupported dxt version";
		}
	}

	u32 dds_pixel_format_fourcc( u8 type )
	{
		switch ( type )
		{
		case compressed_image::rgba_dxt1:
			return OOE_FOURCC( 'D', 'X', 'T', '1' );

		case compressed_image::rgba_dxt3:
			return OOE_FOURCC( 'D', 'X', 'T', '3' );

		case compressed_image::rgba_dxt5:
			return OOE_FOURCC( 'D', 'X', 'T', '5' );

		default:
			throw error::runtime( "dds: " ) << "Unsupported image type";
		}
	}

	u32 dxt_version( u8 type )
	{
		switch ( type )
		{
		case compressed_image::rgba_dxt1:
			return squish::kDxt1;

		case compressed_image::rgba_dxt3:
			return squish::kDxt3;

		case compressed_image::rgba_dxt5:
			return squish::kDxt5;

		default:
			throw error::runtime( "dxt: " ) << "Unsupported image type";
		}
	}
}

namespace ooe
{
//--- dds ----------------------------------------------------------------------
	compressed_image dds::decode( const descriptor& desc )
	{
		memory memory( desc );
		return dds::decode( memory.get(), memory.size() );
	}

	compressed_image dds::decode( const void* data, up_t size )
	{
		if ( *static_cast< const u32* >( data ) != OOE_FOURCC( 'D', 'D', 'S', ' ' ) )
			throw error::runtime( "dds: " ) << "Image is not a DirectDraw Surface";

		const dds_header& header = *add< const dds_header >( data, 4 );

		if ( header.size != sizeof( dds_header ) )
			throw error::runtime( "dds: " ) << "Incorrect header size " << header.size;
		else if ( header.pixel_format.size != sizeof( dds_pixel_format ) )
			throw error::runtime( "dds: " ) <<
				"Incorrect pixel format size " << header.pixel_format.size;
		else if ( ~header.flags & dds_header::use_texture )
			throw error::runtime( "dds: " ) << "Image is not 2-dimensional";
		else if ( ~header.pixel_format.flags & dds_pixel_format::use_fourcc )
			throw error::runtime( "dds: " ) << "Unsupported pixel format";

		compressed_image::type type = dds_compressed_image_type( header.pixel_format.fourcc );
		compressed_image image( header.width, header.height, type );
		up_t byte_size = image.byte_size();

		// conditionally verify linear_size, needed to work around microsoft bug
		if ( header.flags & dds_header::use_linear_size && header.linear_size != byte_size )
			throw error::runtime( "dds: " ) <<
				"Linear size " << header.linear_size << " does not match image size " << byte_size;
		else if ( header.linear_size + sizeof( dds_header ) + 4 > size )
			throw error::runtime( "dds: " ) <<
				"Linear size " << header.linear_size << " is greater than file size " << size;

		const u8* pointer = add< u8 >( &header, sizeof( dds_header ) );
		std::memcpy( image.get(), pointer, byte_size );
		return image;
	}

	void dds::encode( const compressed_image& image, const descriptor& desc )
	{
		file file( desc );
		u32 magic = OOE_FOURCC( 'D', 'D', 'S', ' ' );

		dds_header header;
		std::memset( &header, 0, sizeof( dds_header ) );
		header.size = sizeof( dds_header );
		header.flags = dds_header::use_texture | dds_header::use_linear_size;
		header.height = image.height;
		header.width = image.width;
		header.linear_size = image.byte_size();
		header.pixel_format.size = sizeof( dds_pixel_format );
		header.pixel_format.flags = dds_pixel_format::use_fourcc;
		header.pixel_format.fourcc = dds_pixel_format_fourcc( image.format );

		if ( file.write( &magic, sizeof( magic ) ) != sizeof( magic ) ||
			file.write( &header, sizeof( header ) )  != sizeof( header ) ||
			file.write( image.get(), header.linear_size ) != header.linear_size )
			throw error::runtime( "dds: " ) << "Unable to write image";
	}

//--- dxt ----------------------------------------------------------------------
	uncompressed_image dxt::decode( const compressed_image& in )
	{
		using namespace squish;

		uncompressed_image out( in.width, in.height, uncompressed_image::rgba_u8 );
		u32 flags = dxt_version( in.format );
		DecompressImage( out.as< u8 >(), in.width, in.height, in.as< u8 >(), flags );
		return out;
	}

	compressed_image dxt::encode( const uncompressed_image& in, compressed_image::type type )
	{
		using namespace squish;

		compressed_image out( in.width, in.height, type );
		u32 flags = dxt_version( type ) | squish::kColourIterativeClusterFit;
		CompressImage( in.as< u8 >(), in.width, in.height, out.as< u8 >(), flags );
		return out;
	}
}
