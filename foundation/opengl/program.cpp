/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/opengl/block.hpp"
#include "foundation/opengl/frame.hpp"
#include "foundation/opengl/program.hpp"
#include "foundation/opengl/shader.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( opengl ) )

void check( s32 id )
{
    s32 status;
    GetProgramiv( id, LINK_STATUS, &status );

    s32 size;
    GetProgramiv( id, INFO_LOG_LENGTH, &size );

    // size < 2 to fix issue with nvidia drivers on linux
    if ( status && size < 2 )
        return;

    scoped_array< c8 > array( new c8[ size ] );
    GetProgramInfoLog( id, size, 0, array );

    if ( status )
        OOE_CONSOLE( "opengl::program: " << "Link warning:\n" << array );
    else
        throw error::runtime( "opengl::program: " ) << "Link error:\n" << array;
}

OOE_ANONYMOUS_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- program --------------------------------------------------------------------------------------
program::program( const shader_vector& vector )
try
    : id ( CreateProgram() )
{
    for ( shader_vector::const_iterator i = vector.begin(), end = vector.end(); i != end; ++i )
        AttachShader( id, dynamic_cast< opengl::shader& >( **i ).id );

    LinkProgram( id );
    check( id );
}
catch ( ... )
{
    this->~program();
}

program::~program( void )
{
    DeleteProgram( id );
}

block_type program::block( const buffer_type& buffer ) const
{
    return new opengl::block( id, buffer );
}

frame_type program::frame( u32 width, u32 height ) const
{
    return new opengl::frame( id, width, height );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
