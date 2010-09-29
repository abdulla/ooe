/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_VIEW_FORWARD_HPP
#define OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_VIEW_FORWARD_HPP

#include "foundation/utility/fundamental.hpp"

#ifdef __OBJC__
    @class NSWindow;
#else
    struct NSWindow;
#endif

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

class view_data
{
public:
    NSWindow* window;

protected:
    view_data( void );
};

struct view
{
};

OOE_NAMESPACE_END( ( ooe )( platform ) )


#endif  // OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_VIEW_FORWARD_HPP
