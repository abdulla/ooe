/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_HEADER_HPP
#define OOE_FOUNDATION_IPC_MEMORY_HEADER_HPP

#include "foundation/ipc/traits.hpp"
#include "foundation/ipc/memory/write_buffer.hpp"

namespace ooe
{
	namespace ipc
	{
		namespace memory
		{
			inline buffer_tuple header_adjust( const buffer_tuple& );
			inline void header_write( u8*, const write_buffer& );
			inline std::string header_read( const u8* );
		}
	}

//--- ipc::memory --------------------------------------------------------------
	inline ipc::memory::buffer_tuple ipc::memory::header_adjust( const buffer_tuple& tuple )
	{
		return buffer_tuple( tuple._0 + sizeof( u32 ), tuple._1 - sizeof( u32 ) );
	}

	inline void ipc::memory::header_write( u8* data, const write_buffer& buffer )
	{
		bool is_internal = buffer.is_internal();
		*reinterpret_cast< u32* >( data - sizeof( u32 ) ) = is_internal;

		if ( is_internal )
			return;

		stream_write< std::string >::call( data, buffer.name() );
	}

	inline std::string ipc::memory::header_read( const u8* data )
	{
		bool is_internal = *reinterpret_cast< const u32* >( data - sizeof( u32 ) );

		if ( is_internal )
			return std::string();

		std::string name;
		stream_read< std::string >::call( data, name );
		return name;
	}
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_HEADER_HPP
