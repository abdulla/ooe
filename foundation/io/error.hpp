/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_ERROR_HPP
#define OOE_FOUNDATION_IO_ERROR_HPP

#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( error ) )

struct OOE_VISIBLE io
    : virtual public runtime
{
    io( const c8* string_ )
        : runtime( "io: " )
    {
        *this << string_;
    }

    virtual ~io( void ) throw()
    {
    }
};

OOE_NAMESPACE_END( ( ooe )( error ) )

#endif  // OOE_FOUNDATION_IO_ERROR_HPP
