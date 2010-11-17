/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl/block.hpp"
#include "foundation/opengl/buffer.hpp"
#include "foundation/opengl/program.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( opengl ) )

typedef opengl::block::location_map location_map;
typedef s32 ( function_type )( u32, const c8* );
typedef tuple< u8, u8 > input_tuple;

s32 find( s32 id, location_map& locations, const std::string& name, function_type function )
{
    location_map::const_iterator i = locations.find( name );

    if ( i != locations.end() )
        return i->second;

    s32 location = function( id, name.c_str() );

    if ( location == -1 )
        throw error::runtime( "opengl::block: " ) << "Variable \"" << name << "\" does not exist";

    locations.insert( location_map::value_type( name, location ) );
    return location;
}

void uniform_1i( s32 location, u32 size, const void* data )
{
    Uniform1iv( location, size, reinterpret_cast< const s32* >( data ) );
}

void uniform_2i( s32 location, u32 size, const void* data )
{
    Uniform2iv( location, size, reinterpret_cast< const s32* >( data ) );
}

void uniform_3i( s32 location, u32 size, const void* data )
{
    Uniform3iv( location, size, reinterpret_cast< const s32* >( data ) );
}

void uniform_4i( s32 location, u32 size, const void* data )
{
    Uniform4iv( location, size, reinterpret_cast< const s32* >( data ) );
}

void uniform_1f( s32 location, u32 size, const void* data )
{
    Uniform1fv( location, size, reinterpret_cast< const f32* >( data ) );
}

void uniform_2f( s32 location, u32 size, const void* data )
{
    Uniform2fv( location, size, reinterpret_cast< const f32* >( data ) );
}

void uniform_3f( s32 location, u32 size, const void* data )
{
    Uniform3fv( location, size, reinterpret_cast< const f32* >( data ) );
}

void uniform_4f( s32 location, u32 size, const void* data )
{
    Uniform4fv( location, size, reinterpret_cast< const f32* >( data ) );
}

void uniform_3m( s32 location, u32 size, const void* data )
{
    UniformMatrix3fv( location, size, false, reinterpret_cast< const f32* >( data ) );
}

void uniform_4m( s32 location, u32 size, const void* data )
{
    UniformMatrix4fv( location, size, false, reinterpret_cast< const f32* >( data ) );
}

opengl::block::uniform_array make_array( const void* data, up_t size )
{
    opengl::block::uniform_array array( new u8[ size ] );
    std::memcpy( array, data, size );
    return array;
}

input_tuple buffer_input( const std::string& name, u8 size )
{
    switch ( size )
    {
    case 1:
    case 2:
    case 3:
    case 4:
        return input_tuple( 1, size );

    case 9:
        return input_tuple( 3, 3 );

    case 16:
        return input_tuple( 4, 4 );

    default:
        throw error::runtime( "opengl::block: " ) <<
            "Attribute \"" << name << "\" does not support size " << size;
    }
}

OOE_ANONYMOUS_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- block ----------------------------------------------------------------------------------------
block::block( u32 id_, const buffer_type& index_ )
    : id( id_ ), index( index_ ), uniforms(), textures(), texture_arrays(), buffers(), iterators(),
    locations()
{
    if ( dynamic_cast< opengl::buffer& >( *index ).target != ELEMENT_ARRAY_BUFFER )
        throw error::runtime( "opengl::block: " ) << "Index buffer expected";
}

block::~block( void )
{
}

void block::input( const std::string& name, s32 data[][ 1 ], u32 size )
{
    s32 location = find( id, locations, name, GetUniformLocation );
    uniform_array array = make_array( data, sizeof( *data ) * size );
    uniforms[ location ] = make_tuple( uniform_1i, size, array );
}

void block::input( const std::string& name, s32 data[][ 2 ], u32 size )
{
    s32 location = find( id, locations, name, GetUniformLocation );
    uniform_array array = make_array( data, sizeof( *data ) * size );
    uniforms[ location ] = make_tuple( uniform_2i, size, array );
}

void block::input( const std::string& name, s32 data[][ 3 ], u32 size )
{
    s32 location = find( id, locations, name, GetUniformLocation );
    uniform_array array = make_array( data, sizeof( *data ) * size );
    uniforms[ location ] = make_tuple( uniform_3i, size, array );
}

void block::input( const std::string& name, s32 data[][ 4 ], u32 size )
{
    s32 location = find( id, locations, name, GetUniformLocation );
    uniform_array array = make_array( data, sizeof( *data ) * size );
    uniforms[ location ] = make_tuple( uniform_4i, size, array );
}

void block::input( const std::string& name, f32 data[][ 1 ], u32 size )
{
    s32 location = find( id, locations, name, GetUniformLocation );
    uniform_array array = make_array( data, sizeof( *data ) );
    uniforms[ location ] = make_tuple( uniform_1f, size, array );
}

void block::input( const std::string& name, f32 data[][ 2 ], u32 size )
{
    s32 location = find( id, locations, name, GetUniformLocation );
    uniform_array array = make_array( data, sizeof( *data ) );
    uniforms[ location ] = make_tuple( uniform_2f, size, array );
}

void block::input( const std::string& name, f32 data[][ 3 ], u32 size )
{
    s32 location = find( id, locations, name, GetUniformLocation );
    uniform_array array = make_array( data, sizeof( *data ) );
    uniforms[ location ] = make_tuple( uniform_3f, size, array );
}

void block::input( const std::string& name, f32 data[][ 4 ], u32 size )
{
    s32 location = find( id, locations, name, GetUniformLocation );
    uniform_array array = make_array( data, sizeof( *data ) );
    uniforms[ location ] = make_tuple( uniform_4f, size, array );
}

void block::input( const std::string& name, const mat3* data, u32 size )
{
    s32 location = find( id, locations, name, GetUniformLocation );
    uniform_array array = make_array( data, sizeof( *data ) * size );
    uniforms[ location ] = make_tuple( uniform_3m, size, array );
}

void block::input( const std::string& name, const mat4* data, u32 size )
{
    s32 location = find( id, locations, name, GetUniformLocation );
    uniform_array array = make_array( data, sizeof( *data ) * size );
    uniforms[ location ] = make_tuple( uniform_4m, size, array );
}

void block::input( const std::string& name, const texture_type& texture )
{
    s32 location = find( id, locations, name, GetUniformLocation );
    textures[ location ] = texture;
}

void block::input( const std::string& name, const texture_array_type& texture_array )
{
    s32 location = find( id, locations, name, GetUniformLocation );
    texture_arrays[ location ] = texture_array;
}

void block::input( const std::string& name, u8 size, const buffer_type& buffer, bool instanced )
{
    if ( dynamic_cast< opengl::buffer& >( *buffer ).target != ARRAY_BUFFER )
        throw error::runtime( "opengl::block: " ) << "Point buffer expected";

    s32 location = find( id, locations, name, GetAttribLocation );
    input_tuple in = buffer_input( name, size );

    for ( u8 i = 0; i != in._0; ++i )
    {
        buffer_map::iterator j =
            buffers.insert( buffer_map::value_type( buffer, buffer_tuple( location + i, in._1 ) ) );
        iterator_map::iterator k = iterators.find( location + i );

        if ( k == iterators.end() )
            iterators[ location + i ] = j;
        else
        {
            buffers.erase( k->second );
            k->second = j;
        }
    }

    VertexAttribDivisor( location, instanced );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
