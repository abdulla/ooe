/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl/buffer.hpp"
#include "foundation/opengl/frame.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/opengl/target.hpp"
#include "foundation/opengl/texture.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/tuple.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( opengl ) )

typedef opengl::frame::state_type state_type;
typedef opengl::frame::attachment_map attachment_map;
typedef opengl::frame::location_map location_map;
typedef tuple< u32, u32, u8 > format_tuple;

s32 find( s32 id, location_map& locations, const std::string& name )
{
    location_map::const_iterator i = locations.find( name );

    if ( i != locations.end() )
        return i->second;

    s32 location = GetFragDataLocation( id, name.c_str() );

    if ( location == -1 )
        throw error::runtime( "opengl::frame: " ) << "Variable \"" << name << "\" does not exist";

    locations.insert( location_map::value_type( name, location ) );
    return location;
}

s32 find( const std::string& name )
{
    if ( name == "BACK_LEFT" )
        return BACK_LEFT;
    else if ( name == "BACK_RIGHT" )
        return BACK_LEFT + 1;
    else
        throw error::runtime( "opengl::default_frame: " ) <<
            "Variable \"" << name << "\" is invalid";
}

format_tuple frame_format( image_format::type format )
{
    switch ( format )
    {
    case image_format::bgr_u8:
        return format_tuple( BGR, UNSIGNED_BYTE, 3 );

    case image_format::bgra_u8:
        return format_tuple( BGRA, UNSIGNED_BYTE, 4 );

    //--- u8 -------------------------------------------------------------------
    case image_format::rgb_u8:
        return format_tuple( RGB, UNSIGNED_BYTE, 3 );

    case image_format::rgba_u8:
        return format_tuple( RGBA, UNSIGNED_BYTE, 4 );

    //--- f16 ------------------------------------------------------------------
    case image_format::rgb_f16:
        return format_tuple( RGB, HALF_FLOAT, 6 );

    case image_format::rgba_f16:
        return format_tuple( RGBA, HALF_FLOAT, 8 );

    //--- f32 ------------------------------------------------------------------
    case image_format::rgb_f32:
        return format_tuple( RGB, FLOAT, 12 );

    case image_format::rgba_f32:
        return format_tuple( RGBA, FLOAT, 16 );

    default:
        throw error::runtime( "opengl::frame: " ) << "Unknown frame format: " << format;
    }
}

void frame_read( buffer_ptr& generic_buffer, image_format::type format, u32 target, s32 id,
    u32 width, u32 height, state_type& state )
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
    frame_check( READ_FRAMEBUFFER, state );
    ReadBuffer( target );

    BindBuffer( PIXEL_PACK_BUFFER, buffer.id );
    ReadPixels( 0, 0, width, height, tuple._0, tuple._1, 0 );
}

void frame_write( const frame_ptr& generic_frame, s32 id, u32 width, u32 height, state_type& state )
{
    opengl::frame& frame = dynamic_cast< opengl::frame& >( *generic_frame );

    BindFramebuffer( READ_FRAMEBUFFER, frame.id );
    frame_check( READ_FRAMEBUFFER, frame.state );

    BindFramebuffer( DRAW_FRAMEBUFFER, id );
    frame_check( DRAW_FRAMEBUFFER, state );

    BlitFramebuffer( 0, 0, frame.width, frame.height, 0, 0, width, height,
        COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT, NEAREST );
}

void frame_clear( s32 id, state_type& state )
{
    BindFramebuffer( DRAW_FRAMEBUFFER, id );
    frame_check( DRAW_FRAMEBUFFER, state );
    Clear( COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT );
}

void frame_output( s32 id, s32 location, const opengl::frame::attachment_tuple& tuple,
    attachment_map& attachments, state_type& state )
{
    BindFramebuffer( DRAW_FRAMEBUFFER, id );
    std::pair< attachment_map::iterator, bool > pair =
        attachments.insert( std::make_pair( location, tuple ) );

    if ( pair.second )
        state = opengl::frame::none;
    else
        pair.first->second = tuple;
}

OOE_ANONYMOUS_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- default_frame --------------------------------------------------------------------------------
default_frame::default_frame( u32 width_, u32 height_ )
    : width( width_ ), height( height_ )
{
}

default_frame::~default_frame( void )
{
}

void default_frame::
    read( const std::string& name, image_format::type format, buffer_ptr& generic_buffer )
{
    state_type state = opengl::frame::checked;
    frame_read( generic_buffer, format, find( name ), 0, width, height, state );
}

void default_frame::write( const frame_ptr& generic_frame )
{
    state_type state = opengl::frame::checked;
    frame_write( generic_frame, 0, width, height, state );
}

void default_frame::clear( void )
{
    state_type state = opengl::frame::checked;
    frame_clear( 0, state );
}

void default_frame::output( const std::string&, const texture_ptr& )
{
    throw error::runtime( "opengl::default_frame: " ) << "Can not attach to default frame";
}

void default_frame::output( const std::string&, const target_ptr& )
{
    throw error::runtime( "opengl::default_frame: " ) << "Can not attach to default frame";
}

void default_frame::output( const target_ptr& )
{
    throw error::runtime( "opengl::default_frame: " ) << "Can not attach to default frame";
}

//--- frame ----------------------------------------------------------------------------------------
frame::frame( u32 program_, u32 width_, u32 height_ )
    : id(), program( program_ ), width( width_ ), height( height_ ), state( none ), depth(),
    attachments(), locations()
{
    GenFramebuffers( 1, const_cast< u32* >( &id ) );
}

frame::~frame( void )
{
    DeleteFramebuffers( 1, &id );
}

void frame::read( const std::string& name, image_format::type format, buffer_ptr& generic_buffer )
{
    if ( !attachments.size() )
        throw error::runtime( "opengl::frame: " ) << "Frame has no attachments";

    s32 location = find( program, locations, name );
    frame_read( generic_buffer, format, COLOR_ATTACHMENT0 + location, id, width, height, state );
}

void frame::write( const frame_ptr& generic_frame )
{
    frame_write( generic_frame, id, width, height, state );
}

void frame::clear( void )
{
    frame_clear( id, state );
}

void frame::output( const std::string& name, const texture_ptr& generic_texture )
{
    const opengl::texture& texture = dynamic_cast< const opengl::texture& >( *generic_texture );
    s32 location = find( program, locations, name );
    frame_output( id, location, attachment_tuple( generic_texture, 0 ), attachments, state );

    FramebufferTexture2D
        ( DRAW_FRAMEBUFFER, COLOR_ATTACHMENT0 + location, TEXTURE_2D, texture.id, 0 );
}

void frame::output( const std::string& name, const target_ptr& generic_target )
{
    const opengl::target& target = dynamic_cast< const opengl::target& >( *generic_target );
    s32 location = find( program, locations, name );
    frame_output( id, location, attachment_tuple( 0, generic_target ), attachments, state );

    FramebufferRenderbuffer
        ( DRAW_FRAMEBUFFER, COLOR_ATTACHMENT0 + location, RENDERBUFFER, target.id );
}

void frame::output( const target_ptr& generic_target )
{
    const opengl::target& target = dynamic_cast< const opengl::target& >( *generic_target );

    if ( !depth )
        state = none;

    depth = generic_target;
    FramebufferRenderbuffer( DRAW_FRAMEBUFFER, DEPTH_ATTACHMENT, RENDERBUFFER, target.id );
}

//--- frame_check ----------------------------------------------------------------------------------
void frame_check( u32 target, state_type& state )
{
    if ( state != opengl::frame::none )
        return;

    s32 status = CheckFramebufferStatus( target );

    if ( status != FRAMEBUFFER_COMPLETE )
        throw error::runtime( "opengl::frame: " ) << "Incomplete frame: 0x" << hex( status );

    state = opengl::frame::checked;
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
