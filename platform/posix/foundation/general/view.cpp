/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <X11/Xutil.h>
#include <X11/extensions/xf86dga.h>
#include <X11/extensions/Xrandr.h>

#include "foundation/general/event_queue.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/utility/error.hpp"

#define OOE_X_HEADER_INCLUDED
#include "foundation/general/view.hpp"
#undef OOE_X_HEADER_INCLUDED

namespace
{
	using namespace ooe;
	typedef void ( * choose_type )( const view_data& );

	const u32 event_mask =
		KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask;

	void grab( Display* display, u32 window )
	{
		XGrabKeyboard( display, window, true, GrabModeAsync, GrabModeAsync, CurrentTime );
		XGrabPointer( display, window, true, 0, GrabModeAsync, GrabModeAsync, 0, 0, CurrentTime );
		XF86DGADirectVideo( display, DefaultScreen( display ), XF86DGADirectMouse );
		XWarpPointer( display, 0, window, 0, 0, 0, 0, 0, 0 );
	}

	void ungrab( Display* display )
	{
		XF86DGADirectVideo( display, DefaultScreen( display ), 0 );
		XUngrabPointer( display, CurrentTime );
		XUngrabKeyboard( display, CurrentTime );
	}
}

namespace ooe
{
//--- platform::view_data ------------------------------------------------------
	platform::view_data::view_data( const event_queue& queue_ )
		: queue( queue_ ), window( 0 ), visual_info( 0 )
	{
		queue.configure = make_function( *this, &view_data::configure );
		reinterpret_cast< choose_type >
			( library::find( "ooe_opengl_choose", library::next ).function )( *this );
	}

	platform::view_data::~view_data( void )
	{
		if ( visual_info )	// XFree can not be called with null (see man page)
			XFree( visual_info );
	}

	void platform::view_data::configure( void )
	{
		ungrab( queue.display );
		grab( queue.display, window );

		// remove corresponding motion notify event
		XEvent event;
		XWindowEvent( queue.display, window, PointerMotionMask, &event );
	}

//--- view_data ----------------------------------------------------------------
	view_data::view_data( const event_queue& queue_, u16 width, u16 height, bool full )
		: platform::view_data( queue_ )
	{
		u32 root = DefaultRootWindow( queue.display );
		u32 mask = CWEventMask | CWColormap;
		XSetWindowAttributes attributes;
		std::memset( &attributes, 0, sizeof( attributes ) );

		if ( full )
		{
			mask |= CWOverrideRedirect;
			attributes.override_redirect = 1;
		}

		attributes.event_mask = event_mask | StructureNotifyMask | ExposureMask;
		attributes.colormap = XCreateColormap( queue.display, root, visual_info->visual, 0 );
		window = XCreateWindow( queue.display, root, 0, 0, width, height, 0, visual_info->depth,
			InputOutput, visual_info->visual, mask, &attributes );
		XFreeColormap( queue.display, attributes.colormap );

		if ( !window )
			throw error::runtime( "view: " ) << "Unable to create window";
		else if ( full )
			return;

		XStoreName( queue.display, window, "OOE" );
		Atom wm_delete = XInternAtom( queue.display, "WM_DELETE_WINDOW", 1 );
		XSetWMProtocols( queue.display, window, &wm_delete, 1 );
	}

	view_data::~view_data( void )
	{
		XDestroyWindow( queue.display, window );
	}

//--- platform::view -----------------------------------------------------------
	platform::view::view( const event_queue& queue, bool full )
		: config( 0 ), resize(), rotate()
	{
		if ( !full )
			return;

		config = XRRGetScreenInfo( queue.display, DefaultRootWindow( queue.display ) );

		if ( !config )
			throw error::runtime( "view: " ) << "Unable to get screen configuration";
	}

	platform::view::~view( void )
	{
		if ( config ) // XRRFreeScreenConfigInfo defers to XFree
			XRRFreeScreenConfigInfo( config );
	}

//--- view ---------------------------------------------------------------------
	view::view( const event_queue& queue_, u16 width, u16 height, bool full )
		: view_data( queue_, width, height, full ), platform::view( queue_, full )
	{
		if ( !XMapRaised( queue.display, window ) )
			throw error::runtime( "view: " ) << "Unable to map window";
		else if ( !full )
			return;

		resize = XRRConfigCurrentConfiguration( config, &rotate );
		u32 root = DefaultRootWindow( queue.display );
		s32 length;
		XRRScreenSize* sizes = XRRConfigSizes( config, &length );

		for ( s32 i = 0; i != length; ++i )
		{
			if ( sizes[ i ].width != width || sizes[ i ].height != height )
				continue;
			else if ( !XRRSetScreenConfig( queue.display, config, root, i, rotate, CurrentTime ) )
				break;

			throw error::runtime( "view: " ) << "Unable to set screen configuration";
		}
	}

	view::~view( void )
	{
		ungrab( queue.display );

		if ( !config )
			return;

		u32 root = DefaultRootWindow( queue.display );

		if ( XRRSetScreenConfig( queue.display, config, root, resize, rotate, CurrentTime ) )
			OOE_WARNING( "view", "Unable to set screen configuration" );
	}
}
