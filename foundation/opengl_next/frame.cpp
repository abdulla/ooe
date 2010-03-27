/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/frame.hpp"
#include "foundation/opengl_next/symbol.hpp"
#include "foundation/opengl_next/target.hpp"
#include "foundation/opengl_next/texture.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

// use with EXT_gpu_shader4 or GL_ARB_explicit_attrib_location
/*s32 find( s32 id, opengl::frame::location_map& locations, const std::string& name )
{
	opengl::frame::location_map::const_iterator i = locations.find( name );

	if ( i != locations.end() )
		return i->second;

	s32 location = GetFragDataLocation( id, name.c_str() );

	if ( location == -1 )
		throw error::runtime( "opengl::frame: " ) << "Variable \"" << name << "\" does not exist";

	locations.insert( opengl::frame::location_map::value_type( name, location ) );
	return location;
}*/

s32 frame_attachment( ooe::frame::attachment_type attachment, u32& colour_index )
{
	switch ( attachment )
	{
	case ooe::frame::colour:
		return COLOR_ATTACHMENT0 + colour_index++;

	case ooe::frame::depth:
		return DEPTH_ATTACHMENT;

	default:
		throw error::runtime( "opengl::frame: " ) << "Unknown attachment type: " << attachment;
	}
}

template< typename target, typename source >
	const target& verify( const source& in, u32 width, u32 height )
{
	const target& out = dynamic_cast< const target& >( in );

	if ( out.width != width || out.height != height )
		throw error::runtime( "opengl::frame: " ) << "Attachment size " << out.width << 'x' <<
			out.height << " != frame size " << width << 'x' << height;

	return out;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- default_frame --------------------------------------------------------------------------------
default_frame::default_frame( u32 width_, u32 height_ )
	: width( width_ ), height( height_ )
{
}

default_frame::~default_frame( void )
{
}

void default_frame::write( const frame_type& generic_frame )
{
	opengl::frame& input = dynamic_cast< opengl::frame& >( *generic_frame );
	BindFramebuffer( READ_FRAMEBUFFER, input.id );
	BindFramebuffer( DRAW_FRAMEBUFFER, 0 );
	BlitFramebuffer( 0, 0, input.width, input.height, 0, 0, width, height,
		COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT, NEAREST );
}

void default_frame::output( attachment_type, const texture_type& )
{
	throw error::runtime( "opengl::default_frame: " ) << "Can not attach to default frame";
}

void default_frame::output( attachment_type, const target_type& )
{
	throw error::runtime( "opengl::default_frame: " ) << "Can not attach to default frame";
}

//--- frame ----------------------------------------------------------------------------------------
frame::frame( u32 program_, u32 width_, u32 height_ )
	: id(), width( width_ ), height( height_ ), textures(), targets(), program( program_ ),
	colour_index( 0 )
{
	GenFramebuffers( 1, const_cast< u32* >( &id ) );
}

frame::~frame( void )
{
	DeleteFramebuffers( 1, &id );
}

void frame::write( const frame_type& generic_frame )
{
	opengl::frame& input = dynamic_cast< opengl::frame& >( *generic_frame );
	BindFramebuffer( READ_FRAMEBUFFER, input.id );
	BindFramebuffer( DRAW_FRAMEBUFFER, id );
	BlitFramebuffer( 0, 0, input.width, input.height, 0, 0, width, height,
		COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT, NEAREST );
}

void frame::output( attachment_type type, const texture_type& generic_texture )
{
	const opengl::texture& texture = verify< opengl::texture >( *generic_texture, width, height );
	s32 attachment = frame_attachment( type, colour_index );
	textures[ attachment ] = generic_texture;

	BindFramebuffer( DRAW_FRAMEBUFFER, id );
	FramebufferTexture2D( DRAW_FRAMEBUFFER, attachment, TEXTURE_2D, texture.id, 0 );
}

void frame::output( attachment_type type, const target_type& generic_target )
{
	const opengl::target& target = verify< opengl::target >( *generic_target, width, height );
	s32 attachment = frame_attachment( type, colour_index );
	targets[ attachment ] = generic_target;

	BindFramebuffer( DRAW_FRAMEBUFFER, id );
	FramebufferRenderbuffer( DRAW_FRAMEBUFFER, attachment, RENDERBUFFER, target.id );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
