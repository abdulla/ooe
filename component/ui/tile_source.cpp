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
    ( const std::string& root, const pyramid_index& index, const std::string& type )
{
    std::string path( root );
    path << '/' << index.x << '_' << index.y << '_' << index.level << '.' << type;
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

void encode( const std::string& root, pyramid_index index, image_ptr image )
{
    descriptor desc( make_path( root, index, "png" ), descriptor::write_new );
    png::encode( *image, desc );
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

image tile_source::read( const pyramid_index& i )
{
    pyramid_index index( i.x, i.y, level_limit - i.level );
    return decoder( make_path( root, index, type ) );
}

//--- make_tile ------------------------------------------------------------------------------------
void make_tile( const descriptor& desc, thread_pool& pool, const std::string& root, u16 page_size )
{
    reader_ptr reader = png::open( desc );
    u8 level_limit = log2f( std::max( reader->width, reader->height ) / page_size );
    u32 pages_per_row = ceiling< u32 >( reader->width, page_size );
    u8 pixel_size = ooe::pixel_size( *reader );
    up_t row_size = page_size * pixel_size;
    page_vector pages;

    for ( u32 i = 0, row = 0; reader->decode_row() || row; ++i, row = i % page_size )
    {
        if ( !row )
            pages = make_pages( pages_per_row, page_size, reader->format );

        for ( page_vector::iterator j = pages.begin(), end = pages.end(); j != end; ++j )
        {
            u8* row_pointer = ( *j )->as< u8 >() + row * row_size;
            up_t p = reader->read_pixels( row_pointer, page_size );

            if ( p != page_size )
                std::memset( row_pointer + p * pixel_size, 0, ( page_size - p ) * pixel_size );
        }

        for ( u32 x = 0, y = i / page_size; row == page_size - 1u && x != pages_per_row; ++x )
        {
            pyramid_index index( x, y, level_limit - 0 );
            result< void > r = async( pool, make_function( encode ), root, index, pages[ x ] );

            // synchronise on last page to avoid running too far ahead of encoding, this also
            // prevents excess memory usage, limiting it to approximately one row of pages
            if ( x == pages_per_row - 1 )
                r();
        }
    }
}

OOE_NAMESPACE_END( ( ooe ) )
