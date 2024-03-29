/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <X11/Xutil.h>
#include <X11/extensions/Xrandr.h>

#include "foundation/executable/library.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/visual/event_queue.hpp"

#define OOE_X_HEADER_INCLUDED
#include "foundation/visual/view.hpp"
#undef OOE_X_HEADER_INCLUDED

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

//--- view_data ------------------------------------------------------------------------------------
view_data::view_data( const event_queue& queue_ )
    : queue( queue_ ), window( 0 ), visual_info( 0 )
{
    library::find< void ( const view_data& ) >( "context_open", library::next )( *this );
}

view_data::~view_data( void )
{
    // XFree can not be called with null (see man page)
    if ( visual_info )
        XFree( visual_info );
}

//--- view -----------------------------------------------------------------------------------------
view::view( const event_queue& queue, bool full )
    : config( 0 ), resize(), rotate()
{
    if ( !full )
        return;

    config = XRRGetScreenInfo( queue.display, DefaultRootWindow( queue.display ) );

    if ( !config )
        throw error::runtime( "view: " ) << "Unable to get screen configuration";
}

view::~view( void )
{
    if ( config ) // XRRFreeScreenConfigInfo defers to XFree
        XRRFreeScreenConfigInfo( config );
}

OOE_NAMESPACE_END( ( ooe )( platform ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- view_data ------------------------------------------------------------------------------------
view_data::view_data( const event_queue& queue_, u16 width, u16 height, bool full )
    : platform::view_data( queue_ )
{
    up_t root = DefaultRootWindow( queue.display );
    sp_t mask = CWEventMask | CWColormap;
    XSetWindowAttributes attributes;
    std::memset( &attributes, 0, sizeof( attributes ) );

    if ( full )
    {
        mask |= CWOverrideRedirect;
        attributes.override_redirect = 1;
    }

    attributes.event_mask =
        KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
    attributes.colormap = XCreateColormap( queue.display, root, visual_info->visual, 0 );
    window = XCreateWindow( queue.display, root, 0, 0, width, height, 0, visual_info->depth,
        InputOutput, visual_info->visual, mask, &attributes );
    XFreeColormap( queue.display, attributes.colormap );

    if ( !window )
        throw error::runtime( "view: " ) << "Unable to create window";

    XSetWMProtocols( queue.display, window, &queue.wm_delete, 1 );
    XStoreName( queue.display, window, "OOE" );
}

view_data::~view_data( void )
{
    XDestroyWindow( queue.display, window );
}

//--- view -----------------------------------------------------------------------------------------
view::view( const event_queue& queue_, u16 width, u16 height, bool full )
    : view_data( queue_, width, height, full ), platform::view( queue_, full )
{
    if ( !XMapRaised( queue.display, window ) )
        throw error::runtime( "view: " ) << "Unable to map window";
    else if ( !full )
        return;

    resize = XRRConfigCurrentConfiguration( config, &rotate );
    up_t root = DefaultRootWindow( queue.display );
    s32 length;
    XRRScreenSize* sizes = XRRConfigSizes( config, &length );

    for ( s32 i = 0; i != length; ++i )
    {
        if ( sizes[ i ].width != width || sizes[ i ].height != height )
            continue;
        else if ( !XRRSetScreenConfig( queue.display, config, root, i, rotate, CurrentTime ) )
            break;
        else
            throw error::runtime( "view: " ) << "Unable to set screen configuration";
    }
}

view::~view( void )
{
    if ( !config )
        return;

    up_t root = DefaultRootWindow( queue.display );

    if ( XRRSetScreenConfig( queue.display, config, root, resize, rotate, CurrentTime ) )
        OOE_CONSOLE( "view: " << "Unable to set screen configuration" );
}

OOE_NAMESPACE_END( ( ooe ) )
