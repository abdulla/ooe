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
			struct replace< type, typename enable_if< is_jumbo< type > >::type >;

		template< typename type >
			struct pack< type, typename enable_if< is_jumbo< type > >::type >;

		template< typename type >
			struct unpack< type, typename enable_if< is_jumbo< type > >::type >;
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
			u8 buffer[ size ];
			buffer_pack buffer_pack( buffer, size, 0 );
			layout_pack< type >::call( buffer_pack, value );

			header_type header;
			buffer_pack.store( header, memory, size );
			std::copy( buffer, buffer + size, memory->as< u8 >() );
		}

		operator type( void ) const
		{
			if ( !memory )
				throw error::runtime( "ipc::jumbo: " ) << "Jumbo is uninitialised";

			u8* buffer = memory->as< u8 >();
			buffer_unpack buffer_unpack( buffer, buffer + size );
			type value;
			layout_unpack< type >::call( buffer_unpack, value );
			return value;
		}

	private:
		static const up_t size = sizeof( typename layout_replace< type >::type );

		shared_ptr< shared_memory > memory;
	};

//--- ipc::traits: jumbo -------------------------------------------------------
	template< typename t >
		struct ipc::replace< t, typename enable_if< ipc::is_jumbo< t > >::type >
	{
		typedef std::string value_type;
		typedef typename replace< value_type >::type type;
	};

	template< typename t >
		struct ipc::pack< t, typename enable_if< ipc::is_jumbo< t > >::type >
	{
		typedef typename no_ref< t >::type type;
		typedef typename replace< t >::type value_type;

		static void call( const type& in, value_type& out, buffer_pack& buffer_pack )
		{
			typedef typename replace< t >::value_type contained_type;
			pack< contained_type >::call( in.name(), out, buffer_pack );
		}
	};

	template< typename t >
		struct ipc::unpack< t, typename enable_if< ipc::is_jumbo< t > >::type >
	{
		typedef typename no_ref< t >::type type;
		typedef typename replace< t >::type value_type;

		static void call( const value_type& in, type& out, const buffer_unpack& buffer_unpack )
		{
			typedef typename replace< t >::value_type contained_type;
			contained_type name;
			unpack< contained_type >::call( in, name, buffer_unpack );
			out = type( name );
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_JUMBO_HPP
