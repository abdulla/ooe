/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_STORAGE_BLOB_HPP
#define OOE_COMPONENT_STORAGE_BLOB_HPP

#include "component/storage/digest.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

struct tree;

struct blob
{
    blob( const sha256& );

    void write( up_t, up_t );
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_STORAGE_BLOB_HPP
