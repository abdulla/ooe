/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <X11/Xutil.h>
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
	}

	void ungrab( Display* display )
	{
		XUngrabPointer( display, CurrentTime );
		XUngrabKeyboard( display, CurrentTime );
	}
}

namespace ooe
{
//--- platform::view_data ------------------------------------------------------
	platform::view_data::view_data( const event_queue& queue_, u16 width_, u16 height_ )
		: queue( queue_ ), width( width_ ), height( height_ ), window( 0 ), visual_info( 0 )
	{
		queue.grab = make_function( *this, &view_data::grab );
		queue.warp = make_function( *this, &view_data::warp );
		reinterpret_cast< choose_type >
			( library::find( "ooe_opengl_choose", library::next ).function )( *this );
	}

	platform::view_data::~view_data( void )
	{
		// XFree can not be called with null (see man page)
		if ( visual_info )
			XFree( visual_info );
	}

	void platform::view_data::grab( void )
	{
		::grab( queue.display, window );
		warp();
		queue.x = width / 2;
		queue.y = height / 2;
	}

	void platform::view_data::warp( void )
	{
		XWarpPointer( queue.display, 0, window, 0, 0, 0, 0, width / 2, height / 2 );
	}

//--- view_data ----------------------------------------------------------------
	view_data::view_data( const event_queue& queue_, u16 width_, u16 height_, bool full )
		: platform::view_data( queue_, width_, height_ )
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

		XSetWMProtocols( queue.display, window, &queue.wm_delete, 1 );
		XStoreName( queue.display, window, "OOE" );
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
	view::view( const event_queue& queue_, u16 width_, u16 height_, bool full )
		: view_data( queue_, width_, height_, full ), platform::view( queue_, full )
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
		::ungrab( queue.display );

		if ( !config )
			return;

		u32 root = DefaultRootWindow( queue.display );

		if ( XRRSetScreenConfig( queue.display, config, root, resize, rotate, CurrentTime ) )
			OOE_WARNING( "view", "Unable to set screen configuration" );
	}
}
