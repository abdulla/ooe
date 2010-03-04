/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/ipc/memory/transport.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- transport ------------------------------------------------------------------------------------
void* transport::private_data( void ) const
{
	return memory.get();
}

std::string transport::name( void ) const
{
	return memory.name();
}

bool transport::in_canary( const void* pointer ) const
{
	const u8* begin = memory.as< u8 >() + executable::static_page_size;
	const u8* end = begin + executable::static_page_size;
	return pointer >= begin && pointer < end;
}

void transport::unlink( void )
{
	memory.unlink();
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
