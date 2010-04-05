/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/buffer.hpp"
#include "foundation/opengl_next/frame.hpp"
#include "foundation/opengl_next/symbol.hpp"
#include "foundation/opengl_next/target.hpp"
#include "foundation/opengl_next/texture.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

typedef tuple< u32, u32, u8 > format_tuple;

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
		return format_tuple( BGR, UNSIGNED_BYTE, 3 );

	case image::bgra_u8:
		return format_tuple( BGRA, UNSIGNED_BYTE, 4 );

	//--- u8 -------------------------------------------------------------------
	case image::rgb_u8:
		return format_tuple( RGB, UNSIGNED_BYTE, 3 );

	case image::rgba_u8:
		return format_tuple( RGBA, UNSIGNED_BYTE, 4 );

	//--- f16 ------------------------------------------------------------------
	case image::rgb_f16:
		return format_tuple( RGB, HALF_FLOAT, 6 );

	case image::rgba_f16:
		return format_tuple( RGBA, HALF_FLOAT, 8 );

	//--- f32 ------------------------------------------------------------------
	case image::rgb_f32:
		return format_tuple( RGB, FLOAT, 12 );

	case image::rgba_f32:
		return format_tuple( RGBA, FLOAT, 16 );

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

void frame_check( bool& check, u32 target )
{
	if ( !check )
		return;

	s32 status = CheckFramebufferStatus( target );

	if ( status != FRAMEBUFFER_COMPLETE )
		throw error::runtime( "opengl::frame: " ) << "Frame is incomplete: 0x" << hex( status );

	check = false;
}

void frame_read( buffer_type& generic_buffer, image::type format, s32 id, u32 width, u32 height,
	u32 target, bool& check )
{
	opengl::buffer& buffer = dynamic_cast< opengl::buffer& >( *generic_buffer );

	if ( buffer.target != PIXEL_PACK_BUFFER )
		throw error::runtime( "opengl::frame: " ) << "Pixel buffer expected";

	format_tuple tuple = frame_format( format );
	up_t size = tuple._2 * width * height;

	if ( size > buffer.size )
		throw error::runtime( "opengl::frame: " ) <<
			"Pixel buffer size " << buffer.size << " < " << size;

	BindFramebuffer( READ_FRAMEBUFFER, id );
	frame_check( check, READ_FRAMEBUFFER );
	ReadBuffer( target );

	BindBuffer( PIXEL_PACK_BUFFER, buffer.id );
	ReadPixels( 0, 0, width, height, tuple._0, tuple._1, 0 );
}

void frame_write( const frame_type& generic_frame, s32 id, u32 width, u32 height, u32 read_target,
	u32 draw_target )
{
	opengl::frame& input = dynamic_cast< opengl::frame& >( *generic_frame );

	BindFramebuffer( READ_FRAMEBUFFER, input.id );
	frame_check( input.check, READ_FRAMEBUFFER );
	ReadBuffer( read_target );

	BindFramebuffer( DRAW_FRAMEBUFFER, id );
	DrawBuffers( 1, &draw_target );

	BlitFramebuffer( 0, 0, input.width, input.height, 0, 0, width, height,
		COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT, NEAREST );
}

void frame_clear( s32 id, bool& check )
{
	BindFramebuffer( DRAW_FRAMEBUFFER, id );
	frame_check( check, DRAW_FRAMEBUFFER );
	Clear( COLOR_BUFFER_BIT | DEPTH_BUFFER_BIT );
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

void default_frame::read( buffer_type& generic_buffer, image::type format, u8 index ) const
{
	bool check = false;
	frame_read( generic_buffer, format, 0, width, height, BACK_LEFT + index, check );
}

void default_frame::write( const frame_type& generic_frame, u8 index )
{
	frame_write( generic_frame, 0, width, height, COLOR_ATTACHMENT0 + index, BACK_LEFT );
}

void default_frame::clear( void )
{
	bool check = false;
	frame_clear( 0, check );
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
frame::frame( u32 /* program */, u32 width_, u32 height_ )
	: id(), width( width_ ), height( height_ ), check( true ), colours()
{
	GenFramebuffers( 1, const_cast< u32* >( &id ) );
}

frame::~frame( void )
{
	DeleteFramebuffers( 1, &id );
}

void frame::read( buffer_type& generic_buffer, image::type format, u8 index ) const
{
	if ( !colours.size() )
		throw error::runtime( "opengl::frame: " ) << "Frame has no colour attachment";

	frame_read( generic_buffer, format, id, width, height, COLOR_ATTACHMENT0 + index, check );
}

void frame::write( const frame_type& generic_frame, u8 index )
{
	frame_write( generic_frame, id, width, height, COLOR_ATTACHMENT0 + index, COLOR_ATTACHMENT0 );
}

void frame::clear( void )
{
	frame_clear( id, check );
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
