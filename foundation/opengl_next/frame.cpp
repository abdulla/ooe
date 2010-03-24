/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/frame.hpp"
#include "foundation/opengl_next/symbol.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

void insert( opengl::frame::name_set& names, const std::string& name )
{
	if ( names.find( name ) != names.end() )
		throw error::runtime( "opengl::frame: " ) << "Variable \"" << name << "\" already set";

	names.insert( name );
}

s32 find( s32 id, opengl::frame::name_set& names, const std::string& name )
{
	s32 location = GetFragDataLocation( id, name.c_str() );

	if ( location == -1 )
		throw error::runtime( "opengl::frame: " ) << "Variable \"" << name << "\" does not exist";

	insert( names, name );
	return location;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- frame ----------------------------------------------------------------------------------------
frame::frame( u32 program_ )
	: id(), textures(), targets(), program( program_ ), names()
{
	GenFramebuffers( 1, const_cast< u32* >( &id ) );
}

frame::~frame( void )
{
	DeleteFramebuffers( 1, &id );
}

void frame::
	output( const std::string& name, attachment_type attachment, const texture_type& texture )
{
	s32 location = find( id, names, name );
	textures.push_back( texture_tuple( location, attachment, texture ) );
}

void frame::output( const std::string& name, attachment_type attachment, const target_type& target )
{
	s32 location = find( id, names, name );
	targets.push_back( target_tuple( location, attachment, target ) );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
