/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VISUAL_DARWIN_SIGHT_FORWARD_HPP
#define OOE_FOUNDATION_VISUAL_DARWIN_SIGHT_FORWARD_HPP

#include "foundation/utility/function.hpp"
#include "foundation/utility/fundamental.hpp"

#ifdef __OBJC__
    @class Sight;
#else
    struct Sight;
#endif

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

class sight
{
protected:
    typedef function< void ( const u8* ) > call_type;

    sight( const call_type&, u16, u16 );
    ~sight( void ) OOE_VISIBLE;

private:
    call_type call;
    Sight* id;
};

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif  // OOE_FOUNDATION_VISUAL_DARWIN_SIGHT_FORWARD_HPP
