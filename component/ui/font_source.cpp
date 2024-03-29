/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <cctype>
#include <cmath>

#include <boost/functional/hash.hpp>

#include "component/ui/font_source.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/io/file.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/binary.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/miscellany.hpp"

OOE_NAMESPACE_BEGIN( ( std )( tr1 ) )

template<>
    inline up_t hash< tuple< u32, u32 > >::operator ()( tuple< u32, u32 > t ) const
{
    up_t result = boost::hash< u32 >()( t._0 );
    boost::hash_combine( result, t._1 );
    return result;
}

OOE_NAMESPACE_END( ( std )( tr1 ) )

OOE_ANONYMOUS_BEGIN( ( ooe ) )

const u16 page_wide = 256;
const image_format::type image_type = image_format::rgba_u8;

struct source_metric
    : public font::metric
{
    u16 width;
    u16 height;
    bool valid;

    source_metric( const font::metric& metric_, u16 width_, u16 height_, bool valid_ )
        : metric( metric_ ), width( width_ ), height( height_ ), valid( valid_ )
    {
    }
};

c8 transform( c8 c )
{
    return std::isspace( c ) ? '-' : std::tolower( c );
}

u16 check_size( u16 face_size )
{
    if ( is_bit_round( face_size ) )
        return face_size;

    throw error::runtime( "font_source: " ) << "Font size " << face_size << " is not a power of 2";
}

std::string get_root( const std::string& root, const font::face& face )
{
    std::string suffix = face.family() + '-' + face.style();
    std::transform( suffix.begin(), suffix.end(), suffix.begin(), transform );
    std::string path = canonical_path( root ) + '/' + suffix;

    if ( !exists( path ) )
        make_directory( path );

    return path;
}

u32 get_size( const font::face& face, u16 face_size )
{
    u32 root = std::sqrt( face.size() );
    return bit_round_up( root ) * face_size;
}

memory open_memory( const std::string& root, u32 glyphs, u8 levels )
{
    descriptor desc( root + "/metric", descriptor::read_write );
    up_t size = sizeof( source_metric ) * glyphs * levels;

    if ( desc.size() < size )
        desc.resize( size );

    return memory( desc, memory::read_write );
}

void copy_row( const u8* i, u8* j, u16 size )
{
    for ( const u8* end = i + size * 3; i != end; i += 3, j += 4 )
    {
        j[ 0 ] = i[ 0 ];
        j[ 1 ] = i[ 1 ];
        j[ 2 ] = i[ 2 ];
        j[ 3 ] = ( u32( i[ 0 ] ) + i[ 1 ] + i[ 2 ] + 1 ) / 3;
    }
}

void copy_partial( image& image, const font::bitmap& bitmap, u16 x, u16 y )
{
    if ( x >= bitmap.width || y >= bitmap.height )
        return;

    u8* target = image.as< u8 >();
    const u8* source = bitmap.data + x * 3 + y * bitmap.pitch;
    u16 width = std::min< u16 >( page_wide, bitmap.width - x );
    u16 height = std::min< u16 >( page_wide, bitmap.height - y );

    for ( u16 i = 0; i != height; ++i, target += row_size( image ), source += bitmap.pitch )
        copy_row( source + ( x ? 0 : 3 ), target, width );
}

void copy_square( image& image, const font::bitmap& bitmap, u16 x, u16 y )
{
    u8* target = image.as< u8 >() + ( x + y * image.width ) * 4;
    const u8* source = bitmap.data;
    u16 width = std::min< u16 >( page_wide - x, bitmap.width );
    u16 height = std::min< u16 >( page_wide - y, bitmap.height );

    for ( u16 i = 0; i != height; ++i, target += row_size( image ), source += bitmap.pitch )
        copy_row( source + 3, target, width );
}

const image& write_image( const image& image, const std::string& path )
{
    file file( descriptor( path, descriptor::write_new ) );
    file.write( image.get(), byte_size( image ) );
    return image;
}

source_metric& read_metric( const memory& memory, u32 glyph_index, u32 glyphs, u8 level_inverse )
{
    return memory.as< source_metric >()[ glyph_index + glyphs * level_inverse ];
}

void write_metric( source_metric& sm, const font::metric& metric, const font::bitmap& bitmap )
{
    sm = source_metric( metric, bitmap.width, bitmap.height, true );
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- font_source ----------------------------------------------------------------------------------
font_source::font_source( const font::face& face_, u16 face_size_, const std::string& root_ )
    : mutex(), face( face_ ), face_size( check_size( face_size_ ) ),
    root( get_root( root_, face ) ), source_size( get_size( face, face_size ) ),
    glyphs( face.size() ), level_limit( log2f( source_size / page_wide ) ),
    index_cache( 1 << 9 ), kerning_cache( 1 << 14 ),
    memory( open_memory( root, glyphs, level_limit + 1 ) )
{
}

font_source::~font_source( void )
{
}

u32 font_source::size( void ) const
{
    return source_size;
}

u16 font_source::page_size( void ) const
{
    return page_wide;
}

image_format::type font_source::format( void ) const
{
    return image_type;
}

u16 font_source::font_size( void ) const
{
    return face_size;
}

u32 font_source::glyph_index( u32 code_point ) const
{
    index_cache_type::const_iterator i = index_cache.find( code_point );

    if ( i != index_cache.end() )
        return i->second;

    lock lock( mutex );
    u32 index = face.glyph_index( code_point );
    index_cache.insert( std::make_pair( code_point, index ) );
    return index;
}

f32 font_source::kerning( u32 left, u32 right ) const
{
    kerning_cache_type::key_type key( left, right );
    kerning_cache_type::const_iterator i = kerning_cache.find( key );

    if ( i != kerning_cache.end() )
        return i->second;

    lock lock( mutex );
    f32 advance = face.kerning( left, right );
    kerning_cache.insert( std::make_pair( key, advance ) );
    return advance;
}

font_source::glyph_type font_source::glyph( u32 glyph_index_, u8 level ) const
{
    if ( glyph_index_ >= glyphs )
        throw error::runtime( "font_source: " ) <<
            "Glyph index " << glyph_index_ << " out of range, " << glyphs << " glyphs";
    else if ( level > level_limit )
        throw error::runtime( "font_source: " ) <<
            "Mipmap level " << level << " > maximum level " << level_limit;

    u8 level_inverse = level_limit - level;
    source_metric& sm = read_metric( memory, glyph_index_, glyphs, level_inverse );

    if ( !sm.valid )
    {
        lock lock( mutex );
        font::metric metric = face.metric( glyph_index_ );
        font::bitmap bitmap = face.bitmap( glyph_index_, face_size >> level );
        write_metric( sm, metric, bitmap );
    }

    u32 glyphs_per_row = source_size / face_size;
    return glyph_type
    (
        sm,
        sm.width,
        sm.height,
        ( glyph_index_ % glyphs_per_row ) * face_size,
        ( glyph_index_ / glyphs_per_row ) * face_size
    );
}

image font_source::read( const pyramid_index& index )
{
    // note: arguments will be verified by virtual_texture, no need to here
    image image( page_wide, page_wide, image_type );
    std::string path( root );
    u8 level_inverse = level_limit - index.level;
    path << '/' << index.x << '_' << index.y << '_' << level_inverse << ".raw";

    if ( exists( path ) )
    {
        file file( path );
        file.read( image.get(), byte_size( image ) );
        return image;
    }

    std::memset( image.get(), 0, byte_size( image ) );
    u16 level_size = face_size >> index.level;
    u32 glyphs_per_row = source_size / face_size;
    u32 base_index =
        index.x * page_wide / level_size + ( index.y * page_wide / level_size ) * glyphs_per_row;

    if ( base_index >= glyphs )
        return write_image( image, path );

    if ( level_size >= page_wide )
    {
        u32 pages_per_glyph = level_size / page_wide;
        u32 x_offset = ( index.x % pages_per_glyph ) * page_wide;
        u32 y_offset = ( index.y % pages_per_glyph ) * page_wide;

        lock lock( mutex );
        font::metric metric = face.metric( base_index );
        font::bitmap bitmap = face.bitmap( base_index, level_size );
        copy_partial( image, bitmap, x_offset, y_offset );
        source_metric& sm = read_metric( memory, base_index, glyphs, level_inverse );
        write_metric( sm, metric, bitmap );
        return write_image( image, path );
    }

    for ( u32 j = 0, glyphs_per_page = page_wide / level_size; j != glyphs_per_page; ++j )
    {
        for ( u32 i = 0; i != glyphs_per_page; ++i )
        {
            u32 glyph_index_ = base_index + i + j * glyphs_per_row;

            if ( glyph_index_ >= glyphs )
                return write_image( image, path );

            lock lock( mutex );
            font::metric metric = face.metric( glyph_index_ );
            font::bitmap bitmap = face.bitmap( glyph_index_, level_size );
            copy_square( image, bitmap, i * level_size, j * level_size );
            source_metric& sm = read_metric( memory, glyph_index_, glyphs, level_inverse );
            write_metric( sm, metric, bitmap );
        }
    }

    return write_image( image, path );
}

OOE_NAMESPACE_END( ( ooe ) )
