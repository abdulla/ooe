/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <boost/functional/hash.hpp>

#include "component/ui/glyph_source.hpp"

OOE_NAMESPACE_BEGIN( ( std )( tr1 ) )

template< typename t0, typename t1 >
    inline up_t hash< tuple< t0, t1 > >::operator ()( tuple< t0, t1 > tuple ) const
{
    up_t result = boost::hash< t0 >()( tuple._0 );
    boost::hash_combine( result, tuple._1 );
    return result;
}

OOE_NAMESPACE_END( ( std )( tr1 ) )

OOE_ANONYMOUS_BEGIN( ( ooe ) )

const u32 page_size_ = 256;
const image_format::type format_ = image_format::rgba_u8;

struct source_metric
    : public font::metric
{
    bool valid;

    source_metric( const font::metric& metric_, bool valid_ )
        : metric( metric_ ), valid( valid_ )
    {
    }
};

c8 hyphenate( c8 c )
{
    return std::isspace( c ) ? '-' : std::tolower( c );
}

std::string get_root( const std::string& base, const font::face& face )
{
    std::string suffix = face.string( font::face::family ) + '-' + face.string( font::face::style );
    std::transform( suffix.begin(), suffix.end(), suffix.begin(), hyphenate );
    std::string path = canonical_path( base ) + '/' + suffix;

    if ( !exists( path ) )
        make_directory( path );

    return path;
}

memory open_memory( const std::string& root, const font::face& face )
{
    descriptor desc( root + "/metric", descriptor::read_write );
    up_t size = sizeof( source_metric ) * face.number( font::face::glyphs );

    if ( desc.size() < size )
        desc.resize( size );

    return memory( desc, memory::read_write );
}

u32 get_size( const device_ptr& device )
{
    return device->limit( device::texture_size ) * page_size_;
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- glyph_source ---------------------------------------------------------------------------------
glyph_source::glyph_source( const device_ptr& device, const font::face& face_,
    const std::string& base )
    : mutex(), face( face_ ), root( get_root( base, face ) ), memory( open_memory( root, face ) ),
    index_cache( 1 << 9 ), kerning_cache( 1 << 14 ), size_( get_size( device ) )
{
}

glyph_source::~glyph_source( void )
{
}

u32 glyph_source::size( void ) const
{
    return size_;
}

u16 glyph_source::page_size( void ) const
{
    return page_size_;
}

image_format::type glyph_source::format( void ) const
{
    return format_;
}

u32 glyph_source::glyph_index( u32 code_point ) const
{
    index_type::const_iterator i = index_cache.find( code_point );

    if ( i != index_cache.end() )
        return i->second;

    u32 index;
    {
        lock lock( mutex );
        index = face.glyph_index( code_point );
    }

    index_cache.insert( std::make_pair( code_point, index ) );
    return index;
}

f32 glyph_source::kerning( u32 left, u32 right ) const
{
    kerning_type::key_type key( left, right );
    kerning_type::const_iterator i = kerning_cache.find( key );

    if ( i != kerning_cache.end() )
        return i->second;

    f32 delta;
    {
        lock lock( mutex );
        delta = face.kerning( left, right );
    }

    kerning_cache.insert( std::make_pair( key, delta ) );
    return delta;
}

glyph_coord glyph_source::glyph_coord( u32 glyph_index, u16 size ) const
{
    lock lock( mutex );
    font::metric metric = face.metric( glyph_index );
    return glyph_source( metric );
}

OOE_NAMESPACE_END( ( ooe ) )
