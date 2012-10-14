/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "component/ui/tile_source.hpp"
#include "foundation/image/dds.hpp"
#include "foundation/image/jpeg.hpp"
#include "foundation/image/jpeg2000.hpp"
#include "foundation/image/png.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/io/file.hpp"
#include "foundation/parallel/thread_pool.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef shared_ptr< image > image_ptr;
typedef std::vector< image_ptr > page_vector;

template< typename t >
    struct codec_pair
{
    typedef t codec_type;

    const c8* type;
    codec_type codec;
};

template< typename t >
    bool less( const t& pair, const std::string& type )
{
    return std::strcmp( pair.type, type.c_str() ) < 0;
}

template< typename t >
    typename t::codec_type find( const std::string& type, t* begin, t* end )
{
    t* i = std::lower_bound( begin, end, type, less< t > );

    if ( i == end )
        throw error::runtime( "tile_source: " ) << "Unknown image type \"" << type << '\"';

    return i->codec;
}

void write_metadata
    ( const std::string& root, const std::string& type, const reader_ptr reader, u32 level_limit )
{
    file file( descriptor( root + "/metadata", descriptor::write_new ) );
    file <<
        "{\n"
        "    \"type\": \"" << type << "\",\n"
        "    \"level_limit\": " << level_limit << ",\n"
        "    \"width\": " << reader->width << ",\n"
        "    \"height\": " << reader->height << "\n"
        "}\n";
}

void write_raw( const std::string& root, const pyramid_index& index, const image& image )
{
    descriptor desc( make_path( root, index, "raw" ), descriptor::write_new );
    file( desc ).write( image.get(), byte_size( image ) );
}

void read_raw( const std::string& root, const pyramid_index& index, image& image )
{
    std::string path = make_path( root, index, "raw" );

    if ( exists( path ) )
        file( path ).read( image.get(), byte_size( image ) );
    else
        std::memset( image.get(), 0, byte_size( image ) );
}

void resample_page( const std::string& root, const pyramid_index& index, u16 page_size,
    image_format::type format )
{
    image source( page_size, page_size, format );
    image target( page_size, page_size, format );
    u32 half = page_size / 2;
    u8 subpixels = ooe::subpixels( format );

    for ( u8 i = 0; i != 4; ++i )
    {
        u32 x_off = ( i % 2 ) * half;
        u32 y_off = ( i / 2 ) * half;
        pyramid_index pi( index.x + i % 2, index.y + i / 2, index.level );
        read_raw( root, pi, source );

        for ( u32 y = 0, y_end = half; y != y_end; ++y )
        {
            for ( u32 x = 0, x_end = half; x != x_end; ++x )
            {
                u8* a = pixel_at< u8 >( source, x * 2, y * 2 );
                u8* b = pixel_at< u8 >( source, x * 2 + 1, y * 2 );
                u8* c = pixel_at< u8 >( source, x * 2, y * 2 + 1 );
                u8* d = pixel_at< u8 >( source, x * 2 + 1, y * 2 + 1 );
                u8* t = pixel_at< u8 >( target, x + x_off, y + y_off );

                for ( u8 j = 0; j != subpixels; ++j )
                    t[ j ] = ( u32( a[ j ] ) + b[ j ] + c[ j ] + d[ j ] ) / 4;
            }
        }
    }

    pyramid_index pi( index.x / 2, index.y / 2, index.level - 1 );
    write_raw( root, pi, target );
}

void resample_row
    ( std::string root, u32 size, u32 y, u8 level, u16 page_size, image_format::type format )
{
    for ( u32 x = 0; x < size; x += 2 )
        resample_page( root, pyramid_index( x, y, level ), page_size, format );
}

void encode_row( std::string root, std::string type, encoder_type encoder, u32 size, u32 y,
    u8 level, u16 page_size, image_format::type format )
{
    image image( page_size, page_size, format );

    for ( u32 x = 0; x != size; ++x )
    {
        pyramid_index index( x, y, level );
        std::string path = make_path( root, index, "raw" );

        file( path ).read( image.get(), byte_size( image ) );
        erase( path );

        descriptor desc( make_path( root, index, type ), descriptor::write_new );
        encoder( image, desc );
    }
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- tile_source ----------------------------------------------------------------------------------
tile_source::tile_source( const std::string& root_ )
    : root( root_ ), type(), decoder(), size_(), page_size_(), format_(), level_limit(), width(),
    height()
{
    boost::property_tree::ptree pt;
    read_json( canonical_path( root + "/metadata" ), pt );

    type = pt.get< std::string >( "type" );
    level_limit = pt.get< u8 >( "level_limit" );
    width = pt.get< u32 >( "width" );
    height = pt.get< u32 >( "height" );

    decoder = find_decoder( type );
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

tile_source::area_tuple tile_source::area( void ) const
{
    return area_tuple( width, height );
}

image tile_source::read( const pyramid_index& i )
{
    pyramid_index index( i.x, i.y, level_limit - i.level );
    return decoder( make_path( root, index, type ) );
}

//--- find_open ------------------------------------------------------------------------------------
open_type find_open( const std::string& type )
{
    codec_pair< open_type > pairs[] =
    {
        {   "jpg", jpeg::open   },
        {   "png", png::open    }
    };

    return find( type, pairs, pairs + sizeof( pairs ) / sizeof( *pairs ) );
}

//--- find_decoder ---------------------------------------------------------------------------------
decoder_type find_decoder( const std::string& type )
{
    codec_pair< decoder_type > pairs[] =
    {
        {   "dds", dds::decode      },
        {   "jp2", jpeg2000::decode },
        {   "jpg", jpeg::decode     },
        {   "png", png::decode      }
    };

    return find( type, pairs, pairs + sizeof( pairs ) / sizeof( *pairs ) );
}

//--- find_encoder ---------------------------------------------------------------------------------
encoder_type find_encoder( const std::string& type )
{
    codec_pair< encoder_type > pairs[] =
    {
        {   "dds", dds::encode      },
        {   "jp2", jpeg2000::encode },
        {   "jpg", jpeg::encode     },
        {   "png", png::encode      }
    };

    return find( type, pairs, pairs + sizeof( pairs ) / sizeof( *pairs ) );
}

//--- make_path ------------------------------------------------------------------------------------
std::string make_path
    ( const std::string& root, const pyramid_index& index, const std::string& type )
{
    std::string path( root );
    path << '/' << index.x << '_' << index.y << '_' << index.level << '.' << type;
    return path;
}

//--- make_tile ------------------------------------------------------------------------------------
void make_tile( const descriptor& desc, thread_pool& pool, const std::string& root,
    const std::string& type, u16 page_size )
{
    if ( exists( root ) )
        throw error::runtime( "make_tile: " ) << "Directory \"" << root << "\" exists";

    reader_ptr reader = find_open( type )( desc );
    image_format::type format = reader->format;
    u32 page_limit = ceiling< u32 >( std::max( reader->width, reader->height ), page_size );
    u8 level_limit = std::ceil( log2f( page_limit ) );
    u32 pages_per_row = ceiling< u32 >( reader->width, page_size );
    u8 pixel_size = ooe::pixel_size( *reader );
    up_t row_size = page_size * pixel_size;

    make_directory( root );
    write_metadata( root, type, reader, level_limit );
    page_vector pages;
    pages.reserve( pages_per_row );

    for ( u32 i = 0; i != pages_per_row; ++i )
        pages.push_back( image_ptr( new image( page_size, page_size, format ) ) );

    // split
    for ( u32 i = 0, row = 0; reader->decode_row() || row; ++i, row = i % page_size )
    {
        for ( page_vector::iterator j = pages.begin(), end = pages.end(); j != end; ++j )
        {
            u8* row_pointer = ( *j )->as< u8 >() + row * row_size;
            up_t p = reader->read_pixels( row_pointer, page_size );

            if ( p != page_size )
                std::memset( row_pointer + p * pixel_size, 0, ( page_size - p ) * pixel_size );
        }

        for ( u32 x = 0, y = i / page_size; row == page_size - 1u && x != pages_per_row; ++x )
            write_raw( root, pyramid_index( x, y, level_limit ), *pages[ x ] );
    }

    u32 width_limit = ceiling< u32 >( reader->width, page_size );
    u32 height_limit = ceiling< u32 >( reader->height, page_size );
    u32 w = width_limit;
    u32 h = height_limit;
    reader.reset();
    page_vector().swap( pages );

    // resample
    for ( u8 level = level_limit; level; --level, w = ceiling( w, 2u ), h = ceiling( h, 2u ) )
    {
        for ( u32 y = 0; y < h; y += 2 )
        {
            result< void > sync = async( pool, make_function( resample_row ),
                root, w, y, level, page_size, format );

            if ( y + 2 >= h )
                sync();
        }
    }

    encoder_type encoder = find_encoder( type );
    w = width_limit;
    h = height_limit;

    // encode
    for ( s8 level = level_limit; level != -1; --level, w = ceiling( w, 2u ), h = ceiling( h, 2u ) )
    {
        for ( u32 y = 0; y != h; ++y )
        {
            result< void > sync = async( pool, make_function( encode_row ),
                root, type, encoder, w, y, level, page_size, format );

            if ( y == h - 1 )
                sync();
        }
    }
}

OOE_NAMESPACE_END( ( ooe ) )
