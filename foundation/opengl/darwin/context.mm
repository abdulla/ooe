/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <AppKit/AppKit.h>
#include <OpenGL/OpenGL.h>

#include "foundation/opengl/context.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/visual/view.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

platform::context_type context_construct( const ooe::view_data& view )
{
    NSOpenGLPixelFormatAttribute attribute[] =
    {
        NSOpenGLPFAOpenGLProfile,   NSOpenGLProfileVersion3_2Core,
        NSOpenGLPFADepthSize,       24,
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,
        NSOpenGLPFAAllowOfflineRenderers,
        0
    };

    NSOpenGLPixelFormat* format = [ [ NSOpenGLPixelFormat alloc ] initWithAttributes: attribute ];

    if ( !format )
        throw error::runtime( "opengl: " ) << "Unable to initialise pixel format";

    platform::context_type nsgl =
        [ [ NSOpenGLContext alloc ] initWithFormat: format shareContext: 0 ];
    [ format release ];

    if ( !nsgl )
        throw error::runtime( "opengl: " ) << "Unable to create context";
    else if ( view.window )
        [ nsgl setView: view.window.contentView ];
    else
        [ nsgl setFullScreen ];

    return nsgl;
}

void context_destruct( const ooe::view_data&, platform::context_type nsgl )
{
    [ nsgl release ];
}

void context_current( const ooe::view_data&, platform::context_type nsgl )
{
    [ nsgl makeCurrentContext ];
}

void context_sync( const ooe::view_data&, platform::context_type nsgl, bool vsync )
{
    const s32 value = vsync;
    [ nsgl setValues: &value forParameter: NSOpenGLCPSwapInterval ];
}

void context_swap( const ooe::view_data&, platform::context_type nsgl )
{
    [ nsgl flushBuffer ];
}

OOE_NAMESPACE_END( ( ooe ) )
