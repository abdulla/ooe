/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_POSIX_FOUNDATION_VISUAL_SIGHT_FORWARD_HPP
#define OOE_PLATFORM_POSIX_FOUNDATION_VISUAL_SIGHT_FORWARD_HPP

#include "foundation/io/poll.hpp"
#include "foundation/parallel/thread.hpp"
#include "foundation/utility/atom.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

class sight
{
protected:
    typedef function< void ( const u8* ) > call_type;

    sight( const call_type&, u16 width, u16 height );
    ~sight( void ) OOE_VISIBLE;

private:
    call_type call;
    u16 width;
    u16 height;

    atom< u32 > state;
    ooe::thread thread;

    void status( const descriptor&, poll::type );
    void main( void* );
};

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif  // OOE_PLATFORM_POSIX_FOUNDATION_VISUAL_SIGHT_FORWARD_HPP
