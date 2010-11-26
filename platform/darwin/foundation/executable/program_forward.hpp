/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_DARWIN_FOUNDATION_EXECUTABLE_PROGRAM_FORWARD_HPP
#define OOE_PLATFORM_DARWIN_FOUNDATION_EXECUTABLE_PROGRAM_FORWARD_HPP

#include "foundation/utility/namespace.hpp"

#define OOE_SIGFAULT SIGBUS

#ifdef __OBJC__
    @class NSAutoreleasePool;
#else
    struct NSAutoreleasePool;
#endif

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

class autorelease
{
public:
    autorelease( void );
    ~autorelease( void );

private:
    NSAutoreleasePool* pool;
};

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif  // OOE_PLATFORM_DARWIN_FOUNDATION_EXECUTABLE_PROGRAM_FORWARD_HPP
