/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cmath>

#include "component/ui/virtual_texture.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/binary.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef tuple< u32, u32, u32, u32 > region_type;
const image::type table_format = image::rgb_f32;

u32 table_width( const device_type& device, const physical_source& source )
{
	u32 table_size = ceiling< u32 >( source.size(), source.page_size() );
	u32 size_limit = device->limit( device::texture_size );

	if ( table_size > size_limit )
		throw error::runtime( "virtual_texture: " ) <<
			"Page table size " << table_size << " > device texture size " << size_limit;

	return table_size;
}

u32 cache_width( const device_type& device, u16 page_size )
{
	if ( !is_bit_round( page_size ) )
		throw error::runtime( "physical_cache: " ) <<
			"Page size " << page_size << " is not a power of 2";

	return round_down< u32 >( device->limit( device::texture_size ), page_size );
}

image make_image( u32 size )
{
	uncompressed_image image( size, size, table_format );

	for ( f32* i = image.as< f32 >(), * end = i + image.width * image.height * 3; i != end; ++i )
		*i = -1;

	return image;
}

image_pyramid make_pyramid( u32 table_size )
{
	image_pyramid pyramid( make_image( table_size ) );

	for ( u32 x = table_size >> 1; x; x >>= 1 )
		pyramid.push_back( make_image( x ) );

	return pyramid;
}

texture_type make_cache( const device_type& device, u32 cache_size, image::type format )
{
	image_pyramid pyramid( cache_size, cache_size, format );
	return device->texture( pyramid, texture::linear, false );
}

void write_pyramid( image_pyramid& pyramid, const pyramid_index& index, f32 x, f32 y,
	f32 pow_level )
{
	u32 width = pyramid.width >> index.level;
	f32* rgb = pyramid[ index.level ].as< f32 >() + ( index.x + index.y * width ) * 3;
	rgb[ 0 ] = x;
	rgb[ 1 ] = y;
	rgb[ 2 ] = pow_level;
}

region_type get_region( const physical_source& source, u8 level_limit,
	u32 x, u32 y, u32 width, u32 height, u8 level )
{
	u32 size = source.size();

	if ( level > level_limit )
		throw error::runtime( "virtual_texture: " ) <<
			"Mipmap level " << level << " > maximum level " << level_limit;
	else if ( x + width > size || y + height > size )
		throw error::runtime( "virtual_texture: " ) <<
			"Load of (" << x << ", " << y << ", " << width << ", " << height <<
			") is outside of source size " << size << " at level " << level;

	u32 page_size = source.page_size();
	u32 page_x1 = floor( x >> level, page_size );
	u32 page_y1 = floor( y >> level, page_size );
	u32 page_x2 = page_x1 + std::max( ceiling( width >> level, page_size ), 1u );
	u32 page_y2 = page_y1 + std::max( ceiling( height >> level, page_size ), 1u );
	return region_type( page_x1, page_x2, page_y1, page_y2 );
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- pyramid_index --------------------------------------------------------------------------------
pyramid_index::pyramid_index( void )
	: x( 0 ), y( 0 ), level( 0 )
{
}

pyramid_index::pyramid_index( u32 x_, u32 y_, u8 level_ )
	: x( x_ ), y( y_ ), level( level_ )
{
}

bool operator <( const pyramid_index& i, const pyramid_index& j )
{
	return i.x < j.x || i.y < j.y || i.level < j.level;
}

//--- page_cache -----------------------------------------------------------------------------------
page_cache::page_cache
	( const device_type& device, thread_pool& pool_, image::type cache_format_, u16 page_size )
	: pool( pool_ ), cache_size( cache_width( device, page_size ) ), cache_format( cache_format_ ),
	cache( make_cache( device, cache_size, cache_format ) ), list(), map(), loads( 0 ), queue()
{
	for ( u32 y = 0; y != cache_size; y += page_size )
	{
		for ( u32 x = 0; x != cache_size; x += page_size )
			list.push_back( cache_type( x, y, key_type( 0, pyramid_index() ), false ) );
	}
}

image::type page_cache::format( void ) const
{
	return cache_format;
}

up_t page_cache::pending( void ) const
{
	return loads;
}

void page_cache::read( virtual_texture& texture, pyramid_index index, bool locked )
{
	atom_ptr< ooe::image > image( new ooe::image( texture.source.read( index ) ) );
	queue.enqueue( make_tuple( key_type( &texture, index ), locked, image ) );
}

void page_cache::load( virtual_texture& texture, const pyramid_index& index, bool locked )
{
	const key_type key( &texture, index );
	cache_map::iterator find = map.find( key );
	cache_list::iterator end = list.end();

	if ( find != map.end() )
	{
		find->second->_3 = locked;
		list.splice( end, list, find->second );
		return;
	}

	async( pool, make_function( *this, &page_cache::read ), texture, index, locked );
	// insert index in to map to prevent duplicate loads
	map.insert( cache_map::value_type( key, end ) );
	++loads;
}

void page_cache::unlock( virtual_texture& texture, const pyramid_index& index )
{
	cache_map::iterator find = map.find( key_type( &texture, index ) );

	if ( find != map.end() )
		find->second->_3 = false;
}

void page_cache::write( void )
{
	typedef std::vector< virtual_texture* > dirty_type;
	dirty_type dirty;
	pending_type value;

	for ( ; queue.dequeue( value ); --loads )
	{
		// find least-recently-used unlocked page
		cache_list::iterator page = list.begin();
		cache_list::iterator end = list.end();
		for ( ; page != end && page->_3; ++page ) {}

		if ( page == end )
			throw error::runtime( "virtual_texture: " ) << "All pages are locked";

		// if page has been previously assigned, evict entry
		if ( page->_2._0 )
		{
			virtual_texture& texture = *page->_2._0;
			const pyramid_index& index = page->_2._1;

			map.erase( page->_2 );
			write_pyramid( texture.pyramid, index, -1, -1, -1 );
			texture.bitset.set( index.level );
			dirty.push_back( &texture );
		}

		page->_2 = value._0;
		page->_3 = value._1;
		map[ page->_2 ] = page;
		list.splice( end, list, page );

		virtual_texture& texture = *page->_2._0;
		const pyramid_index& index = page->_2._1;
		f32 table_x = divide( page->_0, cache_size );
		f32 table_y = divide( page->_1, cache_size );
		f32 pow_level = std::pow( 2, texture.pyramid.size() - index.level - 1 );
		write_pyramid( texture.pyramid, index, table_x, table_y, pow_level );
		texture.bitset.set( index.level );
		dirty.push_back( &texture );

		cache->write( *value._2, page->_0, page->_1 );
	}

	for ( dirty_type::iterator i = dirty.begin(), end = dirty.end(); i != end; ++i )
		( *i )->write();
}

void page_cache::evict( virtual_texture& texture )
{
	for ( cache_list::iterator i = list.begin(), end = list.end(); i != end; ++i )
	{
		if ( i->_2._0 != &texture )
			continue;

		i->_2._0 = 0;
		i->_3 = false;
	}
}

//--- virtual_texture ------------------------------------------------------------------------------
virtual_texture::
	virtual_texture( const device_type& device, page_cache& cache_, physical_source& source_ )
	: cache( cache_ ), source( source_ ), table_size( table_width( device, source ) ),
	pyramid( make_pyramid( table_size ) ),
	table( device->texture( pyramid, texture::nearest, false ) ), bitset()
{
	u32 size = source.size();
	load( 0, 0, size, size, pyramid.size() - 1, true );
}

virtual_texture::~virtual_texture( void )
{
	cache.evict( *this );
}

void virtual_texture::input( const std::string& name, block_type& block ) const
{
	u16 page_size = source.page_size();
	block->input( name + ".page_ratio", divide( page_size, cache.cache_size ) );
	block->input( name + ".page_log2", f32( log2( page_size ) ) );
	block->input( name + ".page_cache", cache.cache );
	block->input( name + ".page_table", table );
}

void virtual_texture::load( u32 x, u32 y, u32 width, u32 height, u8 level, bool locked )
{
	region_type region = get_region( source, pyramid.size() - 1, x, y, width, height, level );

	// TODO: limit the load by some number of texels
	for ( u32 j = region._2; j != region._3; ++j )
	{
		for ( u32 i = region._0; i != region._1; ++i )
			cache.load( *this, pyramid_index( i, j, level ), locked );
	}
}

void virtual_texture::unlock( u32 x, u32 y, u32 width, u32 height, u8 level )
{
	region_type region = get_region( source, pyramid.size() - 1, x, y, width, height, level );

	for ( u32 j = region._2; j != region._3; ++j )
	{
		for ( u32 i = region._0; i != region._1; ++i )
			cache.unlock( *this, pyramid_index( i, j, level ) );
	}
}

void virtual_texture::write( void )
{
	if ( bitset.none() )
		return;

	for ( u8 i = 0, end = pyramid.size(); i != end; ++i )
	{
		if ( bitset[ i ] )
			table->write( pyramid.image( i ), 0, 0, i );
	}

	bitset.reset();
}

OOE_NAMESPACE_END( ( ooe ) )
