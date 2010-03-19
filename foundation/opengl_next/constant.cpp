/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/constant.hpp"
#include "foundation/opengl_next/symbol.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

s32 lookup( u32 id, opengl::constant::map_type& map, const std::string& name )
{
	opengl::constant::map_type::const_iterator i = map.find( name );

	if ( i != map.end() )
		return i->second;

	s32 location = GetUniformLocation( id, name.c_str() );
	map.insert( opengl::constant::map_type::value_type( name, location ) );
	return location;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- constant -------------------------------------------------------------------------------------
constant::constant( u32 id_ )
	: id( id_ ), map()
{
}

void constant::insert( const std::string& name, f32 value )
{
	Uniform1fv( lookup( id, map, name ), 1, &value );
}

void constant::insert( const std::string& name, const slang_vec2& value )
{
	Uniform2fv( lookup( id, map, name ), 1, value.data );
}

void constant::insert( const std::string& name, const slang_vec3& value )
{
	Uniform3fv( lookup( id, map, name ), 1, value.data );
}

void constant::insert( const std::string& name, const slang_vec4& value )
{
	Uniform4fv( lookup( id, map, name ), 1, value.data );
}

void constant::insert( const std::string& name, s32 value )
{
	Uniform1iv( lookup( id, map, name ), 1, &value );
}

void constant::insert( const std::string& name, const slang_ivec2& value )
{
	Uniform2iv( lookup( id, map, name ), 1, value.data );
}

void constant::insert( const std::string& name, const slang_ivec3& value )
{
	Uniform3iv( lookup( id, map, name ), 1, value.data );
}

void constant::insert( const std::string& name, const slang_ivec4& value )
{
	Uniform4iv( lookup( id, map, name ), 1, value.data );
}

void constant::insert( const std::string& name, const slang_mat2& value )
{
	UniformMatrix2fv( lookup( id, map, name ), 1, false, value.data );
}

void constant::insert( const std::string& name, const slang_mat3& value )
{
	UniformMatrix3fv( lookup( id, map, name ), 1, false, value.data );
}

void constant::insert( const std::string& name, const slang_mat4& value )
{
	UniformMatrix4fv( lookup( id, map, name ), 1, false, value.data );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
