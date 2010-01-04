/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_GENERAL_VIEW_HPP
#define OOE_FOUNDATION_GENERAL_VIEW_HPP

#include "foundation/general/view_forward.hpp"
#include "foundation/utility/macro.hpp"
#include "foundation/utility/noncopyable.hpp"

namespace ooe
{
	struct event_queue;

//--- view_data ----------------------------------------------------------------
	class view_data
		: private noncopyable, public platform::view_data
	{
	protected:
		view_data( const event_queue&, u16, u16, bool );
		~view_data( void );
	};

//--- view ---------------------------------------------------------------------
	struct OOE_VISIBLE view
		: public view_data, private platform::view
	{
		view( const event_queue&, u16, u16, bool );
		~view( void );
	};
}

#endif	// OOE_FOUNDATION_GENERAL_VIEW_HPP
