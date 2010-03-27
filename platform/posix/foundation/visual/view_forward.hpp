/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_POSIX_FOUNDATION_VISUAL_VIEW_FORWARD_HPP
#define OOE_PLATFORM_POSIX_FOUNDATION_VISUAL_VIEW_FORWARD_HPP

#include "foundation/utility/fundamental.hpp"

#ifndef OOE_X_HEADER_INCLUDED
typedef struct XVisualInfo XVisualInfo;
#endif	// OOE_X_HEADER_INCLUDED

typedef struct _XRRScreenConfiguration XRRScreenConfiguration;

OOE_NAMESPACE_BEGIN( ( ooe ) )

struct event_queue;

OOE_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

class view_data
{
public:
	const event_queue& queue;
	u16 width;
	u16 height;
	up_t window;
	mutable XVisualInfo* visual_info;

protected:
	view_data( const event_queue&, u16, u16 );
	~view_data( void );

	void warp( void );
};

class view
{
protected:
	XRRScreenConfiguration* config;
	u16 resize;
	u16 rotate;
	up_t cursor;

	view( const event_queue&, bool );
	~view( void );
};

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif	// OOE_PLATFORM_POSIX_FOUNDATION_VISUAL_VIEW_FORWARD_HPP
