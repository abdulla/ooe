/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_WRITE_BUFFER_HPP
#define OOE_FOUNDATION_IPC_MEMORY_WRITE_BUFFER_HPP

#include "foundation/ipc/name.hpp"
#include "foundation/ipc/shared_memory.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- write_buffer ---------------------------------------------------------------------------------
class write_buffer
{
public:
	write_buffer( u8* buffer_ptr, up_t buffer_size, up_t size )
		: internal( buffer_size >= size )
	{
		if ( internal )
			unshared = buffer_ptr;
		else
			shared = new shared_memory( unique_name(), shared_memory::create, size );
	}

	write_buffer( const std::string& name_, u8* buffer_ptr )
		: internal( name_.empty() )
	{
		if ( internal )
			unshared = buffer_ptr;
		else
			shared = new shared_memory( name_ );
	}

	~write_buffer( void )
	{
		if ( !internal )
			delete shared;
	}

	void swap( write_buffer& in )
	{
		bool internal_ = internal;
		u8* unshared_ = unshared;
		internal = in.internal;
		unshared = in.unshared;
		in.internal = internal_;
		in.unshared = unshared_;
	}

	u8* get( void ) const
	{
		return internal ? unshared : shared->as< u8 >();
	}

	std::string name( void ) const
	{
		return shared->name();
	}

	bool is_internal( void ) const
	{
		return internal;
	}

private:
	bool internal;

	union
	{
		u8* unshared;
		shared_memory* shared;
	};
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif	// OOE_FOUNDATION_IPC_MEMORY_WRITE_BUFFER_HPP
