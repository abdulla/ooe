/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_VFS_HPP
#define OOE_FOUNDATION_IO_VFS_HPP

#include <list>

#include "foundation/io/descriptor.hpp"
#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

class OOE_VISIBLE vfs
{
public:
    typedef tuple< const std::string, const std::string > tuple_type;
    typedef std::list< tuple_type > list_type;

    vfs( void );

    void insert( const std::string&, const std::string& );
    bool empty( void ) const;

    descriptor operator []( const std::string& ) const;

private:
    mutable list_type list;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_IO_VFS_HPP
