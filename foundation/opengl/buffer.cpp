/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/opengl/buffer.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( opengl ) )

u32 buffer_target( ooe::buffer::type format )
{
    switch ( format )
    {
    case ooe::buffer::point:
        return opengl::ARRAY_BUFFER;

    case ooe::buffer::index:
        return opengl::ELEMENT_ARRAY_BUFFER;

    case ooe::buffer::pixel:
        return opengl::PIXEL_PACK_BUFFER;

    default:
        throw error::runtime( "opengl::buffer: " ) << "Unknown buffer format: " << format;
    }
}

u32 buffer_usage( ooe::buffer::usage_type usage )
{
    switch ( usage )
    {
    case ooe::buffer::static_read:
        return STATIC_READ;

    case ooe::buffer::static_write:
        return STATIC_DRAW;

    case ooe::buffer::stream_read:
        return STREAM_READ;

    case ooe::buffer::stream_write:
        return STREAM_DRAW;

    default:
        throw error::runtime( "opengl::buffer: " ) << "Unknown usage type: " << usage;
    }
}

u32 buffer_access( ooe::buffer::access_type access )
{
    switch ( access )
    {
    case ooe::buffer::read:
        return READ_ONLY;

    case ooe::buffer::write:
        return WRITE_ONLY;

    case ooe::buffer::read_write:
        return READ_WRITE;

    default:
        throw error::runtime( "opengl::buffer: " ) << "Unknown access type: " << access;
    }
}

OOE_ANONYMOUS_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- map ------------------------------------------------------------------------------------------
map::map( void* data_, up_t size_, u32 target_ )
    : ooe::map( data_, size_ ), target( target_ )
{
}

map::~map( void )
{
    if ( !UnmapBuffer( target ) )
        OOE_CONSOLE( "opengl::map: " << "Data corruption occurred" );
}

//--- buffer ---------------------------------------------------------------------------------------
buffer::buffer( up_t size_, type format, usage_type usage )
    : id(), target( buffer_target( format ) ), size( size_ )
{
    GenBuffers( 1, &id );

    BindBuffer( target, id );
    BufferData( target, size, 0, buffer_usage( usage ) );
}

buffer::~buffer( void )
{
    DeleteBuffers( 1, &id );
}

map_ptr buffer::map( access_type access ) const
{
    BindBuffer( target, id );
    void* data = MapBuffer( target, buffer_access( access ) );
    return map_ptr( new opengl::map( data, size, target ) );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
