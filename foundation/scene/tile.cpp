/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/image/png.hpp"
#include "foundation/scene/tile.hpp"
#include "foundation/utility/miscellany.hpp"

namespace ooe
{
	struct tile_header
	{
	};
}

namespace
{
	using namespace ooe;

	const tile_header& header_f( const void* pointer )
	{
		const tile_header& header = *static_cast< const tile_header* >( pointer );
		// do id check
		return header;
	}
}

namespace ooe
{
//--- tile ---------------------------------------------------------------------
	tile::tile( cache& cache )
		: program( cache.program( "effect/tile.lua" ) ), height( cache.video.texture() ), data()
	{
		uncompressed_image image = png::decode( cache.vfs[ "terrain/map.png" ] );
		const u8* buffer_u8 = image.as< u8 >();
		scoped_array< f32 > buffer_f32( new f32[ image.width * image.height ] );
		std::copy( buffer_u8, buffer_u8 + image.width * image.height, buffer_f32.get() );

		height->bind();
		height->load( buffer_f32, image.width, image.height, uncompressed_image::y_f32 );

		const u32 point_length = image.width * image.height * 2;
		const u32 index_length = ( image.width - 1 ) * ( image.height - 1 ) * 6;

		data.mode = video_data::triangle_list;
		data.length = index_length;
		data.size = 2;
		data.layout = new video_data::layout_type[ data.size ];
		data.layout[ 0 ] = video_data::vertex_2;
		data.layout[ 1 ] = video_data::index_16;

		scoped_array< f32 > point( new f32[ point_length ] );
		f32* data_point = point;

		for ( u32 y = 0; y != image.height; ++y )
		{
			for ( u32 x = 0; x != image.width; ++x, data_point += 2 )
			{
				data_point[ 0 ] = divide( x, image.width );
				data_point[ 1 ] = divide( y, image.height );
			}
		}

		scoped_array< u16 > index( new u16[ index_length ] );
		u16* data_index = index;

		for ( u32 y = 0; y != image.height - 1; ++y )
		{
			for ( u32 x = 0; x != image.width - 1; ++x, data_index += 6 )
			{
				u16 quad[ 4 ] =
				{
					static_cast< u16 >( x + ( y * image.width ) ),
					static_cast< u16 >( ( x + 1 ) + ( y * image.width ) ),
					static_cast< u16 >( ( x + 1 ) + ( ( y + 1 ) * image.width ) ),
					static_cast< u16 >( x + ( ( y + 1 ) * image.width ) )
				};

				data_index[ 0 ] = quad[ 2 ];
				data_index[ 1 ] = quad[ 1 ];
				data_index[ 2 ] = quad[ 0 ];
				data_index[ 3 ] = quad[ 0 ];
				data_index[ 4 ] = quad[ 3 ];
				data_index[ 5 ] = quad[ 2 ];
			}
		}

		data.point = cache.video.buffer( buffer::point );
		data.point->bind();
		data.point->load( point, point_length * sizeof( f32 ), buffer::static_draw );

		data.index = cache.video.buffer( buffer::index );
		data.index->bind();
		data.index->load( index, index_length * sizeof( u16 ), buffer::static_draw );
	}

	aabb tile::aabb( void ) const
	{
		return ooe::aabb( vec3::zero, vec3::zero );
	}

	void tile::includes( const camera_instance&, render_queue& queue ) const
	{
		queue.insert( render_queue::insert_type( mat4::identity, data, 0, &program, height, 0 ) );
	}

	node::range_type tile::iterator_range( void ) const
	{
		return range_type();
	}

	bool tile::insert( node&, graph::list_type& )
	{
		return false;
	}
}
