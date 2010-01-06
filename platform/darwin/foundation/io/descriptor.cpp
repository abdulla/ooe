/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <unistd.h>
#include <sys/socket.h>

#include "foundation/io/descriptor.hpp"
#include "foundation/io/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- descriptor -----------------------------------------------------------------------------------
up_t descriptor::splice( const ooe::descriptor& desc, up_t bytes )
{
	s64 spliced;

	if ( sendfile( get(), desc.get(), 0, &spliced, 0, 0 ) )
		throw error::io( "descriptor: " ) <<
			"Unable to splice " << bytes << " bytes: " << error::number( errno );

	return spliced;
}

up_t descriptor::splice( aligned< executable::static_page_size > buffer, up_t bytes )
{
	sp_t spliced = ::write( get(), buffer.get(), bytes );

	if ( spliced == -1 )
		throw error::io( "file: " ) <<
			"Unable to splice " << bytes << " bytes: " << error::number( errno );

	return spliced;
}

OOE_NAMESPACE_END( ( ooe ) )
