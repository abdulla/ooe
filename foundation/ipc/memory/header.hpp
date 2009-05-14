#ifndef OOE_FOUNDATION_IPC_MEMORY_HEADER_HPP
#define OOE_FOUNDATION_IPC_MEMORY_HEADER_HPP

#include "foundation/ipc/traits.hpp"
#include "foundation/ipc/memory/write_buffer.hpp"

namespace ooe
{
	namespace ipc
	{
		typedef tuple< u8*, up_t > buffer_tuple;

		inline buffer_tuple header_adjust( const buffer_tuple& );
		inline void header_write( u8*, const write_buffer& );
		inline std::string header_read( const u8* );
	}

//--- ipc ----------------------------------------------------------------------
	inline ipc::buffer_tuple ipc::header_adjust( const buffer_tuple& tuple )
	{
		return buffer_tuple( tuple._0 + sizeof( u32 ), tuple._1 - sizeof( u32 ) );
	}

	inline void ipc::header_write( u8* data, const write_buffer& buffer )
	{
		bool is_internal = buffer.is_internal();
		*reinterpret_cast< u32* >( data - sizeof( u32 ) ) = is_internal;

		if ( is_internal )
			return;

		stream_write< std::string >::call( data, buffer.name() );
	}

	inline std::string ipc::header_read( const u8* data )
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
