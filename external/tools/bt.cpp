/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>
#include <limits>

#include "foundation/io/file.hpp"
#include "foundation/utility/binary.hpp"
#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"

#include "external/tools/bt.hpp"

namespace
{
	using namespace ooe;

	// as described by: http://vterrain.org/Implementation/Formats/BT.html
	struct header
	{
		enum
		{
			degrees,
			meters,
			feet,
			feet_usa
		};

		static const u16 default_datum = 6326;	// wgs84 datum

		c8 id[ 10 ];

		u32 width;
		u32 height;

		u16 data_size;
		u16 float_flag;
		u16 horizontal_unit;
		u16 utm_zone;
		u16 datum;

		f64 extent_left;
		f64 extent_right;
		f64 extent_bottom;
		f64 extent_top;

		u16 external_projection;
		f32 vertical_scale;

		c8 unused[ 190 ];
	} OOE_PACKED;

	template< typename type >
		void write( file& file, const uncompressed_image& image )
	{
		const type* pixel_be = image.as< type >();
		type min = std::numeric_limits< type >::max();
		type max = std::numeric_limits< type >::min();

		for ( u32 j = 0, end_j = image.width; j != end_j; ++j )
		{
			for ( u32 i = 0, end_i = image.height; i != end_i; ++i, ++pixel_be )
			{
				typedef typename find_size< unsigned_types, sizeof( type ) >::type uint_t;
				const type pixel_le = static_cast< type >
					( endian_swap( *reinterpret_cast< const uint_t* >( pixel_be ) ) );

				min = std::min( min, pixel_le );
				max = std::max( max, pixel_le );
				file.write( &pixel_le, sizeof( type ) );
			}
		}

		std::cerr <<
			"Created binary terrain\n"
			"\tdata size: " << sizeof( type ) * 8 << "\n"
			"\tmin height: " << min << "\n"
			"\tmax height: " << max << '\n';
	}
}

namespace ooe
{
	void make_bt( const uncompressed_image& image, const std::string& name )
	{
		if ( image.format != uncompressed_image::y_u8 || image.format != uncompressed_image::y_f32 )
			throw error::runtime( "tools: " ) << "Unsupported image format";

		header header;
		std::memset( &header, 0, sizeof( header ) );
		std::memcpy( header.id, "binterr1.3", sizeof( header.id ) );

		header.width = image.width;
		header.height = image.height;
		header.data_size = static_cast< u16 >( image.format == uncompressed_image::y_u8 ? 2 : 4 );
		header.horizontal_unit = header::meters;
		header.datum = header::default_datum;
		header.extent_right = header.width;
		header.extent_top = header.height;
		header.vertical_scale = 1;

		file file( descriptor( name + ".bt", descriptor::write_new ) );
		file.write( &header, sizeof( header ) );

		switch ( image.format )
		{
		case uncompressed_image::y_u8:
			return write< u8 >( file, image );

		case uncompressed_image::y_f32:
			return write< f32 >( file, image );

		default:
			throw error::runtime( "tools: " ) << "Unable to handle image format";
		}
	}
}
