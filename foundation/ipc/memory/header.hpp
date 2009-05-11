#ifndef OOE_FOUNDATION_IPC_MEMORY_HEADER_HPP
#define OOE_FOUNDATION_IPC_MEMORY_HEADER_HPP

#include "foundation/ipc/traits.hpp"
#include "foundation/ipc/memory/buffer.hpp"

namespace ooe
{
	namespace ipc
	{
		typedef tuple< u8*, up_t > buffer_tuple;
		typedef buffer< shared_memory > buffer_type;

		inline buffer_tuple header_adjust( const buffer_tuple& );
		inline void header_write( u8*, const buffer_type& );
		inline std::string header_read( const u8* );
	}

//--- ipc ----------------------------------------------------------------------
	inline ipc::buffer_tuple ipc::header_adjust( const buffer_tuple& tuple )
	{
		up_t size = stream_size< u32 >::call( 0 );
		return buffer_tuple( tuple._0 + size, tuple._1 - size );
	}

	inline void ipc::header_write( u8* data, const buffer_type& buffer )
	{
		up_t size = stream_size< u32 >::call( 0 );
		u32 is_internal = buffer.is_internal();
		stream_write< u32 >::call( data - size, is_internal );

		if ( is_internal )
			return;

		stream_write< std::string >::call( data, buffer.name() );
	}

	inline std::string ipc::header_read( const u8* data )
	{
		up_t size = stream_size< u32 >::call( 0 );
		u32 is_internal;
		stream_read< u32 >::call( data - size, is_internal );

		if ( is_internal )
			return std::string();

		std::string name;
		stream_read< std::string >::call( data, name );
		return name;
	}
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_HEADER_HPP
