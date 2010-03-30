/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/buffer.hpp"
#include "foundation/opengl_next/frame.hpp"
#include "foundation/opengl_next/symbol.hpp"
#include "foundation/opengl_next/target.hpp"
#include "foundation/opengl_next/texture.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

typedef tuple< u32, u32 > format_tuple;

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

u32 frame_attachment
	( ooe::frame::attachment_type attachment, opengl::frame::colour_vector& colours )
{
	switch ( attachment )
	{
	case ooe::frame::colour:
		colours.push_back( COLOR_ATTACHMENT0 + colours.size() );
		return colours.back();

	case ooe::frame::depth:
		return DEPTH_ATTACHMENT;

	default:
		throw error::runtime( "opengl::frame: " ) << "Unknown attachment type: " << attachment;
	}
}

format_tuple frame_format( image::type format )
{
	switch ( format )
	{
	case image::bgr_u8:
		return format_tuple( BGR, UNSIGNED_BYTE );

	case image::bgra_u8:
		return format_tuple( BGRA, UNSIGNED_BYTE );

	//--- u8 -------------------------------------------------------------------
	case image::rgb_u8:
		return format_tuple( RGB, UNSIGNED_BYTE );

	case image::rgba_u8:
		return format_tuple( RGBA, UNSIGNED_BYTE );

	//--- f16 ------------------------------------------------------------------
	case image::rgb_f16:
		return format_tuple( RGB, HALF_FLOAT );

	case image::rgba_f16:
		return format_tuple( RGBA, HALF_FLOAT );

	//--- f32 ------------------------------------------------------------------
	case image::rgb_f32:
		return format_tuple( RGB, FLOAT );

	case image::rgba_f32:
		return format_tuple( RGBA, FLOAT );

	default:
		throw error::runtime( "opengl::frame: " ) << "Unknown frame format: " << format;
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

void default_frame::read( buffer_type& generic_buffer, image::type format )
{
	opengl::buffer& buffer = dynamic_cast< opengl::buffer& >( *generic_buffer );

	if ( buffer.target != PIXEL_PACK_BUFFER )
		throw error::runtime( "opengl::frame: " ) << "Pixel buffer expected";

	BindFramebuffer( READ_FRAMEBUFFER, 0 );
	ReadBuffer( FRONT );

	format_tuple tuple = frame_format( format );
	BindBuffer( PIXEL_PACK_BUFFER, buffer.id );
	ReadPixels( 0, 0, width, height, tuple._0, tuple._1, 0 );
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
	: id(), width( width_ ), height( height_ ), check( true ), colours(), program( program_ )
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

void frame::read( buffer_type& generic_buffer, image::type format )
{
	opengl::buffer& buffer = dynamic_cast< opengl::buffer& >( *generic_buffer );

	if ( buffer.target != PIXEL_PACK_BUFFER )
		throw error::runtime( "opengl::frame: " ) << "Pixel buffer expected";

	BindFramebuffer( READ_FRAMEBUFFER, id );
	ReadBuffer( COLOR_ATTACHMENT0 );

	format_tuple tuple = frame_format( format );
	BindBuffer( PIXEL_PACK_BUFFER, buffer.id );
	ReadPixels( 0, 0, width, height, tuple._0, tuple._1, 0 );
}

void frame::output( attachment_type type, const texture_type& generic_texture )
{
	const opengl::texture& texture = verify< opengl::texture >( *generic_texture, width, height );
	u32 attachment = frame_attachment( type, colours );
	check = true;

	BindFramebuffer( DRAW_FRAMEBUFFER, id );
	FramebufferTexture2D( DRAW_FRAMEBUFFER, attachment, TEXTURE_2D, texture.id, 0 );
}

void frame::output( attachment_type type, const target_type& generic_target )
{
	const opengl::target& target = verify< opengl::target >( *generic_target, width, height );
	u32 attachment = frame_attachment( type, colours );
	check = true;

	BindFramebuffer( DRAW_FRAMEBUFFER, id );
	FramebufferRenderbuffer( DRAW_FRAMEBUFFER, attachment, RENDERBUFFER, target.id );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
