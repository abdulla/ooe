/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/general/maths.hpp"
#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/pointer.hpp"

#include "external/tools/height.hpp"
#include "external/tools/interface.hpp"

namespace
{
	using namespace ooe;

	void image_calc( const uncompressed_image& image, const f32* scale, f32* vertices,
		f32* textures, const bool bit_16, u16* indices_16, u32* indices_32 )
	{
		if ( image.format != uncompressed_image::y_u8 )
			throw error::runtime( "tools: " ) << "Unsupported image format";

		const u8 channel_size = image.channel_size();
		u8* const data = image.as< u8 >();
		u32 vertex = 0;
		u32 texture = 0;
		u32 index = 0;
		f32 u = 0;
		f32 v = 0;
		f32 offset_u = divide( 1, image.width - 1 );
		f32 offset_v = divide( 1, image.height - 1 );
		up_t offset = 0;

		for ( u32 y = 0; y != image.height; ++y, v += offset_v )
		{
			u = 0;

			for ( u32 x = 0; x != image.width; ++x, u += offset_u )
			{
				vertices[ vertex++ ] =
					( static_cast< f32 >( x ) - divide( image.width, 2 ) ) * scale[ 0 ];
				vertices[ vertex ] = 0;

				for ( u8 i = 0; i != channel_size; ++i )
					vertices[ vertex ] += static_cast< f32 >( data[ offset++ ] << ( i * 8 ) );

				vertices[ vertex++ ] *= scale[ 1 ];
				vertices[ vertex++ ] =
					( static_cast< f32 >( y ) - divide( image.height, 2 ) ) * scale[ 2 ];
				textures[ texture++ ] = u;
				textures[ texture++ ] = v;

				if ( y >= image.height - 1 || x >= image.width - 1 )
					continue;

				u32 quad[ 4 ] =
				{
					x + ( y * image.width ),
					( x + 1 ) + ( y * image.width ),
					( x + 1 ) + ( ( y + 1 ) * image.width ),
					x + ( ( y + 1 ) * image.width )
				};

				if ( bit_16 )
				{
					indices_16[ index++ ] = static_cast< u16 >( quad[ 2 ] );
					indices_16[ index++ ] = static_cast< u16 >( quad[ 1 ] );
					indices_16[ index++ ] = static_cast< u16 >( quad[ 0 ] );
					indices_16[ index++ ] = static_cast< u16 >( quad[ 0 ] );
					indices_16[ index++ ] = static_cast< u16 >( quad[ 3 ] );
					indices_16[ index++ ] = static_cast< u16 >( quad[ 2 ] );
				}
				else
				{
					indices_32[ index++ ] = quad[ 2 ];
					indices_32[ index++ ] = quad[ 1 ];
					indices_32[ index++ ] = quad[ 0 ];
					indices_32[ index++ ] = quad[ 0 ];
					indices_32[ index++ ] = quad[ 3 ];
					indices_32[ index++ ] = quad[ 2 ];
				}
			}
		}
	}

	void face_calc( const u32 length, const u32 amount, const f32* vertices, f32* normals,
		const bool bit_16, const u16* indices_16, const u32* indices_32 )
	{
		scoped_array< f32 > faces( new f32[ length ] );
		u32 index = 0;

		while ( index != length )
		{
			const f32* off_a;
			const f32* off_b;
			const f32* off_c;

			if ( bit_16 )
			{
				off_a = vertices + ( indices_16[ index++ ] * 3 );
				off_b = vertices + ( indices_16[ index++ ] * 3 );
				off_c = vertices + ( indices_16[ index++ ] * 3 );
			}
			else
			{
				off_a = vertices + ( indices_32[ index++ ] * 3 );
				off_b = vertices + ( indices_32[ index++ ] * 3 );
				off_c = vertices + ( indices_32[ index++ ] * 3 );
			}

			vec3 a( off_a[ 0 ], off_a[ 1 ], off_a[ 2 ] );
			vec3 b( off_b[ 0 ], off_b[ 1 ], off_b[ 2 ] );
			vec3 c( off_c[ 0 ], off_c[ 1 ], off_c[ 2 ] );
			b -= a;
			c -= a;
			a = cross( b, c );
			faces[ index - 3 ] = a.x;
			faces[ index - 2 ] = a.y;
			faces[ index - 1 ] = a.z;
		}

		u32 vertex = 0;

		while ( vertex != amount )
		{
			index = 0;
			u32 normal = vertex / 3;
			vec3 sum( vec3::zero );

			while ( index != length )
			{
				const f32* v = faces + index;

				if ( bit_16 )
				{
					if ( indices_16[ index ] == normal ||
						indices_16[ index + 1 ] == normal ||
						indices_16[ index + 2 ] == normal )
						sum += column( v );
				}
				else
				{
					if ( indices_32[ index ] == normal ||
						indices_32[ index + 1 ] == normal ||
						indices_32[ index + 2 ] == normal )
						sum += column( v );
				}

				index += 3;
			}

			sum = normalise( sum );
			normals[ vertex++ ] = sum.x;
			normals[ vertex++ ] = sum.y;
			normals[ vertex++ ] = sum.z;
		}
	}

	void map_write( const std::string& name, const u32 length, const u32 points,
		const f32* vertices, const f32* normals, const f32* textures, const bool bit_16,
		const u16* indices_16, const u32* indices_32 )
	{
		file file( descriptor( name + ".lua", descriptor::write_new ) );

		std::string string;
		string <<
		"function load( video )\n"
			"\tlocal layout_array =\n"
			"\t{\n"
				"\t\tvideo.vertex,\n"
				"\t\tvideo.normal,\n"
				"\t\tvideo.texture,\n"
				"\t\tvideo.index_" << ( bit_16 ? "16" : "32" ) << "\n"
			"\t}\n"
			"\n"
			"\tlocal point_buffer = video:buffer( video.point )\n"
			"\tpoint_buffer:load( point_data )\n"
			"\n"
			"\tlocal index_buffer = video:buffer( video.index )\n"
			"\tindex_buffer:load( index_data )\n"
			"\n"
			"\treturn\n"
			"\t{\n"
				"\t\tmode = video.triangle_list,\n"
				"\t\tlength = " << length << ",\n"
				"\t\tlayout = layout_array,\n"
				"\t\tpoint = point_buffer,\n"
				"\t\tindex = index_buffer\n"
			"\t}\n"
		"end\n"
		"\n"
		"point_data = \"";
		file.write( string.c_str(), string.size() );

		for ( u32 i = 0; i != points; ++i )
		{
			tools::encode_write( file, vertices + ( i * 3 ), sizeof( f32 ) * 3 );
			tools::encode_write( file, normals + ( i * 3 ), sizeof( f32 ) * 3 );
			tools::encode_write( file, textures + ( i * 2 ), sizeof( f32 ) * 2 );
		}

		string.clear();
		string << "\"\n\nindex_data = \"";
		file.write( string.c_str(), string.size() );

		if ( bit_16 )
			tools::encode_write( file, indices_16, length * sizeof( u16 ) );
		else
			tools::encode_write( file, indices_32, length * sizeof( u32 ) );

		string.clear();
		string << '\"';
		file.write( string.c_str(), string.size() );
	}
}

namespace ooe
{
//--- make_height --------------------------------------------------------------
	void make_height( const uncompressed_image& image, const f32* scale, const std::string& name )
	{
		const u32 length = ( image.width - 1 ) * ( image.height - 1 ) * 6;
		const u32 points = image.width * image.height;
		const bool bit_16 = length < static_cast< u16 >( -1 ) ? true : false;
		scoped_array< f32 > vertices( new f32[ points * 3 ] );
		scoped_array< f32 > normals( new f32[ points * 3 ] );
		scoped_array< f32 > textures( new f32[ points * 2 ] );
		scoped_array< u16 > indices_16( bit_16 ? new u16[ length ] : 0 );
		scoped_array< u32 > indices_32( bit_16 ? 0 : new u32[ length ] );

		image_calc( image, scale, vertices, textures, bit_16, indices_16, indices_32 );
		face_calc( length, points * 3, vertices, normals, bit_16, indices_16, indices_32 );
		map_write( name, length, points, vertices, normals, textures, bit_16, indices_16,
			indices_32 );
	}
}
