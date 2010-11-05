/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl/buffer.hpp"
#include "foundation/opengl/frame.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/opengl/target.hpp"
#include "foundation/opengl/texture.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/tuple.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

typedef tuple< u32, u32, u8 > format_tuple;

s32 find( s32 id, opengl::frame::location_map& locations, const std::string& name )
{
    opengl::frame::location_map::const_iterator i = locations.find( name );

    if ( i != locations.end() )
        return i->second;

    s32 location = GetFragDataLocation( id, name.c_str() );

    if ( location == -1 )
        throw error::runtime( "opengl::frame: " ) << "Variable \"" << name << "\" does not exist";

    locations.insert( opengl::frame::location_map::value_type( name, location ) );
    return location;
}

s32 find( const std::string& name )
{
    if ( name == "BACK_LEFT" )
        return BACK_LEFT;
    else if ( name == "BACK_RIGHT" )
        return BACK_LEFT + 1;
    else
        throw error::runtime( "opengl::default_frame: " ) << "Variable \"" << name << "\" invalid";
}

format_tuple frame_format( image::type format )
{
    switch ( format )
    {
    case image::bgr_u8:
        return format_tuple( BGR, UNSIGNED_BYTE, 3 );

    case image::bgra_u8:
        return format_tuple( BGRA, UNSIGNED_BYTE, 4 );

    //--- u8 -------------------------------------------------------------------
    case image::rgb_u8:
        return format_tuple( RGB, UNSIGNED_BYTE, 3 );

    case image::rgba_u8:
        return format_tuple( RGBA, UNSIGNED_BYTE, 4 );

    //--- f16 ------------------------------------------------------------------
    case image::rgb_f16:
        return format_tuple( RGB, HALF_FLOAT, 6 );

    case image::rgba_f16:
        return format_tuple( RGBA, HALF_FLOAT, 8 );

    //--- f32 ------------------------------------------------------------------
    case image::rgb_f32:
        return format_tuple( RGB, FLOAT, 12 );

    case image::rgba_f32:
        return format_tuple( RGBA, FLOAT, 16 );

    default:
        throw error::runtime( "opengl::frame: " ) << "Unknown frame format: " << format;
    }
}

void frame_check( bool& do_check, u32 target )
{
    if ( !do_check )
        return;

    s32 status = CheckFramebufferStatus( target );

    if ( status != FRAMEBUFFER_COMPLETE )
        throw error::runtime( "opengl::frame: " ) << "Frame is incomplete: 0x" << hex( status );

    do_check = false;
}

void frame_read( buffer_type& generic_buffer, image::type format,
    s32 id, u32 width, u32 height, bool& do_check, u32 target )
{
    opengl::buffer& buffer = dynamic_cast< opengl::buffer& >( *generic_buffer );

    if ( buffer.target != PIXEL_PACK_BUFFER )
        throw error::runtime( "opengl::frame: " ) << "Pixel buffer expected";

    format_tuple tuple = frame_format( format );
    up_t size = tuple._2 * width * height;

    if ( size > buffer.size )
        throw error::runtime( "opengl::frame: " ) <<
            "Pixel buffer size " << buffer.size << " < " << size;

    BindFramebuffer( READ_FRAMEBUFFER, id );
    frame_check( do_check, READ_FRAMEBUFFER );
    ReadBuffer( target );

    BindBuffer( PIXEL_PACK_BUFFER, buffer.id );
    ReadPixels( 0, 0, width, height, tuple._0, tuple._1, 0 );
}

void frame_write( const frame_type& generic_frame, const std::string& read_name,
    s32 id, u32 width, u32 height, bool& do_check, u32 draw_target )
{
    opengl::frame& frame = dynamic_cast< opengl::frame& >( *generic_frame );
    s32 read_target = find( frame.program, frame.locations, read_name );

    BindFramebuffer( READ_FRAMEBUFFER, frame.id );
    frame_check( frame.do_check, READ_FRAMEBUFFER );
    ReadBuffer( read_target );

    BindFramebuffer( DRAW_FRAMEBUFFER, id );
    frame_check( do_check, DRAW_FRAMEBUFFER );
    DrawBuffers( 1, &draw_target );

    BlitFramebuffer( 0, 0, frame.width, frame.height, 0, 0, width, height,
        COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT, NEAREST );
}

void frame_clear( s32 id, bool& do_check )
{
    BindFramebuffer( DRAW_FRAMEBUFFER, id );
    frame_check( do_check, DRAW_FRAMEBUFFER );
    Clear( COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT );
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- default_frame --------------------------------------------------------------------------------
default_frame::default_frame( u32 width_, u32 height_ )
    : width( width_ ), height( height_ )
{
}

default_frame::~default_frame( void )
{
}

void default_frame::read( const std::string& name, image::type format, buffer_type& generic_buffer )
{
    bool do_check = false;
    frame_read( generic_buffer, format, 0, width, height, do_check, find( name ) );
}

void default_frame::write
    ( const std::string& write_name, const std::string& read_name, const frame_type& generic_frame )
{
    bool do_check = false;
    frame_write( generic_frame, read_name, 0, width, height, do_check, find( write_name ) );
}

void default_frame::clear( void )
{
    bool do_check = false;
    frame_clear( 0, do_check );
}

void default_frame::output( const std::string&, const texture_type& )
{
    throw error::runtime( "opengl::default_frame: " ) << "Can not attach to default frame";
}

void default_frame::output( const std::string&, const target_type& )
{
    throw error::runtime( "opengl::default_frame: " ) << "Can not attach to default frame";
}

//--- frame ----------------------------------------------------------------------------------------
frame::frame( u32 program_, u32 width_, u32 height_ )
    : id(), program( program_ ), width( width_ ), height( height_ ), do_check( true ), colours(),
    locations()
{
    GenFramebuffers( 1, const_cast< u32* >( &id ) );
}

frame::~frame( void )
{
    DeleteFramebuffers( 1, &id );
}

void frame::read( const std::string& name, image::type format, buffer_type& generic_buffer )
{
    if ( !colours.size() )
        throw error::runtime( "opengl::frame: " ) << "Frame has no colour attachment";

    s32 location = find( program, locations, name );
    frame_read( generic_buffer, format, id, width, height, do_check, location );
}

void frame::write
    ( const std::string& write_name, const std::string& read_name, const frame_type& generic_frame )
{
    s32 location = find( program, locations, write_name );
    frame_write( generic_frame, read_name, id, width, height, do_check, location );
}

void frame::clear( void )
{
    frame_clear( id, do_check );
}

void frame::output( const std::string& name, const texture_type& generic_texture )
{
    const opengl::texture& texture = dynamic_cast< const opengl::texture& >( *generic_texture );
    s32 location = find( program, locations, name );

    BindFramebuffer( DRAW_FRAMEBUFFER, id );
    FramebufferTexture2D( DRAW_FRAMEBUFFER, location, TEXTURE_2D, texture.id, 0 );

    colours.push_back( location );
    do_check = true;
}

void frame::output( const std::string& name, const target_type& generic_target )
{
    const opengl::target& target = dynamic_cast< const opengl::target& >( *generic_target );
    s32 location = find( program, locations, name );

    BindFramebuffer( DRAW_FRAMEBUFFER, id );
    FramebufferRenderbuffer( DRAW_FRAMEBUFFER, location, RENDERBUFFER, target.id );

    colours.push_back( location );
    do_check = true;
}

void frame::check( void )
{
    BindFramebuffer( DRAW_FRAMEBUFFER, id );
    frame_check( do_check, DRAW_FRAMEBUFFER );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
