/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_PYTHON_ERROR_HPP
#define OOE_COMPONENT_PYTHON_ERROR_HPP

#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( error ) )

struct OOE_VISIBLE python
    : virtual public runtime
{
    python( void )
        : runtime( "python: " )
    {
    }

    virtual ~python( void ) throw()
    {
    }
};

OOE_NAMESPACE_END( ( ooe )( error ) )

#endif  // OOE_COMPONENT_PYTHON_ERROR_HPP
