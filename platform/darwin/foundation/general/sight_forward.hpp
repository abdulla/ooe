/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_GENERAL_SIGHT_FORWARD_HPP
#define OOE_FOUNDATION_GENERAL_SIGHT_FORWARD_HPP

#include "foundation/utility/function.hpp"
#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"

#ifdef __OBJC__
	@class Sight;
#else
	struct Sight;
#endif

namespace ooe
{
	namespace platform
	{
		class sight;
	}

	class platform::sight
	{
	protected:
		typedef function< void ( const u8* ) > call_type;

		sight( const call_type&, u16, u16 );
		~sight( void ) OOE_VISIBLE;

	private:
		call_type call;
		Sight* id;
	};
}

#endif	// OOE_FOUNDATION_GENERAL_SIGHT_FORWARD_HPP
