/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_IPC_MEMORY_RPC_FORWARD_HPP
	#define OOE_FOUNDATION_IPC_MEMORY_RPC_FORWARD_HPP

#include "foundation/ipc/memory/error.hpp"
#include "foundation/ipc/memory/header.hpp"
#include "foundation/ipc/memory/transport.hpp"

namespace ooe
{
	namespace ipc
	{
		namespace memory
		{
			class rpc_base;

			template< typename >
				struct rpc;


			inline const u8* validate( const u8* );
		}
	}

//--- ipc::memory::rpc_base ----------------------------------------------------
	class ipc::memory::rpc_base
	{
	protected:
		memory::transport& transport;
		const u32 index;

		rpc_base( memory::transport& transport_, u32 index_ )
			: transport( transport_ ), index( index_ )
		{
		}
	};

//--- ipc::memory --------------------------------------------------------------
	inline const u8* ipc::memory::validate( const u8* data )
	{
		u32 type;
		data += read< u32 >::call( data, type );

		switch ( type )
		{
		case error::none:
			return data;

		case error::link:
			throw error::connection();

		case error::exception:
			bool executed;
			const c8* what;
			const c8* where;
			stream_read< bool, const c8*, const c8* >::call( data, executed, what, where );
			throw error::memory_rpc( executed ) << what << "\n\nServer stack trace:" << where;

		default:
			throw error::memory_rpc( false ) << "Unknown error code: " << type;
		}
	}
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/ipc/memory/rpc_forward.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_IPC_MEMORY_RPC_FORWARD_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

namespace ooe
{
#if LIMIT != OOE_PP_LIMIT
	namespace ipc
	{
		namespace memory
		{
			template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
				struct rpc< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

			template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
				struct rpc< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;
		}
	}

//--- ipc::memory::rpc ---------------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		struct ipc::memory::rpc< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		: private rpc_base
	{
		typedef void result_type;

		rpc( memory::transport& transport_, u32 index_ )
			: rpc_base( transport_, index_ )
		{
		}

		result_type operator ()
			( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, typename call_traits< t, >::param_type a ) ) const
		{
			buffer_tuple tuple = header_adjust( transport.get() );

			{
				up_t size = stream_size< u32 BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) >::
					call( index BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
				write_buffer buffer( tuple, size );

				stream_write< u32 BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) >::
					call( buffer.get(), index BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
				header_write( tuple._0, buffer );

				transport.notify();
			}

			{
				write_buffer buffer( header_read( tuple._0 ), tuple._0 );
				validate( buffer.get() );
			}
		}
	};

	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct ipc::memory::rpc< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		: private rpc_base
	{
		typedef typename no_ref< r >::type result_type;

		rpc( memory::transport& transport_, u32 index_ )
			: rpc_base( transport_, index_ )
		{
		}

		result_type operator ()
			( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, typename call_traits< t, >::param_type a ) ) const
		{
			buffer_tuple tuple = header_adjust( transport.get() );

			{
				up_t size = stream_size< u32 BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) >::
					call( index BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
				write_buffer buffer( tuple, size );

				stream_write< u32 BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) >::
					call( buffer.get(), index BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
				header_write( tuple._0, buffer );

				transport.notify();
			}

			{
				write_buffer buffer( header_read( tuple._0 ), tuple._0 );
				const u8* data = validate( buffer.get() );

				result_type value;
				stream_read< result_type >::call( data, value );
				return value;
			}
		}
	};
#endif
}

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
