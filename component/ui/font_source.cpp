/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cctype>
#include <cmath>

#include "component/ui/font_source.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/io/file.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/binary.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/miscellany.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const image::type image_type = image::rgba_u8;
const u16 page_wide = 256;

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

std::string get_root( const std::string& root, font::face& face )
{
    std::string suffix = face.string( font::face::family ) + '-' + face.string( font::face::style );
    std::transform( suffix.begin(), suffix.end(), suffix.begin(), transform );
    std::string path = canonical_path( root ) + '/' + suffix;

    if ( !exists( path ) )
        make_directory( path );

    return path;
}

u32 get_size( const font::face& face, u32 face_size )
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

void copy_partial( uncompressed_image& image, const font::bitmap& bitmap, u32 x, u32 y )
{
    if ( x >= bitmap.metric.width || y >= bitmap.metric.height )
        return;

    u8* target = image.as< u8 >() + 3 /* alpha */;
    const u8* source = bitmap.data + x + y * bitmap.metric.width;

    u32 width = std::min< u32 >( page_wide, bitmap.metric.width - x );
    u32 height = std::min< u32 >( page_wide, bitmap.metric.height - y );

    for ( u32 i = 0; i != height;
        ++i, target += image.row_size(), source += bitmap.metric.width )
        skip_out( source, source + width, target, image.channels() );
}

void copy_square( uncompressed_image& image, const font::bitmap& bitmap, u32 x, u32 y )
{
    u8* target = image.as< u8 >() + ( x + y * image.width ) * image.pixel_size() + 3 /* alpha */;
    const u8* source = bitmap.data;

    for ( u32 i = 0; i != bitmap.metric.height;
        ++i, target += image.row_size(), source += bitmap.metric.width )
        skip_out( source, source + bitmap.metric.width, target, image.channels() );
}

const uncompressed_image& write_image( const uncompressed_image& image, const std::string& path )
{
    file file( descriptor( path, descriptor::write_new ) );
    file.write( image.get(), image.byte_size() );
    return image;
}

void write_metric( const memory& memory, up_t char_code, u32 glyphs, u8 level_inverse,
    const font::metric& metric )
{
    source_metric* metrics = memory.as< source_metric >() + char_code + glyphs * level_inverse;
    *metrics = source_metric( metric, true );
}

source_metric& read_metric( const memory& memory, up_t char_code, u32 glyphs, u8 level_inverse )
{
    return memory.as< source_metric >()[ char_code + glyphs * level_inverse ];
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- font_source ----------------------------------------------------------------------------------
font_source::font_source( font::face& face_, u32 face_size_, const std::string& root_ )
    : face( face_ ), face_size( face_size_ ), root( get_root( root_, face ) ),
    source_size( get_size( face, face_size ) ), glyphs( face.number( font::face::glyphs ) ),
    first( face.number( font::face::first ) ), level_limit( log2f( source_size / page_wide ) ),
    mutex(), memory( open_memory( root, glyphs, level_limit + 1 ) )
{
    if ( !is_bit_round( face_size ) )
        throw error::runtime( "font_source: " ) <<
            "Font size " << face_size << " is not a power of 2";
}

font_source::~font_source( void )
{
}

u32 font_source::size( void ) const
{
    return source_size;
}

image::type font_source::format( void ) const
{
    return image_type;
}

u16 font_source::page_size( void ) const
{
    return page_wide;
}

u32 font_source::font_size( void ) const
{
    return face_size;
}

font_source::glyph_type font_source::glyph( up_t char_code, u8 level ) const
{
    if ( level > level_limit )
        throw error::runtime( "font_source: " ) <<
            "Mipmap level " << level << " > maximum level " << level_limit;

    u32 code = first >= char_code ? 0 : char_code - first;
    u8 level_inverse = level_limit - level;
    source_metric& metric = read_metric( memory, code, glyphs, level_inverse );

    if ( !metric.valid )
    {
        lock lock( mutex );
        font::bitmap bitmap = face.character( char_code, face_size >> level );
        metric = source_metric( bitmap.metric, true );
    }

    u32 glyphs_per_row = source_size / face_size;
    return glyph_type
    (
        ( code % glyphs_per_row ) * face_size,
        ( code / glyphs_per_row ) * face_size,
        metric
    );
}

image font_source::read( const pyramid_index& index )
{
    // note: arguments will be verified by virtual_texture, no need to here
    uncompressed_image image( page_wide, page_wide, image_type );
    std::string path( root );
    u8 level_inverse = level_limit - index.level;
    path << '/' << index.x << '_' << index.y << '_' << level_inverse << ".raw";

    if ( exists( path ) )
    {
        file file( path );
        file.read( image.get(), image.byte_size() );
        return image;
    }

    std::memset( image.get(), 0, image.byte_size() );
    u32 level_size = face_size >> index.level;
    u32 glyphs_per_row = source_size / face_size;
    up_t char_code =
        index.x * page_wide / level_size + ( index.y * page_wide / level_size ) * glyphs_per_row;

    if ( char_code >= glyphs )
        return write_image( image, path );

    if ( level_size >= page_wide )
    {
        u32 pages_per_glyph = level_size / page_wide;
        u32 x_offset = ( index.x % pages_per_glyph ) * page_wide;
        u32 y_offset = ( index.y % pages_per_glyph ) * page_wide;

        lock lock( mutex );
        font::bitmap bitmap = face.character( char_code + first, level_size );
        copy_partial( image, bitmap, x_offset, y_offset );
        write_metric( memory, char_code, glyphs, level_inverse, bitmap.metric );
        return write_image( image, path );
    }

    for ( u32 j = 0, glyphs_per_page = page_wide / level_size; j != glyphs_per_page; ++j )
    {
        for ( u32 i = 0; i != glyphs_per_page; ++i )
        {
            u32 code = char_code + i + j * glyphs_per_row;

            if ( code >= glyphs )
                return write_image( image, path );

            lock lock( mutex );
            font::bitmap bitmap = face.character( code + first, level_size );
            copy_square( image, bitmap, i * level_size, j * level_size );
            write_metric( memory, code, glyphs, level_inverse, bitmap.metric );
        }
    }

    return write_image( image, path );
}

OOE_NAMESPACE_END( ( ooe ) )
