/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#undef __CONSTANT_CFSTRINGS__
#include <AppKit/AppKit.h>

#include "foundation/general/view.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
//--- platform::view_data ------------------------------------------------------
	platform::view_data::view_data( void )
		: window( 0 )
	{
	}

//--- view_data ----------------------------------------------------------------
	view_data::view_data( const event_queue&, u16 width, u16 height, bool full )
		: platform::view_data()
	{
		if ( full )
			return;

		ProcessSerialNumber serial = { 0, kCurrentProcess };

		if ( TransformProcessType( &serial, kProcessTransformToForegroundApplication ) ||
			SetFrontProcess( &serial ) )
			throw error::runtime( "view: " ) << "Unable to set process to foreground";

		NSRect rect = { { 0, 0 }, { width, height } };
		window = [ [ NSWindow alloc ] initWithContentRect: rect styleMask: NSTitledWindowMask
			backing: NSBackingStoreBuffered defer: false ];

		if ( !window )
			throw error::runtime( "view: " ) << "Unable to initialise window";

		[ window orderFront: 0 ];
		window.title = @"OOE";
		window.acceptsMouseMovedEvents = true;
	}

	view_data::~view_data( void )
	{
		[ window release ];
	}

//--- view ---------------------------------------------------------------------
	view::view( const event_queue& queue, u16 width, u16 height, bool full )
		: view_data( queue, width, height, full ), platform::view()
	{
		CGPoint point = { width / 2, height / 2 };

		if ( !full )
		{
			NSRect rect = window.frame;
			point.x += rect.origin.x;
			point.y += rect.origin.y;
		}

		CGDirectDisplayID display = CGMainDisplayID();
		CGDisplayMoveCursorToPoint( display, point );
		CGDisplayHideCursor( display );
		CGAssociateMouseAndMouseCursorPosition( false );

		if ( !full )
			return;

		CFArrayRef modes = CGDisplayCopyAllDisplayModes( display, 0 );
		CGDisplayMode* mode = 0;

		for ( sp_t i = 0; i != CFArrayGetCount( modes ); ++i )
		{
			CGDisplayMode* current = ( CGDisplayMode* )CFArrayGetValueAtIndex( modes, i );

			if ( CGDisplayModeGetWidth( current ) != width &&
				CGDisplayModeGetHeight( current ) != height )
				continue;

			CFStringRef encoding = CGDisplayModeCopyPixelEncoding( current );

			if ( !CFStringCompare( encoding, CFSTR( IO32BitDirectPixels ), 0 ) )
			{
				mode = current;
				break;
			}
		}

		CGDisplayCapture( display );

		if ( !mode )
			throw error::runtime( "view: " ) << "Unable to find display mode";
		else if ( CGDisplaySetDisplayMode( display, mode, 0 ) )
			throw error::runtime( "view: " ) << "Unable to set display mode";
	}

	view::~view( void )
	{
		if ( !window )
			CGReleaseAllDisplays();
	}
}
