/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl/block.hpp"
#include "foundation/opengl/buffer.hpp"
#include "foundation/opengl/program.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

typedef s32 ( function_type )( u32, const c8* );

s32 find( s32 id, opengl::block::location_map& locations, const std::string& name,
	function_type function )
{
	opengl::block::location_map::const_iterator i = locations.find( name );

	if ( i != locations.end() )
		return i->second;

	s32 location = function( id, name.c_str() );

	if ( location == -1 )
		throw error::runtime( "opengl::block: " ) << "Variable \"" << name << "\" does not exist";

	locations.insert( opengl::block::location_map::value_type( name, location ) );
	return location;
}

void uniform_1i( s32 location, const opengl::block::uniform_data data )
{
	Uniform1iv( location, 1, reinterpret_cast< const s32* >( data ) );
}

void uniform_2i( s32 location, const opengl::block::uniform_data data )
{
	Uniform2iv( location, 1, reinterpret_cast< const s32* >( data ) );
}

void uniform_3i( s32 location, const opengl::block::uniform_data data )
{
	Uniform3iv( location, 1, reinterpret_cast< const s32* >( data ) );
}

void uniform_1f( s32 location, const opengl::block::uniform_data data )
{
	Uniform1fv( location, 1, reinterpret_cast< const f32* >( data ) );
}

void uniform_2f( s32 location, const opengl::block::uniform_data data )
{
	Uniform2fv( location, 1, reinterpret_cast< const f32* >( data ) );
}

void uniform_3f( s32 location, const opengl::block::uniform_data data )
{
	Uniform3fv( location, 1, reinterpret_cast< const f32* >( data ) );
}

void uniform_3m( s32 location, const opengl::block::uniform_data data )
{
	UniformMatrix3fv( location, 1, false, reinterpret_cast< const f32* >( data ) );
}

void uniform_4m( s32 location, const opengl::block::uniform_data data )
{
	UniformMatrix4fv( location, 1, false, reinterpret_cast< const f32* >( data ) );
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- block ----------------------------------------------------------------------------------------
block::block( u32 id_, const buffer_type& index_ )
	: id( id_ ), do_check( true ), index( index_ ), uniforms(), textures(), texture_arrays(),
	buffers(), locations()
{
	if ( dynamic_cast< opengl::buffer& >( *index ).target != ELEMENT_ARRAY_BUFFER )
		throw error::runtime( "opengl::block: " ) << "Index buffer expected";
}

block::~block( void )
{
}

void block::input( const std::string& name, s32 x )
{
	s32 location = find( id, locations, name, GetUniformLocation );

	uniform_tuple& tuple = uniforms[ location ];
	std::memcpy( tuple._0, &x, sizeof( x ) );
	tuple._1 = uniform_1i;
}

void block::input( const std::string& name, s32 x, s32 y )
{
	s32 location = find( id, locations, name, GetUniformLocation );
	s32 array[] = { x, y };

	uniform_tuple& tuple = uniforms[ location ];
	std::memcpy( tuple._0, array, sizeof( array ) );
	tuple._1 = uniform_2i;
}

void block::input( const std::string& name, s32 x, s32 y, s32 z )
{
	s32 location = find( id, locations, name, GetUniformLocation );
	s32 array[] = { x, y, z };

	uniform_tuple& tuple = uniforms[ location ];
	std::memcpy( tuple._0, array, sizeof( array ) );
	tuple._1 = uniform_3i;
}

void block::input( const std::string& name, f32 x )
{
	s32 location = find( id, locations, name, GetUniformLocation );

	uniform_tuple& tuple = uniforms[ location ];
	std::memcpy( tuple._0, &x, sizeof( x ) );
	tuple._1 = uniform_1f;
}

void block::input( const std::string& name, f32 x, f32 y )
{
	s32 location = find( id, locations, name, GetUniformLocation );
	f32 array[] = { x, y };

	uniform_tuple& tuple = uniforms[ location ];
	std::memcpy( tuple._0, array, sizeof( array ) );
	tuple._1 = uniform_2f;
}

void block::input( const std::string& name, f32 x, f32 y, f32 z )
{
	s32 location = find( id, locations, name, GetUniformLocation );
	f32 array[] = { x, y, z };

	uniform_tuple& tuple = uniforms[ location ];
	std::memcpy( tuple._0, array, sizeof( array ) );
	tuple._1 = uniform_3f;
}

void block::input( const std::string& name, const mat3& m )
{
	s32 location = find( id, locations, name, GetUniformLocation );

	uniform_tuple& tuple = uniforms[ location ];
	std::memcpy( tuple._0, &m, sizeof( m ) );
	tuple._1 = uniform_3m;
}

void block::input( const std::string& name, const mat4& m )
{
	s32 location = find( id, locations, name, GetUniformLocation );

	uniform_tuple& tuple = uniforms[ location ];
	std::memcpy( tuple._0, &m, sizeof( m ) );
	tuple._1 = uniform_4m;
}

void block::input( const std::string& name, const texture_type& texture )
{
	s32 location = find( id, locations, name, GetUniformLocation );
	textures[ location ] = texture;
	do_check = true;
}

void block::input( const std::string& name, const texture_array_type& texture_array )
{
	s32 location = find( id, locations, name, GetUniformLocation );
	texture_arrays[ location ] = texture_array;
	do_check = true;
}

void block::input( const std::string& name, u8 size, const buffer_type& buffer )
{
	if ( dynamic_cast< opengl::buffer& >( *buffer ).target != ARRAY_BUFFER )
		throw error::runtime( "opengl::block: " ) << "Point buffer expected";

	s32 location = find( id, locations, name, GetAttribLocation );
	buffers.insert( buffer_map::value_type( buffer, buffer_tuple( location, size ) ) );
	do_check = true;
}

void block::check( void )
{
	if ( !do_check )
		return;

	check_program( id );
	do_check = false;
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
