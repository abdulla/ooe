/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_TEST_UNIT_ERROR_HPP
#define OOE_TEST_UNIT_ERROR_HPP

#include "foundation/utility/error.hpp"

namespace ooe
{
	namespace error
	{
		class assertion;
	}

	struct OOE_VISIBLE error::assertion
		: virtual public runtime
	{
		assertion( void )
			: runtime( "unit::assert: " )
		{
		}

		virtual ~assertion( void ) throw()
		{
		}
	};
}

#endif	// OOE_TEST_UNIT_ERROR_HPP
