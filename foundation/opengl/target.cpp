/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl/symbol.hpp"
#include "foundation/opengl/target.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

u32 target_format( image::type format )
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

	//--------------------------------------------------------------------------
	default:
		throw error::runtime( "opengl::target: " ) << "Unknown image format: " << format;
	}
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- target ---------------------------------------------------------------------------------------
target::target( u32 width_, u32 height_, image::type format )
	: id(), width( width_ ), height( height_ )
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
