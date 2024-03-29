/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#define GLX_GLXEXT_PROTOTYPES
#include <GL/glx.h>
#undef GLX_GLXEXT_PROTOTYPES

#include "foundation/opengl/context.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/visual/event_queue.hpp"

#define OOE_X_HEADER_INCLUDED
#include "foundation/visual/view.hpp"
#undef OOE_X_HEADER_INCLUDED

OOE_NAMESPACE_BEGIN( ( ooe ) )

platform::context_type context_construct( const ooe::view_data& view )
{
    platform::context_type glx = glXCreateContext( view.queue.display, view.visual_info, 0, true );
    XFree( view.visual_info );
    view.visual_info = 0;

    if ( !glx )
        throw error::runtime( "opengl: " ) << "Unable to create context";

    return glx;
}

void context_destruct( const ooe::view_data& view, platform::context_type glx )
{
    glXDestroyContext( view.queue.display, glx );
}

void context_current( const ooe::view_data& view, platform::context_type glx )
{
    if ( !glXMakeCurrent( view.queue.display, view.window, glx ) )
        throw error::runtime( "opengl: " ) << "Unable to set current context";
}

void context_sync( const ooe::view_data& view, platform::context_type, bool vsync )
{
    if ( glXSwapIntervalEXT( view.queue.display, view.window, vsync ) )
        throw error::runtime( "opengl: " ) << "Unable to set vertical sync";
}

void context_swap( const ooe::view_data& view, platform::context_type )
{
    glXSwapBuffers( view.queue.display, view.window );
}

OOE_NAMESPACE_END( ( ooe ) )

extern "C" void OOE_VISIBLE context_open( const ooe::view_data& view )
{
    ooe::s32 attributes[] =
    {
        GLX_DEPTH_SIZE,     24,
        GLX_DOUBLEBUFFER,
        0
    };

    view.visual_info =
        glXChooseVisual( view.queue.display, DefaultScreen( view.queue.display ), attributes );

    if ( !view.visual_info )
        throw ooe::error::runtime( "view: " ) << "No matching visual information";
}
