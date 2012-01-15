/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_EXECUTABLE_DARWIN_PROGRAM_FORWARD_HPP
#define OOE_FOUNDATION_EXECUTABLE_DARWIN_PROGRAM_FORWARD_HPP

#include "foundation/utility/macro.hpp"
#include "foundation/utility/namespace.hpp"

#define OOE_SIGFAULT SIGBUS

#ifdef __OBJC__
    @class NSAutoreleasePool;
#else
    struct NSAutoreleasePool;
#endif

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

class OOE_VISIBLE autorelease
{
public:
    autorelease( void );
    ~autorelease( void );

private:
    NSAutoreleasePool* pool;
};

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif  // OOE_FOUNDATION_EXECUTABLE_DARWIN_PROGRAM_FORWARD_HPP
