/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_CHECKPOINT_HPP
#define OOE_COMPONENT_REGISTRY_CHECKPOINT_HPP

#include "foundation/io/descriptor.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

class OOE_VISIBLE checkpoint
{
public:
	typedef scoped_ptr< descriptor > desc_ptr;

    checkpoint( void );
    bool update( void );

private:
	desc_ptr desc;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_REGISTRY_CHECKPOINT_HPP
