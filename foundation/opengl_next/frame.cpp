/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/frame.hpp"
#include "foundation/opengl_next/symbol.hpp"
#include "foundation/opengl_next/target.hpp"
#include "foundation/opengl_next/texture.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

u32 frame_mask( u8 flags )
{
	u32 mask = 0;

	if ( flags & ooe::frame::color )
		mask |= COLOR_BUFFER_BIT;

	if ( flags & ooe::frame::depth )
		mask |= DEPTH_BUFFER_BIT;

	return mask;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- frame ----------------------------------------------------------------------------------------
frame::frame( void )
	: id(), textures(), targets(), width(), height()
{
	GenFramebuffers( 1, const_cast< u32* >( &id ) );
}

frame::~frame( void )
{
	DeleteFramebuffers( 1, &id );
}

void frame::write( const frame_type& source, u8 flags )
{
	opengl::frame& input = dynamic_cast< opengl::frame& >( *source );
	BindFramebuffer( READ_FRAMEBUFFER, input.id );
	BindFramebuffer( DRAW_FRAMEBUFFER, id );
	BlitFramebuffer
		( 0, 0, input.width, input.height, 0, 0, width, height, frame_mask( flags ), NEAREST );
}

void frame::insert( const target_type& target, attachment_type attachment )
{
	opengl::target& input = dynamic_cast< opengl::target& >( *target );
	width = std::min( width, input.width );
	height = std::min( height, input.height );
	targets.push_back( target_tuple( target, attachment ) );
}

void frame::insert( const texture_type& texture, attachment_type attachment )
{
	opengl::texture& input = dynamic_cast< opengl::texture& >( *texture );
	width = std::min( width, input.width );
	height = std::min( height, input.height );
	textures.push_back( texture_tuple( texture, attachment ) );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
