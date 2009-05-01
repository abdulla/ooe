/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_TOOLS_INTERFACE_HPP
#define OOE_EXTERNAL_TOOLS_INTERFACE_HPP

#include "foundation/io/file.hpp"

namespace ooe
{
	namespace tools
	{
		void encode_write( file&, const void*, const up_t );
	}
}

#endif	// OOE_EXTERNAL_TOOLS_INTERFACE_HPP
