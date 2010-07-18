/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <sys/mman.h>

#include "foundation/executable/environment.hpp"
#include "foundation/utility/error.hpp"
#include "test/mock/inject.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( mock ) )

void inject( void* from, void* to )
{
	up_t address = reinterpret_cast< up_t >( from ) & ~( executable::static_page_size - 1 );
	void* pointer = reinterpret_cast< void* >( address );

	if ( mprotect( pointer, executable::static_page_size * 2, PROT_READ | PROT_WRITE | PROT_EXEC ) )
		throw error::runtime( "mock::inject: " ) <<
			"Unable to unprotect address " << ptr( from ) << ": " << error::number( errno );

	struct
	{
		u8 push;
		void* address;
		u8 ret;
	} OOE_PACKED jump =
	{
		0x68,
		to,
		0xc3
	};

	std::memcpy( from, &jump, sizeof( jump ) );
}

OOE_NAMESPACE_END( ( ooe )( mock ) )
