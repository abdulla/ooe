/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/block.hpp"
#include "foundation/opengl_next/buffer.hpp"
#include "foundation/opengl_next/symbol.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

typedef s32 ( function_type )( u32, const c8* );

void insert( opengl::block::name_set& names, const std::string& name )
{
	if ( names.find( name ) == names.end() )
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

s32 use( s32 id, opengl::block::name_set& names, const std::string& name )
{
	s32 location = find( id, names, name, GetUniformLocation );
	UseProgram( id );
	return location;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- block ----------------------------------------------------------------------------------------
block::block( u32 id_, const buffer_type& index_ )
	: id( id_ ), index( index_ ), in_textures(), in_buffers(), out_textures(), out_targets(),
	names()
{
	if ( dynamic_cast< opengl::buffer& >( *index ).target != ELEMENT_ARRAY_BUFFER )
		throw error::runtime( "opengl::block: " ) << "Index buffer expected";
}

block::~block( void )
{
}

void block::input( const std::string& name, s32 x )
{
	Uniform1i( use( id, names, name ), x );
}

void block::input( const std::string& name, s32 x, s32 y, s32 z )
{
	Uniform3i( use( id, names, name ), x, y, z );
}

void block::input( const std::string& name, f32 x )
{
	Uniform1f( use( id, names, name ), x );
}

void block::input( const std::string& name, f32 x, f32 y, f32 z )
{
	Uniform3f( use( id, names, name ), x, y, z );
}

void block::input( const std::string& name, const mat3& m )
{
	UniformMatrix3fv( use( id, names, name ), 1, false, m[ 0 ] );
}

void block::input( const std::string& name, const mat4& m )
{
	UniformMatrix4fv( use( id, names, name ), 1, false, m[ 0 ] );
}

void block::input( const std::string& name, const texture_type& texture )
{
	s32 location = find( id, names, name, GetUniformLocation );
	in_textures.push_back( in_texture_tuple( location, texture ) );
}

void block::input( const std::string& name, u8 size, const buffer_type& buffer )
{
	s32 location = find( id, names, name, GetAttribLocation );
	in_buffers.insert( in_buffer_map::value_type( buffer, in_buffer_tuple( location, size ) ) );
}

void block::
	output( const std::string& name, attachment_type attachment, const texture_type& texture )
{
	s32 location = find( id, names, name, GetFragDataLocation );
	out_textures.push_back( out_texture_tuple( location, attachment, texture ) );
}

void block::output( const std::string& name, attachment_type attachment, const target_type& target )
{
	s32 location = find( id, names, name, GetFragDataLocation );
	out_targets.push_back( out_target_tuple( location, attachment, target ) );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
