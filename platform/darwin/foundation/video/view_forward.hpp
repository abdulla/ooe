/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VIDEO_VIEW_FORWARD_HPP
#define OOE_FOUNDATION_VIDEO_VIEW_FORWARD_HPP

#include "foundation/utility/fundamental.hpp"

#ifdef __OBJC__
	@class NSWindow;
#else
	struct NSWindow;
#endif

namespace ooe
{
	namespace platform
	{
		class view_data;
		struct view;
	}

	class platform::view_data
	{
	public:
		NSWindow* window;

	protected:
		view_data( void );
	};

	struct platform::view
	{
	};
}

#endif	// OOE_FOUNDATION_VIDEO_VIEW_FORWARD_HPP
