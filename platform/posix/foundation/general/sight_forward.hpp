/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_GENERAL_SIGHT_FORWARD_HPP
#define OOE_FOUNDATION_GENERAL_SIGHT_FORWARD_HPP

#include "foundation/parallel/task.hpp"
#include "foundation/utility/atom.hpp"

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

		sight( const call_type&, u16 width, u16 height );
		~sight( void ) OOE_VISIBLE;

	private:
		atom< u32 > active;
		unique_task< void ( call_type, u16, u16 ) > task;

		void main( call_type, u16, u16 );
	};
}

#endif	// OOE_FOUNDATION_GENERAL_SIGHT_FORWARD_HPP
