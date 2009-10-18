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
		s32 fd_in = self.illegal_access< true >();
		s32 fd_out = desc.illegal_access< true >();
		sp_t spliced = ::splice( fd_in, 0, fd_out, 0, bytes, SPLICE_F_MOVE );

		if ( spliced == -1 )
			throw error::io( "descriptor: " ) << "Unable to splice: " << error::number( errno );

		return spliced;
	}
}
