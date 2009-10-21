/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <fcntl.h>

#include "foundation/io/descriptor.hpp"
#include "foundation/io/error.hpp"

namespace ooe
{
//--- platform::descriptor -----------------------------------------------------
	up_t platform::descriptor::splice( const ooe::descriptor& desc, up_t bytes )
	{
		ooe::descriptor& self = *static_cast< ooe::descriptor* >( this );
		sp_t spliced = ::splice( self.get(), 0, desc.get(), 0, bytes, SPLICE_F_MOVE );

		if ( spliced == -1 )
			throw error::io( "descriptor: " ) << "Unable to splice: " << error::number( errno );

		return spliced;
	}
}
