/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_DESCRIPTOR_FORWARD_HPP
#define OOE_FOUNDATION_IO_DESCRIPTOR_FORWARD_HPP

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"

namespace ooe
{
	namespace platform
	{
		struct descriptor;
	}

	struct OOE_VISIBLE platform::descriptor
	{
		up_t splice( const descriptor&, up_t );
	};
}

#endif	// OOE_FOUNDATION_IO_DESCRIPTOR_FORWARD_HPP
