/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "component/ui/tile_source.hpp"
#include "foundation/image/dds.hpp"
#include "foundation/image/exr.hpp"
#include "foundation/image/jpeg.hpp"
#include "foundation/image/jpeg2000.hpp"
#include "foundation/image/png.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

struct decoder_pair
{
    const c8* type;
    decoder_type decoder;
};

bool less( const decoder_pair& pair, const std::string& type )
{
    return std::strcmp( pair.type, type.c_str() ) < 0;
}

decoder_type find( const std::string& type )
{
    decoder_pair pair[] =
    {
        { "dds",        compressed_decode< dds::decode >        },
        { "exr",        uncompressed_decode< exr::decode >      },
        { "jpeg",       uncompressed_decode< jpeg::decode >     },
        { "jpeg2000",   uncompressed_decode< jpeg2000::decode > },
        { "png",        uncompressed_decode< png::decode >      }
    };

    decoder_pair* begin = pair;
    decoder_pair* end = begin + sizeof( pair ) / sizeof( *pair );
    decoder_pair* i = std::lower_bound( begin, end, type, less );

    if ( i == end )
        throw error::runtime( "tile_source: " ) << "Unknown image type \"" << type << '\"';

    return i->decoder;
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- tile_source ----------------------------------------------------------------------------------
tile_source::tile_source( const std::string& root_ )
    : root( root_ ), type(), decoder(), size_(), page_size_(), format_(), level_limit()
{
    boost::property_tree::ptree pt;
    read_json( canonical_path( root + "/metadata" ), pt );

    type = pt.get< std::string >( "type" );
    level_limit = pt.get< u8 >( "level_limit" );

    decoder = find( type );
    image image = decoder( root + "/0_0_0." + type );
    size_ = image.width << level_limit;
    page_size_ = image.width;
    format_ = image.format;
}

tile_source::~tile_source( void )
{
}

u32 tile_source::size( void ) const
{
    return size_;
}

u16 tile_source::page_size( void ) const
{
    return page_size_;
}

image::type tile_source::format( void ) const
{
    return format_;
}

image tile_source::read( const pyramid_index& index )
{
    std::string path( root );
    path << '/' << index.x << '_' << index.y << '_' << level_limit - index.level << '.' << type;
    return decoder( path );
}

OOE_NAMESPACE_END( ( ooe ) )
