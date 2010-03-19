/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/symbol.hpp"
#include "foundation/opengl_next/target.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

u32 target_format( u8 format )
{
	switch ( format )
	{
	case image::bgr_u8:
		return RGB8;

	case image::bgra_u8:
		return RGBA8;

	//--- u8 -------------------------------------------------------------------
	case image::rgb_u8:
		return RGB8;

	case image::rgba_u8:
		return RGBA8;

	//--- f16 ------------------------------------------------------------------
	case image::rgb_f16:
		return RGB16F;

	case image::rgba_f16:
		return RGBA16F;

	//--- f32 ------------------------------------------------------------------
	case image::rgb_f32:
		return RGB32F;

	case image::rgba_f32:
		return RGBA32F;

	//--- depth ----------------------------------------------------------------
	case ooe::target::depth_u24:
		return DEPTH_COMPONENT24;

	case ooe::target::depth_f32:
		return DEPTH_COMPONENT32F;

	//--------------------------------------------------------------------------
	default:
		throw error::runtime( "opengl::texture: " ) <<
			"Unknown uncompressed image format: " << format;
	}
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- target ---------------------------------------------------------------------------------------
target::target( u32 width, u32 height, u8 format )
	: id()
{
	GenRenderbuffers( 1, const_cast< u32* >( &id ) );

	BindRenderbuffer( RENDERBUFFER, id );
	RenderbufferStorage( RENDERBUFFER, target_format( format ), width, height );
}

target::~target( void )
{
	DeleteRenderbuffers( 1, &id );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
