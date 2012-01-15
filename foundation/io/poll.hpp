/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_POLL_HPP
#define OOE_FOUNDATION_IO_POLL_HPP

#include <vector>

#include <poll.h>

#include "foundation/io/descriptor.hpp"
#include "foundation/utility/function.hpp"
#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- poll -----------------------------------------------------------------------------------------
class OOE_VISIBLE poll
{
public:
    enum type
    {
        error,
        disconnect,
        input
    };

    typedef function< void ( const descriptor&, type ) > function_type;

    void insert( const descriptor&, const function_type& = function_type() );
    void wait( void ) const;

private:
    typedef tuple< descriptor, function_type > datum_tuple;
    typedef std::vector< datum_tuple > datum_vector;
    typedef std::vector< pollfd > fd_vector;

    datum_vector data;
    mutable fd_vector fds;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_IO_POLL_HPP
