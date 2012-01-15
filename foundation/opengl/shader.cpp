/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/io/memory.hpp"
#include "foundation/opengl/shader.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

u32 get_stage( shader::type stage )
{
    switch ( stage )
    {
    case shader::geometry:
        return GEOMETRY_SHADER;

    case shader::vertex:
        return VERTEX_SHADER;

    case shader::fragment:
        return FRAGMENT_SHADER;

    default:
        throw error::runtime( "opengl::shader: " ) << "Unknown shader type: " << stage;
    }
}

const c8* stage_name( shader::type stage )
{
    switch ( stage )
    {
    case shader::geometry:
        return "Geometry";

    case shader::vertex:
        return "Vertex";

    case shader::fragment:
        return "Fragment";

    default:
        throw error::runtime( "opengl::shader: " ) << "Unknown shader type: " << stage;
    }
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- shader ---------------------------------------------------------------------------------------
shader::shader( const std::string& source, type stage )
try
    : id( CreateShader( get_stage( stage ) ) )
{
    const c8* data = source.data();
    s32 size = source.size();
    ShaderSource( id, 1, &data, &size );
    CompileShader( id );

    s32 status;
    GetShaderiv( id, COMPILE_STATUS, &status );

    if ( status )
        return;

    GetShaderiv( id, INFO_LOG_LENGTH, &size );
    scoped_array< c8 > array( new c8[ size ] );
    GetShaderInfoLog( id, size, 0, array );
    throw error::runtime( "opengl::shader: " ) << stage_name( stage ) << " shader:\n" << array;
}
catch ( ... )
{
    this->~shader();
}

shader::~shader( void )
{
    DeleteShader( id );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
