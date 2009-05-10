/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_BUFFER_HPP
#define OOE_FOUNDATION_IPC_MEMORY_BUFFER_HPP

#include "foundation/ipc/memory/name.hpp"
#include "foundation/ipc/memory/shared_memory.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
	namespace ipc
	{
		typedef tuple< u8*, up_t > buffer_tuple;
		class buffer_base;

		template< typename >
			class buffer;

		template<>
			class buffer< u8 >;

		template<>
			class buffer< shared_memory >;
	}

//--- ipc::buffer_base ---------------------------------------------------------
	class ipc::buffer_base
		: private noncopyable
	{
	protected:
		bool internal;

		buffer_base( up_t internal_size, up_t requested_size )
			: internal( internal_size >= requested_size )
		{
		}

		buffer_base( bool internal_ )
			: internal( internal_ )
		{
		}
	};

//--- ipc::buffer --------------------------------------------------------------
	template<>
		class ipc::buffer< u8 >
		: private buffer_base
	{
	public:
		buffer( buffer_tuple tuple, up_t size )
			: buffer_base( tuple._1, size ), pointer( internal ? tuple._0 : new u8[ size ] )
		{
		}

		~buffer( void )
		{
			if ( !internal )
				delete[] pointer;
		}

		u8* get( void ) const
		{
			return pointer;
		}

	private:
		u8* pointer;
	};

	template<>
		class ipc::buffer< ipc::shared_memory >
		: private buffer_base
	{
	public:
		buffer( buffer_tuple tuple, up_t size )
			: buffer_base( tuple._1, size ), pointer()
		{
			if ( internal )
				pointer = tuple._0;
			else
				pointer = new shared_memory( unique_name(), shared_memory::create, size );
		}

		buffer( const std::string& name_, void* data )
			: buffer_base( name_.empty() ), pointer()
		{
			if ( internal )
				pointer = data;
			else
				pointer = new shared_memory( name_ );
		}

		~buffer( void )
		{
			if ( !internal )
				delete static_cast< shared_memory* >( pointer );
		}

		void swap( buffer& in )
		{
			bool internal_ = internal;
			void* pointer_ = pointer;
			internal = in.internal;
			pointer = in.pointer;
			in.internal = internal_;
			in.pointer = pointer_;
		}

		u8* get( void ) const
		{
			return internal ? static_cast< u8* >( pointer ) :
				static_cast< shared_memory* >( pointer )->as< u8 >();
		}

		std::string name( void ) const
		{
			return static_cast< shared_memory* >( pointer )->name();
		}

		bool is_internal( void ) const
		{
			return internal;
		}

	private:
		void* pointer;
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_BUFFER_HPP
