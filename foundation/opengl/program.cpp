/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/opengl/block.hpp"
#include "foundation/opengl/frame.hpp"
#include "foundation/opengl/program.hpp"
#include "foundation/opengl/shader.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

void check_status( s32 id, s32 parameter, const c8* type )
{
    s32 status;
    GetProgramiv( id, parameter, &status );

    s32 size;
    GetProgramiv( id, INFO_LOG_LENGTH, &size );

    if ( status && !size )
        return;

    scoped_array< c8 > array( new c8[ size ] );
    GetProgramInfoLog( id, size, 0, array );

    if ( status )
        OOE_CONSOLE( "opengl::program: " << type << " warning:\n" << array );
    else
        throw error::runtime( "opengl::program: " ) << type << " error:\n" << array;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- program --------------------------------------------------------------------------------------
program::program( const shader_vector& vector, u32 vertices )
try
    : id ( CreateProgram() )
{
    for ( shader_vector::const_iterator i = vector.begin(), end = vector.end(); i != end; ++i )
        AttachShader( id, dynamic_cast< opengl::shader& >( **i ).id );

    if ( vertices )
    {
        ProgramParameteri( id, GEOMETRY_INPUT_TYPE, TRIANGLES );
        ProgramParameteri( id, GEOMETRY_OUTPUT_TYPE, TRIANGLES );
        ProgramParameteri( id, GEOMETRY_VERTICES_OUT, vertices );
    }

    LinkProgram( id );
    check_status( id, LINK_STATUS, "Link" );
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

void check_program( u32 id )
{
    ValidateProgram( id );
    check_status( id, VALIDATE_STATUS, "Validate" );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
