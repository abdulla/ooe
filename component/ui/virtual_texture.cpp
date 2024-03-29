/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <cmath>
#include <iostream>

#include "component/ui/virtual_texture.hpp"
#include "foundation/parallel/thread_pool.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/binary.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef tuple< u32, u32, u32, u32 > region_type;
typedef std::pair< page_cache::page_map::iterator, page_cache::page_map::iterator > pair_type;
const image_format::type table_format = image_format::rg_s16;

u16 check_page_size( u16 page_size )
{
    if ( !is_bit_round( page_size ) )
        throw error::runtime( "physical_cache: " ) <<
            "Page size " << page_size << " is not a power of 2";

    return page_size;
}

physical_source& check_source( physical_source& source, page_cache& cache )
{
    image_format::type source_format = source.format();
    image_format::type cache_format = cache.format();
    u16 source_page_size = source.page_size();
    u16 cache_page_size = cache.page_size();

    if ( source_format != cache_format )
        throw error::runtime( "virtual_texture: " ) <<
            "Source format " << source_format << " != cache format " << cache_format;
    else if ( source_page_size != cache_page_size )
        throw error::runtime( "virtual_texture: " ) <<
            "Source page size " << source_page_size << " != cache page size " << cache_page_size;

    return source;
}

image make_image( u32 size )
{
    image out( size, size, table_format );

    for ( s16* i = out.as< s16 >(), * end = i + pixels( out ) * subpixels( out ); i != end; ++i )
        *i = -1;

    return out;
}

image_pyramid make_pyramid( const physical_source& source )
{
    u32 table_size = source.size() / source.page_size();
    image_pyramid pyramid( make_image( table_size ) );

    for ( u32 x = table_size >> 1; x; x >>= 1 )
        pyramid.insert( make_image( x ) );

    return pyramid;
}

texture_ptr make_table( const device_ptr& device, const image_pyramid& pyramid )
{
    return device->texture( pyramid, texture::nearest, false );
}

texture_array_ptr make_array( const device_ptr& device, image_format::type format, u16 page_size )
{
    u32 size = device->limit( device::array_size );
    return device->texture_array( image_metadata( page_size, page_size, format ), size );
}

void write_pyramid( image_pyramid& pyramid, const pyramid_index& index, s16 i, s16 exponent )
{
    image image = pyramid.image( index.level );
    s16* rg = pixel_at< s16 >( image, index.x, index.y );
    rg[ 0 ] = i;
    rg[ 1 ] = exponent;
}

region_type get_region
    ( const physical_source& source, u32 width, u32 height, u32 x, u32 y, u8 level, u8 level_limit )
{
    u32 size = source.size();

    if ( level > level_limit )
        throw error::runtime( "virtual_texture: " ) <<
            "Mipmap level " << level << " > maximum level " << level_limit;
    else if ( x + width > size || y + height > size )
        throw error::runtime( "virtual_texture: " ) <<
            "Load of ( " << width << ", " << height << ", " << x << ", " << y <<
            " ) is outside of source size " << size << " at level " << level;

    u32 page_size = source.page_size();
    u32 page_x1 = floor( x >> level, page_size );
    u32 page_y1 = floor( y >> level, page_size );
    u32 page_x2 = page_x1 + std::max( ceiling( width >> level, page_size ), 1u );
    u32 page_y2 = page_y1 + std::max( ceiling( height >> level, page_size ), 1u );
    return region_type( page_x1, page_x2, page_y1, page_y2 );
}

void insert_page( page_cache::page_map& pages, const page_cache::cache_list::iterator& i )
{
    pair_type pair = pages.equal_range( i->_1._0 );

    for ( ; pair.first != pair.second; ++pair.first )
    {
        if ( pair.first->second == i )
            return;
    }

    pages.insert( pair.first, std::make_pair( i->_1._0, i ) );
}

void erase_page( page_cache::page_map& pages, const page_cache::cache_list::iterator& i )
{
    pair_type pair = pages.equal_range( i->_1._0 );

    for ( ; pair.first != pair.second; ++pair.first )
    {
        if ( pair.first->second == i )
        {
            pages.erase( pair.first );
            return;
        }
    }
}

OOE_ANONYMOUS_END( ( ooe ) )

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
    return i.x < j.x || ( i.x == j.x && ( i.y < j.y || ( i.y == j.y && i.level < j.level ) ) );
}

//--- page_cache -----------------------------------------------------------------------------------
page_cache::page_cache
    ( const device_ptr& device, thread_pool& pool_, u16 size, image_format::type cache_format )
    : pool( pool_ ), page_size_( check_page_size( size ) ), format_( cache_format ),
    array( make_array( device, format_, page_size_ ) ), list(), map(), pages(), loads( 0 ), queue()
{
    u16 array_size = device->limit( device::array_size );

    if ( !array_size )
        throw error::runtime( "page_cache: " ) << "Device does not support texture arrays";

    for ( u16 i = 0; i != array_size; ++i )
        list.push_back( cache_type( i, key_type( 0, pyramid_index() ), false ) );
}

u16 page_cache::page_size( void ) const
{
    return page_size_;
}

image_format::type page_cache::format( void ) const
{
    return format_;
}

up_t page_cache::pending( void ) const
{
    return loads;
}

void page_cache::read( virtual_texture& texture, pyramid_index index, bool locked )
{
    image_ptr image;
    OOE_PRINT( "page_cache", image = image_ptr( new ooe::image( texture.source.read( index ) ) ) );
    queue.push( pending_type( key_type( &texture, index ), locked, image ) );
}

void page_cache::load( virtual_texture& texture, const pyramid_index& index, bool locked )
{
    cache_list::iterator null;
    cache_map::value_type value( key_type( &texture, index ), null );
    std::pair< cache_map::iterator, bool > pair = map.insert( value );

    if ( pair.second )
    {
        async( pool, make_function( *this, &page_cache::read ), texture, index, locked );
        ++loads;
    }
    else if ( pair.first->second != null )
        list.splice( list.end(), list, pair.first->second );
}

void page_cache::unlock( virtual_texture& texture, const pyramid_index& index )
{
    cache_map::iterator find = map.find( key_type( &texture, index ) );

    if ( find != map.end() )
        find->second->_2 = false;
}

void page_cache::write( void )
{
    typedef std::vector< virtual_texture* > dirty_type;
    dirty_type dirty;
    pending_type value;

    for ( ; queue.try_pop( value ); --loads )
    {
        // if read failed, remove from map
        if ( !value._2 )
        {
            map.erase( value._0 );
            continue;
        }

        // find least-recently-used unlocked page
        cache_list::iterator page = list.begin();
        cache_list::iterator end = list.end();
        for ( ; page != end && page->_2; ++page ) {}

        if ( page == end )
            throw error::runtime( "page_cache: " ) << "All pages are locked";

        // if page has been previously assigned, evict entry
        if ( page->_1._0 )
        {
            virtual_texture& texture = *page->_1._0;
            const pyramid_index& index = page->_1._1;

            write_pyramid( texture.pyramid, index, -1, -1 );
            texture.bitset.set( index.level );
            dirty.push_back( &texture );

            map.erase( page->_1 );
            erase_page( pages, page );
        }

        page->_1 = value._0;
        page->_2 = value._1;
        map[ value._0 ] = page;
        insert_page( pages, page );
        list.splice( end, list, page );

        virtual_texture& texture = *page->_1._0;
        const pyramid_index& index = page->_1._1;
        s16 exponent = texture.pyramid.size() - index.level - 1;

        write_pyramid( texture.pyramid, index, page->_0, exponent );
        texture.bitset.set( index.level );
        dirty.push_back( &texture );

        array->write( *value._2, 0, 0, page->_0 );
    }

    for ( dirty_type::iterator i = dirty.begin(), end = dirty.end(); i != end; ++i )
        ( *i )->write();
}

void page_cache::evict( virtual_texture& texture )
{
    pair_type pair = pages.equal_range( &texture );
    cache_list::iterator begin = list.begin();

    while ( pair.first != pair.second )
    {
        cache_list::iterator i = pair.first->second;
        pages.erase( pair.first++ );

        i->_1._0 = 0;
        i->_2 = false;
        list.splice( begin, list, i );
    }
}

//--- virtual_texture ------------------------------------------------------------------------------
virtual_texture::
    virtual_texture( const device_ptr& device, page_cache& cache_, physical_source& source_ )
    : cache( cache_ ), source( check_source( source_, cache ) ), pyramid( make_pyramid( source ) ),
    table( make_table( device, pyramid ) ), bitset()
{
    u32 size = source.size();
    load( size, size, 0, 0, pyramid.size() - 1, true );
}

virtual_texture::~virtual_texture( void )
{
    cache.evict( *this );
}

void virtual_texture::input( block_ptr& block, const std::string& name ) const
{
    s32 begin = log2f( source.page_size() );
    s32 end = log2f( source.size() ) + 1;
    block->input( name + ".bias_range", begin, end );
    block->input( name + ".page_table", table );
    block->input( name + ".page_cache", cache.array );
}

void virtual_texture::load( u32 width, u32 height, u32 x, u32 y, u8 level, bool locked )
{
    region_type region = get_region( source, width, height, x, y, level, pyramid.size() - 1 );

    // TODO: limit the load by some number of texels
    for ( u32 j = region._2; j != region._3; ++j )
    {
        for ( u32 i = region._0; i != region._1; ++i )
            cache.load( *this, pyramid_index( i, j, level ), locked );
    }
}

void virtual_texture::unlock( u32 width, u32 height, u32 x, u32 y, u8 level )
{
    region_type region = get_region( source, width, height, x, y, level, pyramid.size() - 1 );

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
