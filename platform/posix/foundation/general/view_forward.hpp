/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_GENERAL_VIEW_FORWARD_HPP
#define OOE_FOUNDATION_GENERAL_VIEW_FORWARD_HPP

#include "foundation/utility/fundamental.hpp"

#ifndef OOE_X_HEADER_INCLUDED
typedef struct XVisualInfo XVisualInfo;
#endif	// OOE_X_HEADER_INCLUDED

typedef struct _XRRScreenConfiguration XRRScreenConfiguration;

namespace ooe
{
	struct event_queue;

	namespace platform
	{
		class view_data;
		class view;

		typedef void ( choose_type )( view_data& );
	}

	class platform::view_data
	{
	public:
		const event_queue& queue;
		u16 width;
		u16 height;
		u32 window;
		mutable XVisualInfo* visual_info;

	protected:
		view_data( const event_queue&, u16, u16 );
		~view_data( void );

	private:
		void grab( void );
		void warp( void );
	};

	class platform::view
	{
	protected:
		XRRScreenConfiguration* config;
		u16 resize;
		u16 rotate;

		view( const event_queue&, bool );
		~view( void );
	};
}

#endif	// OOE_FOUNDATION_GENERAL_VIEW_FORWARD_HPP
