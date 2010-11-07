/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "component/ui/tile_source.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- tile_source ----------------------------------------------------------------------------------
tile_source::tile_source( const std::string& root_, const decoder_map& map )
    : root( root_ ), type(), decoder(), size_(), format_(), page_size_(), level_limit()
{
    boost::property_tree::ptree pt;
    read_json( canonical_path( root + "/metadata" ), pt );

    type = pt.get< std::string >( "type" );
    level_limit = pt.get< u8 >( "level_limit" );
    decoder_map::const_iterator i = map.find( type );

    if ( i == map.end() )
        throw error::runtime( "tile_source: " ) <<
            "Unable to find decoder for image type \"" << type << "\"";

    decoder = i->second;
    image image = decoder( root + "/0_0_0." + type );
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
