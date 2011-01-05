/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl/symbol.hpp"
#include "foundation/opengl/target.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( opengl ) )

u32 target_format( image_format::type format )
{
    switch ( format )
    {
    case image_format::bgr_u8:
        return RGB8;

    case image_format::bgra_u8:
        return RGBA8;

    //--- u8 -------------------------------------------------------------------
    case image_format::rgb_u8:
        return RGB8;

    case image_format::rgba_u8:
        return RGBA8;

    //--- f16 ------------------------------------------------------------------
    case image_format::rgb_f16:
        return RGB16F;

    case image_format::rgba_f16:
        return RGBA16F;

    //--- f32 ------------------------------------------------------------------
    case image_format::rgb_f32:
        return RGB32F;

    case image_format::rgba_f32:
        return RGBA32F;

    //--- depth ----------------------------------------------------------------
    case image_format::depth_u24:
        return DEPTH_COMPONENT24;

    //--------------------------------------------------------------------------
    default:
        throw error::runtime( "opengl::target: " ) << "Unknown image format: " << format;
    }
}

OOE_ANONYMOUS_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- target ---------------------------------------------------------------------------------------
target::target( const image_metadata& metadata )
    : id(), width( metadata.width ), height( metadata.height )
{
    GenRenderbuffers( 1, &id );

    BindRenderbuffer( RENDERBUFFER, id );
    RenderbufferStorage
        ( RENDERBUFFER, target_format( metadata.format ), metadata.width, metadata.height );
}

target::~target( void )
{
    DeleteRenderbuffers( 1, &id );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )
