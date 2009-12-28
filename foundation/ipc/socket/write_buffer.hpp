/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SOCKET_WRITE_BUFFER_HPP
#define OOE_FOUNDATION_IPC_SOCKET_WRITE_BUFFER_HPP

#include "foundation/utility/fundamental.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( socket ) )

//--- write_buffer ---------------------------------------------------------------------------------
class write_buffer
{
public:
	write_buffer( u8* buffer_ptr, up_t buffer_size, up_t size )
		: internal( buffer_size >= size ), pointer( internal ? buffer_ptr : new u8[ size ] )
	{
	}

	~write_buffer( void )
	{
		if ( !internal )
			delete[] pointer;
	}

	u8* get( void ) const
	{
		return pointer;
	}

private:
	bool internal;
	u8* pointer;
};

OOE_NAMESPACE_END( ( ooe )( ipc )( socket ) )

#endif	// OOE_FOUNDATION_IPC_SOCKET_WRITE_BUFFER_HPP
