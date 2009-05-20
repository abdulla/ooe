/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <sys/ioctl.h>
#include <sys/time.h>

#undef __STRICT_ANSI__
#include <linux/videodev.h>
#define __STRICT_ANSI__

#include "foundation/executable/timer.hpp"
#include "foundation/general/sight.hpp"
#include "foundation/io/memory.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;

	memory initialise( descriptor& desc, u16 width, u16 height, up_t* offset )
	{
		video_capability capability;
		desc.control( VIDIOCGCAP, &capability );

		if ( width > capability.maxwidth || height > capability.maxheight )
			throw error::runtime( "sight: " ) << "Exceeded maximum video size";

		video_mbuf buffer;
		desc.control( VIDIOCGMBUF, &buffer );

		if ( buffer.frames < 2 )
			throw error::runtime( "sight: " ) << "Unable to do double buffering";

		offset[ 0 ] = buffer.offsets[ 0 ];
		offset[ 1 ] = buffer.offsets[ 1 ];
		return memory( desc, memory::read, buffer.size );
	}
}

namespace ooe
{
//--- platform::sight ----------------------------------------------------------
	platform::sight::sight( const call_type& call, u16 width, u16 height )
		: active( true ), task( make_function( *this, &sight::main ), call, width, height )
	{
	}

	platform::sight::~sight( void )
	{
		active = false;
		task();
	}

	void platform::sight::main( call_type call, u16 width, u16 height )
	{
		up_t offset[ 2 ];
		ooe::descriptor desc( "/dev/video0" );
		memory memory( initialise( desc, width, height, offset ) );

		u8 index = 0;
		epoch_t sleep( 0, 15000000 );
		video_mmap map = { index, height, width, VIDEO_PALETTE_RGB24 };

		for ( ; active; index = 1 - index )
		{
			map.frame = 1 - index;
			desc.control( VIDIOCMCAPTURE, &map );
			desc.control( VIDIOCSYNC, &index );
			call( memory.as< u8 >() + offset[ index ] );
			timer::sleep( sleep );
		}
	}

//--- sight --------------------------------------------------------------------
	sight::sight( const call_type& call_, u16 width, u16 height )
		: platform::sight( call_, width, height )
	{
	}

	uncompressed_image::type sight::format( void )
	{
		return uncompressed_image::bgr_u8;
	}
}
