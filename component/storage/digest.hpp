/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_STORAGE_DIGEST_HPP
#define OOE_COMPONENT_STORAGE_DIGEST_HPP

#include <openssl/sha.h>

OOE_NAMESPACE_BEGIN( ( ooe ) )

struct sha256
{
    u8 digest[ SHA256_DIGEST_LENGTH ];
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_STORAGE_DIGEST_HPP
