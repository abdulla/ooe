/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/symbol.hpp"
#include "foundation/opengl_next/texture.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/visual/graphics_forward.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- public_texture -------------------------------------------------------------------------------
struct public_texture
	: public texture
{
	using texture::pointer;
	using texture::function;
};

//--- texture_api ----------------------------------------------------------------------------------
struct texture_api
{
	const u32 id;
	const u32 width;
	const u32 height;

	texture_api( u32 width_, u32 height_ )
		: id(), width( width_ ), height( height_ )
	{
		GenTextures( 1, const_cast< u32* >( &id ) );
	}

	~texture_api( void )
	{
		DeleteTextures( 1, &id );
	}
};

//--- load_uncompressed ----------------------------------------------------------------------------
void load_uncompressed( opaque_ptr& /*pointer*/, const image& /*image*/, u8 /*level*/ )
{
}

//--- load_compressed ------------------------------------------------------------------------------
void load_compressed( opaque_ptr& /*pointer*/, const image& /*image*/, u8 /*level*/ )
{
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- transform_texture ----------------------------------------------------------------------------
void transform_texture( texture& protected_texture )
{
	public_texture& texture = static_cast< public_texture& >( protected_texture );

	if ( texture.transformed() )
		return;

	texture_handle& handle = *texture.pointer.as< texture_handle >();

	if ( handle.vector.empty() )
		throw error::runtime( "opengl::transform_texture: " ) << "Texture of size " <<
			handle.width << 'x' << handle.height << " and format " << handle.format <<
			" contains no image data";

	opaque_ptr pointer( new texture_api( handle.width, handle.height ), destroy< texture_api > );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
