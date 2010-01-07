/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <fcntl.h>

#include "foundation/io/descriptor.hpp"
#include "foundation/io/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- descriptor -----------------------------------------------------------------------------------
up_t descriptor::splice( const ooe::descriptor& desc, up_t bytes )
{
	sp_t spliced = ::splice( get(), 0, desc.get(), 0, bytes, SPLICE_F_MOVE );

	if ( spliced == -1 )
		throw error::io( "descriptor: " ) <<
			"Unable to splice " << bytes << " bytes: " << error::number( errno );

	return spliced;
}

OOE_NAMESPACE_END( ( ooe ) )
