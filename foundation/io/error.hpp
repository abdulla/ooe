/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_ERROR_HPP
#define OOE_FOUNDATION_IO_ERROR_HPP

#include "foundation/utility/error.hpp"

namespace ooe
{
    namespace error
    {
        struct io;
    }

    struct OOE_VISIBLE error::io
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
}

#endif  // OOE_FOUNDATION_IO_ERROR_HPP
