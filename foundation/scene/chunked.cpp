/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <cmath>

#include "foundation/image/jpeg.hpp"
#include "foundation/scene/chunked.hpp"
#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
//--- compressed_vertex --------------------------------------------------------
	struct compressed_vertex
	{
		s16 x;					// compressed x
		s16 y;					// compressed y
		s16 z;					// compressed z
		s16 delta;				// y morph delta
	} OOE_PACKED;

//--- vertex_header ------------------------------------------------------------
	struct vertex_header
	{
		u16 vertex_count;
		compressed_vertex vertex[ 1 ];
	} OOE_PACKED;

//--- index_header -------------------------------------------------------------
	struct index_header
	{
		u32 index_count;
		u16 index[ 1 ];
	} OOE_PACKED;

//--- tqt_header ---------------------------------------------------------------
	struct tqt_header
	{
		u32 id;					// fourcc: "tqt"
		u32 version;			// version: 1
		u32 tree_depth;			// depth of texture quad tree

		u32 tile_size;
		u32 data_offset[ 1 ];	// texture data offset
	} OOE_PACKED;

//--- chunk_header -------------------------------------------------------------
	struct chunk_header
	{
		u32 label;				// this chunk's label
		s32 neighbour[ 4 ];		// the neighbour's labels

		u8 level;
		u16 x;
		u16 z;
		s16 min_y;
		s16 max_y;

		u32 data_offset;		// vertex data offset
	} OOE_PACKED;

//--- chunked_header -----------------------------------------------------------
	struct chunked_header
	{
		u32 id;					// fourcc: "CHU"
		u16 version;			// version: 9
		u16 tree_depth;			// depth of chunk lod tree

		f32 max_error;			// error on maximum resolution
		f32 vertical_scale;		// vertical scale of terrain
		f32 horizontal_scale;	// horizontal scale of terrain
		u32 chunk_count;		// number of chunks contained
	} OOE_PACKED;
}

namespace
{
	using namespace ooe;

//--- utility ------------------------------------------------------------------
	const chunked_header& chunked_header_f( const void* pointer )
	{
		const chunked_header& header = *static_cast< const chunked_header* >( pointer );

		if ( header.id != OOE_FOURCC( 'C', 'H', 'U', 0 ) || header.version != 9 )
			throw error::runtime( "chunked: " ) <<
				"Invalid chunked data (version " << header.version << ")";

		return header;
	}

	const vertex_header& vertex_header_f( const chunk_header& header, const void* pointer )
	{
		pointer = add< void >( pointer, header.data_offset );
		return *static_cast< const vertex_header* >( pointer );
	}

	const index_header& index_header_f( const vertex_header& header )
	{
		const void* pointer = &header;
		pointer = add< void >( pointer, sizeof( vertex_header ) );
		pointer = add< void >( pointer, sizeof( compressed_vertex ) * ( header.vertex_count - 1 ) );
		return *static_cast< const index_header* >( pointer );
	}

	aabb box_f( const chunked_header& root, const chunk_header& leaf )
	{
		// factor is the amount to adjust dimensions by based on chunk level
		const f32 factor = static_cast< f32 >( 1 << ( root.tree_depth - leaf.level - 1 ) );
		// extra is the amount to adjust the x/z extents by to cover gaps
		const f32 extra = 1e-3f;

		const f32 xz = factor * root.horizontal_scale * .5f + extra;
		const f32 difference = static_cast< f32 >( leaf.max_y - leaf.min_y );
		const vec3 extent( xz, difference * .5f * root.vertical_scale, xz );
		const vec3 center(
			( static_cast< f32 >( leaf.x ) + .5f ) * factor * root.horizontal_scale,
			static_cast< f32 >( leaf.max_y + leaf.min_y ) * .5f * root.vertical_scale,
			( static_cast< f32 >( leaf.z ) + .5f ) * factor * root.horizontal_scale );

		return aabb( center - extent, center + extent );
	}

	u16 lod_f( const aabb& box, const vec3& eye, f32 max_lod, u16 tree_depth )
	{
		const vec3 extent = aabb_extent( box );
		vec3 displace = eye - aabb_center( box );
		displace.x = std::max( std::abs( displace.x ) - extent.x, 0.f );
		displace.y = std::max( std::abs( displace.y ) - extent.y, 0.f );
		displace.z = std::max( std::abs( displace.z ) - extent.z, 0.f );

		const f32 d = std::sqrt( dot( displace, displace ) );
		const s32 t = ( tree_depth << 8 ) - 1;
		const s32 l = static_cast< s32 >( log2f( std::max( 1.f, d / max_lod ) ) * 256 );
		return static_cast< u16 >( clamp( t - l, 0, 0xffff ) );
	}

	const tqt_header& tqt_header_f( const void* pointer )
	{
		const tqt_header& header = *static_cast< const tqt_header* >( pointer );

		if ( header.id != OOE_FOURCC( 't', 'q', 't', 0 ) || header.version != 1 )
			throw error::runtime( "tqt: " ) <<
				"Invalid tqt data (version " << header.version << ")";

		return header;
	}

	u32 count_f( u32 depth )
	{
		return 0x55555555 & ( ( 1 << depth * 2 ) - 1 );
	}

	void unload_f( chunk& chunk, atom& state )
	{
		chunk.parallel_unload();
		state = chunk::none;
	}
}

namespace ooe
{
//--- common_uniform -----------------------------------------------------------
	class common_uniform
	{
	public:
		common_uniform( const program& program )
			: min_1( program.uniform( "min_1", uniform::float_3 ) ),
			max_1( program.uniform( "max_1", uniform::float_3 ) ),
			min_2( program.uniform( "min_2", uniform::float_3 ) ),
			max_2( program.uniform( "max_2", uniform::float_3 ) ),
			morph( program.uniform( "morph", uniform::float_1 ) )
		{
		}

		void apply( variable& var, const aabb& box_1, const aabb& box_2, const f32& value ) const
		{
			var.insert( min_1, &box_1.min.x );
			var.insert( max_1, &box_1.max.x );
			var.insert( min_2, &box_2.min.x );
			var.insert( max_2, &box_2.max.x );
			var.insert( morph, &value );
		}

	private:
		const uniform_type min_1;
		const uniform_type max_1;
		const uniform_type min_2;
		const uniform_type max_2;
		const uniform_type morph;
	};

//--- chunk_args ---------------------------------------------------------------
	struct chunk_args
	{
		const common_uniform common;

		const chunked_header& root;
		chunk::array_type& array;
		u32 allocated;

		const void* base;
		const void* pointer;
		const u32 tqt_depth;

		chunk_args( const program& program, const chunked_header& root_, chunk::array_type& array_,
			const void* base_, u32 tqt_depth_ )
			: common( program ), root( root_ ), array( array_ ), allocated( 1 ), base( base_ ),
			pointer( add< void >( base, sizeof( chunked_header ) ) ), tqt_depth( tqt_depth_ )
		{
		}
	};

//--- update_args --------------------------------------------------------------
	struct update_args
	{
		ooe::schedule& schedule;
		const ooe::video& video;
		const ooe::tqt& tqt;
		const attribute_type& delta;
		const vec3& eye;

		const f32 max_lod;
		const u16 tree_depth;

		update_args( ooe::schedule& schedule_, const ooe::video& video_, const ooe::tqt& tqt_,
			const attribute_type& delta_, const vec3& eye_, f32 max_lod_, u16 tree_depth_ )
			: schedule( schedule_ ), video( video_ ), tqt( tqt_ ), delta( delta_ ), eye( eye_ ),
			max_lod( max_lod_ ), tree_depth( tree_depth_ )
		{
		}
	};

//--- includes_args ------------------------------------------------------------
	struct includes_args
	{
		const ooe::frustum& frustum;
		ooe::render_queue& queue;
		ooe::program& program;

		includes_args( const ooe::frustum& frustum_, ooe::render_queue& queue_,
			ooe::program& program_ )
			: frustum( frustum_ ), queue( queue_ ), program( program_ )
		{
		}

		void enqueue( const video_data& data, const variable& var, const texture_type& tex_1,
			const texture_type& tex_2 ) const
		{
			render_queue::insert_type value( mat4::identity, data, &var, &program, tex_1, tex_2 );
			queue.insert( value );
		}
	};

//--- tqt ----------------------------------------------------------------------
	tqt::tqt( const descriptor& desc )
		: memory( desc ), header( tqt_header_f( memory.get() ) )
	{
	}

	uncompressed_image tqt::load( u32 level, u32 column, u32 row ) const
	{
		const u32 index = count_f( level ) + ( row << level ) + column;
		const u32 offset = header.data_offset[ index ];
		return jpeg::decode( add< void >( memory.get(), offset ), memory.size() - offset );
	}

	u32 tqt::depth( void ) const
	{
		return header.tree_depth;
	}

//--- chunk --------------------------------------------------------------------
	chunk::chunk( chunk_args& args, chunk* parent_, u16 chunk_depth )
		: header( *static_cast< const chunk_header* >( args.pointer ) ),
		vh( vertex_header_f( header, args.base ) ), ih( index_header_f( vh ) ), parent( parent_ ),
		child(), box( box_f( args.root, header ) ), tex_1(), tex_2(), data(), var(), state( none ),
		image(), point(), lod( static_cast< u16 >( parent ? parent->lod + 256 : 0 ) ), morph( 0 ),
		split( false )
	{
		//--- setup member variables -------------------------------------------
		chunk* up_1 = this;
		chunk* up_2 = up_1->parent ? up_1->parent : up_1;

		// run up tree to find appropriate primary and secondary textures
		while ( up_1->header.level >= args.tqt_depth )
			up_1 = up_1->parent;

		if ( header.level > args.tqt_depth )
			up_2 = up_1;

		tex_2 = &up_2->tex_1;
		args.common.apply( var, up_1->box, up_2->box, morph );

		if ( !chunk_depth )
		{
			std::memset( child, 0, sizeof( child ) );
			return;
		}

		//--- recurse to child nodes -------------------------------------------
		const u32 chunks = args.root.chunk_count;
		const u16 depth = static_cast< u16 >( chunk_depth - 1 );

		for ( u8 i = 0; i != 4; ++i )
		{
			args.pointer = add< void >( args.pointer, sizeof( chunk_header ) );
			const u32 label = static_cast< const chunk_header* >( args.pointer )->label;

			if ( args.allocated++ == chunks )
				throw error::runtime( "chunk: " ) << "More than " << chunks << " chunks found";
			else if ( label >= chunks )
				throw error::runtime( "chunk: " ) << "Invalid label \"" << label << '\"';
			else if ( args.array[ label ] )
				throw error::runtime( "chunk: " ) << "Duplicate label \"" << label << '\"';
			else
				child[ i ] = args.array[ label ] = new chunk( args, this, depth );
		}
	}

	void chunk::parallel_load( const tqt& tqt, const attribute_type& delta )
	{
		//--- load image data --------------------------------------------------
		if ( header.level < tqt.depth() )
			image = new uncompressed_image( tqt.load( header.level, header.x, header.z ) );

		//--- setup video data -------------------------------------------------
		const vec3 extent = aabb_extent( box ) / ( 1 << 14 );
		const vec3 center = aabb_center( box );
		data.mode = video_data::triangle_strip;
		data.length = ih.index_count;
		data.size = 3;
		data.layout = new video_data::layout_type[ data.size ];
		data.extra = new attribute_type[ 1 ];

		data.layout[ 0 ] = video_data::vertex_3;
		data.layout[ 1 ] = video_data::attribute;
		data.layout[ 2 ] = video_data::index_16;

		data.extra[ 0 ] = delta;

		//--- build point data -------------------------------------------------
		point = new f32[ vh.vertex_count * 4 ];

		for ( u16 i = 0; i != vh.vertex_count; ++i )
		{
			f32* value = point + i * 4;
			const compressed_vertex& vertex = vh.vertex[ i ];

			value[ 0 ] = static_cast< f32 >( vertex.x ) * extent.x + center.x;
			value[ 1 ] = vertex.y;
			value[ 2 ] = static_cast< f32 >( vertex.z ) * extent.z + center.z;
			value[ 3 ] = vertex.delta;
		}

		state = half;
	}

	void chunk::parallel_unload( void )
	{
		image = 0;
		point = 0;
	}

	void chunk::serial_load( const video& video, u32 tqt_depth )
	{
		if ( image )
		{
			tex_1 = video.texture();
			tex_1->bind();
			tex_1->load( image->get(), image->width, image->height, image->format );
		}
		else
		{
			const chunk* up = this;

			// run up tree to find appropriate primary texture
			while ( up->header.level >= tqt_depth )
				up = up->parent;

			tex_1 = up->tex_1;
		}

		data.point = video.buffer( buffer::point );
		data.point->bind();
		data.point->load( point, 4 * vh.vertex_count * sizeof( f32 ), buffer::static_draw );

		data.index = video.buffer( buffer::index );
		data.index->bind();
		data.index->load( ih.index, ih.index_count * sizeof( u16 ), buffer::static_draw );

		state = full;
	}

	void chunk::serial_unload( void )
	{
		tex_1 = 0;
		data.point = 0;
		data.index = 0;
	}

	void chunk::unload_subtree( schedule& schedule )
	{
		if ( !child[ 0 ] )
			return;

		for ( u8 i = 0; i != 4; ++i )
		{
			chunk& leaf = *child[ i ];
			// shark showed atom::operator s32() wasting cpu time
			const s32 leaf_state = leaf.state;

			if ( leaf_state == half )
			{
				leaf.state = wait;
				schedule.push_back< void, chunk&, atom& >( unload_f, leaf, leaf.state );
			}
			else if ( leaf_state == full )
			{
				leaf.serial_unload();
				leaf.state = none;
			}
			else
				continue;

			leaf.unload_subtree( schedule );
		}
	}

	void chunk::update( const update_args& args )
	{
		const s32 this_lod = lod_f( box, args.eye, args.max_lod, args.tree_depth );

		//--- no split ---------------------------------------------------------
		if ( !child[ 0 ] || this_lod < ( lod | 0xff ) )
		{
			// if children, unload
			unload_subtree( args.schedule );

			// root chunk, calculate lod
			if ( !( lod & 0xff00 ) )
				lod = static_cast< u16 >( clamp( this_lod, lod & 0xff00, lod | 0xff ) );

			morph = 1.f - divide( lod & 255, 255 );
			split = false;
			return;
		}

		//--- prepare ----------------------------------------------------------
		split = true;

		for ( u8 i = 0; i != 4; ++i )
		{
			chunk& leaf = *child[ i ];
			// shark showed atom::operator s32() wasting cpu time
			const s32 leaf_state = leaf.state;

			if ( leaf_state == none )
			{
				leaf.state = wait;
				args.schedule.push_back< void, const tqt&, const attribute_type& >
					( make_function( leaf, &chunk::parallel_load ), args.tqt, args.delta );
				split = false;
				continue;
			}
			else if ( leaf_state == wait )
			{
				split = false;
				continue;
			}
			else if ( leaf_state == half )
			{
				leaf.serial_load( args.video, args.tqt.depth() );
				args.schedule.push_back( make_function( leaf, &chunk::parallel_unload ) );
			}
		}

		if ( !split )
			return;

		//--- split ------------------------------------------------------------
		for ( u8 i = 0; i != 4; ++i )
		{
			chunk& leaf = *child[ i ];
			const s32 child_lod = lod_f( leaf.box, args.eye, args.max_lod, args.tree_depth );
			leaf.lod = static_cast< u16 >( clamp( child_lod, leaf.lod & 0xff00, leaf.lod | 0xff ) );
			leaf.update( args );
		}
	}

	void chunk::includes( const includes_args& args, geometry::intersection result ) const
	{
		if ( result != geometry::inside )
		{
			result = ooe::includes( args.frustum, box );

			if ( result == geometry::outside )
				return;
		}

		if ( !split )
		{
			args.enqueue( data, var, tex_1, *tex_2 );
			return;
		}

		for ( u8 i = 0; i != 4; ++i )
			child[ i ]->includes( args, result );
	}

	aabb chunk::aabb( void ) const
	{
		return box;
	}

//--- chunked ------------------------------------------------------------------
	chunked::chunked( const std::string& path, ooe::schedule& schedule_,  cache& cache, radian fov,
		u32 width, f32 max_pixel )
		: tqt( cache.vfs[ path + ".tqt" ] ), memory( cache.vfs[ path + ".chu" ] ),
		header( chunked_header_f( memory.get() ) ), chunk( new chunk::type[ header.chunk_count ] ),
		lod( ( header.max_error / max_pixel ) * ( static_cast< f32 >( width ) / std::tan( fov ) ) ),
		schedule( schedule_ ), video( cache.video ),
		program( cache.program( "effect/chunked.lua" ) ), delta()
	{
		program.bind();
		chunk_args args( program, header, chunk, memory.get(), tqt.depth() );
		delta = program.attribute( "delta", attribute::float_1 );
		const u32 label = static_cast< const chunk_header* >( args.pointer )->label;

		if ( label )
			throw error::runtime( "chunked: " ) << "Invalid root chunk label \"" << label << '\"';

		chunk[ 0 ] = new ooe::chunk( args, 0, static_cast< u16 >( header.tree_depth - 1 ) );
		chunk[ 0 ]->parallel_load( tqt, delta );
		chunk[ 0 ]->serial_load( video, args.tqt_depth );
		chunk[ 0 ]->parallel_unload();

		const s32 unit[] = { 0, 1 };
		uniform_type scale = program.uniform( "scale", uniform::float_1 );
		scale->load( &header.vertical_scale );
		uniform_type tex_1 = program.uniform( "tex_1", uniform::int_1 );
		tex_1->load( unit + 0 );
		uniform_type tex_2 = program.uniform( "tex_2", uniform::int_1 );
		tex_2->load( unit + 1 );
	}

	aabb chunked::aabb( void ) const
	{
		return chunk[ 0 ]->aabb();
	}

	void chunked::includes( const camera_instance& instance, render_queue& queue ) const
	{
		// need to adjust position details due to node position in scene graph
		const u16 depth = header.tree_depth;
		const vec3& eye = instance.position;
		const frustum& frustum = instance.frustum;

		chunk[ 0 ]->update( update_args( schedule, video, tqt, delta, eye, lod, depth ) );
		chunk[ 0 ]->includes( includes_args( frustum, queue, program ), geometry::outside );
	}

	node::range_type chunked::iterator_range( void ) const
	{
		return range_type();
	}

	bool chunked::insert( node&, graph::list_type& )
	{
		return false;
	}
}
