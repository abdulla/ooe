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
#include "foundation/parallel/thread_pool.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef atom_ptr< image > image_ptr;
typedef std::vector< image_ptr > page_vector;

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
        { "dds",        dds::decode         },
        { "exr",        exr::decode         },
        { "jpeg",       jpeg::decode        },
        { "jpeg2000",   jpeg2000::decode    },
        { "png",        png::decode         }
    };

    decoder_pair* begin = pair;
    decoder_pair* end = begin + sizeof( pair ) / sizeof( *pair );
    decoder_pair* i = std::lower_bound( begin, end, type, less );

    if ( i == end )
        throw error::runtime( "tile_source: " ) << "Unknown image type \"" << type << '\"';

    return i->decoder;
}

std::string make_path
    ( const std::string& root, const std::string& type, const pyramid_index& index, u8 level_limit )
{
    std::string path( root );
    path << '/' << index.x << '_' << index.y << '_' << level_limit - index.level << '.' << type;
    return path;
}

page_vector make_pages( u32 pages_per_row, u16 page_size, image_format::type format )
{
    page_vector pages;
    pages.reserve( pages_per_row );

    for ( u32 i = 0; i != pages_per_row; ++i )
        pages.push_back( new image( page_size, page_size, format ) );

    return pages;
}

void encode_page( image_ptr image, const std::string& root, pyramid_index index, u8 level_limit )
{
    std::string path = make_path( root, "png", index, level_limit );
    png::encode( *image, descriptor( path, descriptor::write_new ) );
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

image_format::type tile_source::format( void ) const
{
    return format_;
}

image tile_source::read( const pyramid_index& index )
{
    return decoder( make_path( root, type, index, level_limit ) );
}

//--- make_tile ------------------------------------------------------------------------------------
void make_tile( const descriptor& desc, thread_pool& pool, const std::string& root, u16 page_size )
{
    reader_ptr reader = png::open( desc );
    u32 pages_per_row = ceiling< u32 >( reader->width, page_size );
    u8 pixel_size = ooe::pixel_size( *reader );
    up_t row_size = page_size * pixel_size;
    page_vector pages;

    for ( u32 y = 0, row = 0; reader->decode_row() || row; ++row %= page_size )
    {
        if ( !row )
        {
            // must set end to pages.size() to prevent encoding on first iteration
            for ( u32 x = 0, end = pages.size(); x != end; ++x )
            {
                result< void > r = async( pool, make_function( encode_page ), pages[ x ], root,
                    pyramid_index( x, y, 0 ), 0 );

                // synchronise on last page to avoid running too far ahead of encoding, this also
                // prevents excess memory usage, limiting it to approximately one row of pages
                if ( x == end - 1 )
                    r();
            }

            pages = make_pages( pages_per_row, page_size, reader->format );
            ++y;
        }

        for ( page_vector::iterator i = pages.begin(), end = pages.end(); i != end; ++i )
        {
            u8* row_pointer = ( *i )->as< u8 >() + row * row_size;
            up_t p = reader->read_pixels( row_pointer, page_size );

            if ( p != page_size )
                std::memset( row_pointer + p * pixel_size, 0, ( page_size - p ) * pixel_size );
        }
    }
}

OOE_NAMESPACE_END( ( ooe ) )
