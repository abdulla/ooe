/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_DESCRIPTOR_FORWARD_HPP
#define OOE_FOUNDATION_IO_DESCRIPTOR_FORWARD_HPP

#include "foundation/executable/environment.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

class descriptor;

OOE_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

struct OOE_VISIBLE descriptor
{
	up_t splice( const ooe::descriptor&, up_t );
	up_t splice( aligned< executable::static_page_size >, up_t );
};

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif	// OOE_FOUNDATION_IO_DESCRIPTOR_FORWARD_HPP
