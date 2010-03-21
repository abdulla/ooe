/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/buffer.hpp"
#include "foundation/opengl_next/context.hpp"
#include "foundation/opengl_next/frame.hpp"
#include "foundation/opengl_next/program.hpp"
#include "foundation/opengl_next/shader.hpp"
#include "foundation/opengl_next/symbol.hpp"
#include "foundation/opengl_next/target.hpp"
#include "foundation/opengl_next/texture.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- driver ---------------------------------------------------------------------------------------
class driver
	: public ooe::driver
{
public:
	driver( const ooe::view_data& );
	virtual ~driver( void );
	virtual void swap( void );

	virtual texture_type texture( const image_pyramid&, texture::type, bool ) const;
	virtual shader_type shader( const std::string&, shader::type ) const;
	virtual program_type program( const shader_vector& ) const;
	virtual buffer_type buffer( up_t, buffer::type, buffer::usage_type ) const;
	virtual target_type target( u32, u32, u8 ) const;
	virtual frame_type frame( void ) const;

private:
	const view_data& view;
	platform::context_type context;
};

driver::driver( const ooe::view_data& view_ )
try
	: view( view_ ), context( context_construct( view ) )
{
	context_current( view, context );
	context_sync( view, context, true );
	load_symbols();

	PixelStorei( PACK_ALIGNMENT, 1 );
	PixelStorei( UNPACK_ALIGNMENT, 1 );
}
catch ( ... )
{
	this->~driver();
}

driver::~driver( void )
{
	context_current( view, 0 );
	context_destruct( view, context );
}

void driver::swap( void )
{
	context_swap( view, context );
	Clear( DEPTH_BUFFER_BIT | COLOR_BUFFER_BIT );
}

texture_type driver::
	texture( const image_pyramid& pyramid, texture::type filter, bool generate_mipmap ) const
{
	if ( is_compressed( pyramid.format ) )
		return new compressed_texture( pyramid, filter, generate_mipmap );
	else
		return new uncompressed_texture( pyramid, filter, generate_mipmap );
}

shader_type driver::shader( const std::string& source, shader::type type ) const
{
	return new opengl::shader( source, type );
}

program_type driver::program( const shader_vector& vector ) const
{
	return new opengl::program( vector );
}

buffer_type driver::buffer( up_t size, buffer::type format, buffer::usage_type usage ) const
{
	return new opengl::buffer( size, format, usage );
}

target_type driver::target( u32 width, u32 height, u8 format ) const
{
	return new opengl::target( width, height, format );
}

frame_type driver::frame( void ) const
{
	return new opengl::frame();
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )

extern "C" driver_type driver_open( const view_data& view )
{
	return new opengl::driver( view );
}
