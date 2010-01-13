/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <AppKit/AppKit.h>
#include <OpenGL/OpenGL.h>

#include "foundation/opengl/context.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/video/view.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

void context_multithread( platform::context_type nsgl, bool enable )
{
	CGLError ( * cgl_set )( CGLContextObj, CGLContextEnable ) = enable ? CGLEnable : CGLDisable;
	CGLContextObj cgl = static_cast< CGLContextObj >( nsgl.CGLContextObj );

	if ( cgl_set( cgl, kCGLCEMPEngine ) )
		throw error::runtime( "opengl: " ) << "Unable to set multi-threaded execution";
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

platform::context_type context_construct( const ooe::view_data& )
{
	NSOpenGLPixelFormatAttribute attribute[] =
	{
		NSOpenGLPFAColorSize,		24,
		NSOpenGLPFAAlphaSize,		8,
		NSOpenGLPFADepthSize,		32,

		NSOpenGLPFADoubleBuffer,	true,
		NSOpenGLPFANoRecovery,		true,
		NSOpenGLPFAAccelerated,		true,
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

	return nsgl;
}

void context_destruct( const ooe::view_data&, platform::context_type nsgl )
{
	[ nsgl release ];
}

bool context_current( const ooe::view_data&, platform::context_type nsgl )
{
	[ nsgl makeCurrentContext ];
	return true;
}

bool context_sync( const ooe::view_data&, platform::context_type nsgl, bool vsync )
{
	context_multithread( nsgl, !vsync );
	const s32 value = vsync;
	[ nsgl setValues: &value forParameter: NSOpenGLCPSwapInterval ];
	return true;
}

void context_swap( const ooe::view_data&, platform::context_type nsgl )
{
	[ nsgl flushBuffer ];
}

void setup_context( const ooe::view_data& view, platform::context_type nsgl )
{
	[ nsgl makeCurrentContext ];

	if ( view.window )
		[ nsgl setView: view.window.contentView ];
	else
		[ nsgl setFullScreen ];
}

OOE_NAMESPACE_END( ( ooe ) )
