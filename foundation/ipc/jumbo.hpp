/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_JUMBO_HPP
#define OOE_FOUNDATION_IPC_MEMORY_JUMBO_HPP

#include "foundation/ipc/name.hpp"
#include "foundation/ipc/shared_memory.hpp"
#include "foundation/ipc/traits.hpp"

namespace ooe
{
	namespace ipc
	{
		template< typename >
			struct is_jumbo;

		template< typename >
			class jumbo;

		template< typename type >
			struct size< type, typename enable_if< is_jumbo< type > >::type >;

		template< typename type >
			struct read< type, typename enable_if< is_jumbo< type > >::type >;

		template< typename type >
			struct write< type, typename enable_if< is_jumbo< type > >::type >;
	}

//--- ipc::is_jumbo ------------------------------------------------------------
	template< typename type >
		struct ipc::is_jumbo
	{
		template< typename >
			struct apply
			: public false_type
		{
		};

		template< typename t >
			struct apply< jumbo< t > >
			: public true_type
		{
		};

		static const bool value = apply< typename no_ref< type >::type >::value;
	};

//--- ipc::jumbo ---------------------------------------------------------------
	template< typename type >
		class ipc::jumbo
	{
	public:
		jumbo( void )
			: memory()
		{
		}

		jumbo( const std::string& name_ )
			: memory( new shared_memory( name_ ) )
		{
		}

		std::string name( void ) const
		{
			if ( !memory )
				throw error::runtime( "ipc::jumbo: " ) << "Jumbo is uninitialised";

			return memory->name();
		}

		void operator ()( const type& value )
		{
			up_t size = stream_size< type >::call( value );
			memory = new shared_memory( unique_name(), shared_memory::create, size );
			stream_write< type >::call( memory->as< u8 >(), value );
		}

		operator type( void ) const
		{
			if ( !memory )
				throw error::runtime( "ipc::jumbo: " ) << "Jumbo is uninitialised";

			type value;
			stream_read< type >::call( memory->as< u8 >(), value );
			return value;
		}

	private:
		shared_ptr< shared_memory > memory;
	};

//--- ipc::traits: jumbo -------------------------------------------------------
	template< typename t >
		struct ipc::size< t, typename enable_if< ipc::is_jumbo< t > >::type >
	{
		static up_t call( typename call_traits< t >::param_type value )
		{
			return size< std::string >::call( value.name() );
		}
	};

	template< typename t >
		struct ipc::read< t, typename enable_if< ipc::is_jumbo< t > >::type >
	{
		static up_t call( const u8* buffer, typename call_traits< t >::reference value )
		{
			std::string name;
			up_t size = read< std::string >::call( buffer, name );

			value = typename no_ref< t >::type( name );

			return size;
		}
	};

	template< typename t >
		struct ipc::write< t, typename enable_if< ipc::is_jumbo< t > >::type >
	{
		static up_t call( u8* buffer, typename call_traits< t >::param_type value )
		{
			return write< std::string >::call( buffer, value.name() );
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_JUMBO_HPP
