/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cstdlib>

#include "component/ui/tile_source.hpp"
#include "foundation/io/memory.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef std::vector< std::string > vector_type;
typedef std::string::const_iterator iterator_type;

vector_type words( iterator_type i, const iterator_type& end )
{
    vector_type vector;

    while ( i != end )
    {
        while ( i != end && isspace( *i ) )
            ++i;

        iterator_type begin = i;

        while ( i != end && !isspace( *i ) )
            ++i;

        vector.push_back( std::string( begin, i ) );
    }

    return vector;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- tile_source ----------------------------------------------------------------------------------
tile_source::tile_source( const std::string& root_, const decoder_map& map )
    : root( root_ ), type(), decoder(), size_(), format_(), page_size_(), level_limit()
{
    // TODO: use boost::ptree for configuration
    memory memory( root + "/metadata" );
    std::string metadata( memory.as< c8 >(), memory.size() );
    vector_type vector = words( metadata.begin(), --metadata.end() );

    if ( vector.size() != 2 )
        throw error::runtime( "tile_source: " ) << "Invalid metadata";

    decoder_map::const_iterator i = map.find( vector[ 0 ] );

    if ( i == map.end() )
        throw error::runtime( "tile_source: " ) <<
            "Unable to find decoder for image type \"" << vector[ 0 ] << "\"";

    type = i->first;
    decoder = i->second;
    s32 levels = std::atoi( vector[ 1 ].c_str() );

    if ( !levels )
        throw error::runtime( "tile_source: " ) << "Invalid number of mipmap levels: " << levels;

    level_limit = levels - 1;
    image image = decoder( root + "/0_0_0." + i->first );
    size_ = image.width << level_limit;
    format_ = image.format;
    page_size_ = image.width;
}

tile_source::~tile_source( void )
{
}

u32 tile_source::size( void ) const
{
    return size_;
}

image::type tile_source::format( void ) const
{
    return format_;
}

u16 tile_source::page_size( void ) const
{
    return page_size_;
}

image tile_source::read( const pyramid_index& index )
{
    std::string path( root );
    path << '/' << index.x << '_' << index.y << '_' << level_limit - index.level << '.' << type;
    return decoder( path );
}

OOE_NAMESPACE_END( ( ooe ) )
