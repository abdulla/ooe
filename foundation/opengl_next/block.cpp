/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/block.hpp"
#include "foundation/opengl_next/buffer.hpp"
#include "foundation/opengl_next/symbol.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

typedef s32 ( function_type )( u32, const c8* );

void insert( opengl::block::name_set& names, const std::string& name )
{
	if ( names.find( name ) != names.end() )
		throw error::runtime( "opengl::block: " ) << "Variable \"" << name << "\" already set";

	names.insert( name );
}

s32 find( s32 id, opengl::block::name_set& names, const std::string& name, function_type function )
{
	s32 location = function( id, name.c_str() );

	if ( location == -1 )
		throw error::runtime( "opengl::block: " ) << "Variable \"" << name << "\" does not exist";

	insert( names, name );
	return location;
}

void uniform_1i( s32 location, const opengl::block::uniform_data data )
{
	Uniform1iv( location, 1, reinterpret_cast< const s32* >( data ) );
}

void uniform_3i( s32 location, const opengl::block::uniform_data data )
{
	Uniform3iv( location, 1, reinterpret_cast< const s32* >( data ) );
}

void uniform_1f( s32 location, const opengl::block::uniform_data data )
{
	Uniform1fv( location, 1, reinterpret_cast< const f32* >( data ) );
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
	: id( id_ ), index( index_ ), uniforms(), textures(), buffers(), names()
{
	if ( dynamic_cast< opengl::buffer& >( *index ).target != ELEMENT_ARRAY_BUFFER )
		throw error::runtime( "opengl::block: " ) << "Index buffer expected";
}

block::~block( void )
{
}

void block::input( const std::string& name, s32 x )
{
	uniform_tuple tuple;
	tuple._0 = find( id, names, name, GetUniformLocation );
	tuple._2 = uniform_1i;
	std::memcpy( tuple._1, &x, sizeof( x ) );

	uniforms.push_back( tuple );
}

void block::input( const std::string& name, s32 x, s32 y, s32 z )
{
	s32 array[] = { x, y, z };

	uniform_tuple tuple;
	tuple._0 = find( id, names, name, GetUniformLocation );
	tuple._2 = uniform_3i;
	std::memcpy( tuple._1, array, sizeof( array ) );

	uniforms.push_back( tuple );
}

void block::input( const std::string& name, f32 x )
{
	uniform_tuple tuple;
	tuple._0 = find( id, names, name, GetUniformLocation );
	tuple._2 = uniform_1f;
	std::memcpy( tuple._1, &x, sizeof( x ) );

	uniforms.push_back( tuple );
}

void block::input( const std::string& name, f32 x, f32 y, f32 z )
{
	f32 array[] = { x, y, z };

	uniform_tuple tuple;
	tuple._0 = find( id, names, name, GetUniformLocation );
	tuple._2 = uniform_3f;
	std::memcpy( tuple._1, array, sizeof( array ) );

	uniforms.push_back( tuple );
}

void block::input( const std::string& name, const mat3& m )
{
	uniform_tuple tuple;
	tuple._0 = find( id, names, name, GetUniformLocation );
	tuple._2 = uniform_3m;
	std::memcpy( tuple._1, &m, sizeof( m ) );

	uniforms.push_back( tuple );
}

void block::input( const std::string& name, const mat4& m )
{
	uniform_tuple tuple;
	tuple._0 = find( id, names, name, GetUniformLocation );
	tuple._2 = uniform_4m;
	std::memcpy( tuple._1, &m, sizeof( m ) );

	uniforms.push_back( tuple );
}

void block::input( const std::string& name, const texture_type& texture )
{
	s32 location = find( id, names, name, GetUniformLocation );
	textures.push_back( texture_tuple( location, texture ) );
}

void block::input( const std::string& name, u8 size, const buffer_type& buffer )
{
	s32 location = find( id, names, name, GetAttribLocation );
	buffers.insert( buffer_map::value_type( buffer, buffer_tuple( location, size ) ) );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
