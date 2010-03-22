/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/block.hpp"
#include "foundation/opengl_next/symbol.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

s32 lookup( u32 id, opengl::block::uniform_map& uniforms, const std::string& name )
{
	opengl::block::uniform_map::const_iterator i = uniforms.find( name );

	if ( i != uniforms.end() )
		return i->second;

	s32 location = GetUniformLocation( id, name.c_str() );

	if ( location == -1 )
		throw error::runtime( "opengl::block: " ) << "Unknown uniform \"" << name << "\"";

	uniforms.insert( opengl::block::uniform_map::value_type( name, location ) );
	return location;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- block ----------------------------------------------------------------------------------------
block::block( u32 id_ )
	: id( id_ )
{
}

block::~block( void )
{
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
