/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <fcntl.h>

#include "foundation/io/descriptor.hpp"
#include "foundation/io/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

//--- platform::descriptor -------------------------------------------------------------------------
up_t descriptor::splice( const ooe::descriptor& desc, up_t size )
{
	ooe::descriptor& self = *static_cast< ooe::descriptor* >( this );
	sp_t spliced = ::splice( self.get(), 0, desc.get(), 0, size, SPLICE_F_MOVE );

	if ( spliced == -1 )
		throw error::io( "descriptor: " ) <<
			"Unable to splice " << size << " bytes: " << error::number( errno );

	return spliced;
}

up_t descriptor::splice( aligned< executable::static_page_size > data, up_t size )
{
	ooe::descriptor& self = *static_cast< ooe::descriptor* >( this );
	iovec vector = { data.get(), size };
	sp_t spliced = vmsplice( self.get(), &vector, 1, SPLICE_F_GIFT );

	if ( spliced == -1 )
		throw error::io( "descriptor: " ) <<
			"Unable to splice " << size << " bytes: " << error::number( errno );

	return spliced;
}

OOE_NAMESPACE_END( ( ooe )( platform ) )
