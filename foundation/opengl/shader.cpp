/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/io/memory.hpp"
#include "foundation/opengl/shader.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

u32 get_stage( shader::type stage )
{
    switch ( stage )
    {
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
shader::shader( const memory_vector& vector, type stage )
try
    : id( CreateShader( get_stage( stage ) ) )
{
    std::vector< const c8* > data;
    std::vector< s32 > size;

    for ( memory_vector::const_iterator i = vector.begin(), end = vector.end(); i != end; ++i )
    {
        data.push_back( i->as< c8 >() );
        size.push_back( i->size() );
    }

    ShaderSource( id, vector.size(), &data[ 0 ], &size[ 0 ] );
    CompileShader( id );

    s32 status;
    GetShaderiv( id, COMPILE_STATUS, &status );

    if ( status )
        return;

    s32 length;
    GetShaderiv( id, INFO_LOG_LENGTH, &length );
    scoped_array< c8 > array( new c8[ length ] );
    GetShaderInfoLog( id, length, 0, array );
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
