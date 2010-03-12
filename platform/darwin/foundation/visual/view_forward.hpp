/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_VIEW_FORWARD_HPP
#define OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_VIEW_FORWARD_HPP

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

#endif	// OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_VIEW_FORWARD_HPP
