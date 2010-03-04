/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_ERROR_HPP
#define OOE_FOUNDATION_IPC_ERROR_HPP

#include "foundation/ipc/error_forward.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( error ) )

enum ipc
{
	none,
	exception,
	link
};

//--- rpc ------------------------------------------------------------------------------------------
struct OOE_VISIBLE rpc
	: virtual public runtime
{
	const bool executed;

	rpc( bool executed_ )
		: runtime( "ipc: " ), executed( executed_ )
	{
		*this << "Executed: " << ( executed ? "true" : "false" ) << '\n';
	}

	virtual ~rpc( void ) throw()
	{
	}
};

//--- verification ---------------------------------------------------------------------------------
struct OOE_VISIBLE verification
	: virtual public runtime
{
	verification( const void* value, u8 index )
		: runtime( "ipc: " )
	{
		using ooe::operator <<;
		*this << "Invalid pointer " << ptr( value ) << " in argument " << index;
	}

	virtual ~verification( void ) throw()
	{
	}
};

//--- connection -----------------------------------------------------------------------------------
struct OOE_VISIBLE connection
	: virtual public runtime
{
	connection( void )
		: runtime( "ipc: " )
	{
		*this << "Connection down";
	}

	virtual ~connection( void ) throw()
	{
	}
};

OOE_NAMESPACE_END( ( ooe )( error ) )

#endif	// OOE_FOUNDATION_IPC_ERROR_HPP
