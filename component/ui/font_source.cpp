/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <cctype>
#include <cmath>

#include "component/ui/font_source.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/io/file.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/binary.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/miscellany.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

const u16 page_wide = 256;
const image_format::type image_type = image_format::rgba_u8;

struct source_metric
    : public font::metric
{
    bool valid;

    source_metric( const font::metric& metric_, bool valid_ )
        : metric( metric_ ), valid( valid_ )
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
    std::string suffix = face.string( font::face::family ) + '-' + face.string( font::face::style );
    std::transform( suffix.begin(), suffix.end(), suffix.begin(), transform );
    std::string path = canonical_path( root ) + '/' + suffix;

    if ( !exists( path ) )
        make_directory( path );

    return path;
}

u32 get_size( const font::face& face, u16 face_size )
{
    u32 root = std::sqrt( face.number( font::face::glyphs ) );
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
    if ( x >= bitmap.metric.width || y >= bitmap.metric.height )
        return;

    u8* target = image.as< u8 >();
    const u8* source = bitmap.data + x * 3 + y * bitmap.pitch;

    u16 width = std::min< u16 >( page_wide, bitmap.metric.width - x );
    u16 height = std::min< u16 >( page_wide, bitmap.metric.height - y );

    for ( u16 i = 0; i != height;
        ++i, target += row_size( image ), source += bitmap.pitch )
        copy_row( source + ( x ? 0 : 3 ), target, width );
}

void copy_square( image& image, const font::bitmap& bitmap, u16 x, u16 y )
{
    u8* target = image.as< u8 >() + ( x + y * image.width ) * 4;
    const u8* source = bitmap.data;

    for ( u16 i = 0; i != bitmap.metric.height;
        ++i, target += row_size( image ), source += bitmap.pitch )
        copy_row( source + 3, target, bitmap.metric.width );
}

const image& write_image( const image& image, const std::string& path )
{
    file file( descriptor( path, descriptor::write_new ) );
    file.write( image.get(), byte_size( image ) );
    return image;
}

void write_metric( const memory& memory, u32 glyph_index, u32 glyphs, u8 level_inverse,
    const font::metric& metric )
{
    source_metric* metrics = memory.as< source_metric >() + glyph_index + glyphs * level_inverse;
    *metrics = source_metric( metric, true );
}

source_metric& read_metric( const memory& memory, u32 glyph_index, u32 glyphs, u8 level_inverse )
{
    return memory.as< source_metric >()[ glyph_index + glyphs * level_inverse ];
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- font_source ----------------------------------------------------------------------------------
font_source::font_source( const font::face& face_, u16 face_size_, const std::string& root_ )
    : face( face_ ), face_size( check_size( face_size_ ) ), root( get_root( root_, face ) ),
    source_size( get_size( face, face_size ) ), glyphs( face.number( font::face::glyphs ) ),
    level_limit( log2f( source_size / page_wide ) ), mutex(),
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
    lock lock( mutex );
    return face.glyph_index( code_point );
}

f32 font_source::kerning( u32 left, u32 right, u8 level ) const
{
    lock lock( mutex );
    return face.kerning( left, right, face_size >> level );
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
    source_metric& metric = read_metric( memory, glyph_index_, glyphs, level_inverse );

    if ( !metric.valid )
    {
        lock lock( mutex );
        font::bitmap bitmap = face.bitmap( glyph_index_, face_size >> level );
        metric = source_metric( bitmap.metric, true );
    }

    u32 glyphs_per_row = source_size / face_size;
    return glyph_type
    (
        metric,
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
        font::bitmap bitmap = face.bitmap( base_index, level_size );
        copy_partial( image, bitmap, x_offset, y_offset );
        write_metric( memory, base_index, glyphs, level_inverse, bitmap.metric );
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
            font::bitmap bitmap = face.bitmap( glyph_index_, level_size );
            copy_square( image, bitmap, i * level_size, j * level_size );
            write_metric( memory, glyph_index_, glyphs, level_inverse, bitmap.metric );
        }
    }

    return write_image( image, path );
}

OOE_NAMESPACE_END( ( ooe ) )
