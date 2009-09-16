/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_BUFFER_HPP
#define OOE_FOUNDATION_IPC_MEMORY_BUFFER_HPP

#include "foundation/ipc/name.hpp"
#include "foundation/ipc/shared_memory.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
	namespace ipc
	{
		namespace memory
		{
			typedef tuple< u8*, up_t > buffer_tuple;
			class write_buffer;
		}
	}

//--- ipc::memory::write_buffer ------------------------------------------------
	class ipc::memory::write_buffer
	{
	public:
		write_buffer( buffer_tuple tuple, up_t size )
			: internal( tuple._1 >= size )
		{
			if ( internal )
				unshared = tuple._0;
			else
				shared = new locked_memory( unique_name(), locked_memory::create, size );
		}

		write_buffer( const std::string& name_, u8* data )
			: internal( name_.empty() )
		{
			if ( internal )
				unshared = data;
			else
				shared = new locked_memory( name_ );
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
			locked_memory* shared;
		};
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_BUFFER_HPP
