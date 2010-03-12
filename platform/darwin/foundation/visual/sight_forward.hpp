/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_SIGHT_FORWARD_HPP
#define OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_SIGHT_FORWARD_HPP

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

#endif	// OOE_PLATFORM_DARWIN_FOUNDATION_VISUAL_SIGHT_FORWARD_HPP
