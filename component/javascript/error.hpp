/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_JAVASCRIPT_ERROR_HPP
#define OOE_COMPONENT_JAVASCRIPT_ERROR_HPP

#include "foundation/utility/error.hpp"

namespace ooe
{
	namespace error
	{
		struct javascript;
	}

	struct OOE_VISIBLE error::javascript
		: virtual public runtime
	{
		javascript( void )
			: runtime( "javascript: " )
		{
		}

		virtual ~javascript( void ) throw()
		{
		}
	};
}

#endif	// OOE_COMPONENT_JAVASCRIPT_ERROR_HPP
