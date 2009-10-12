/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_IPC_SOCKET_RPC_FORWARD_HPP
	#define OOE_FOUNDATION_IPC_SOCKET_RPC_FORWARD_HPP

#include "foundation/ipc/error.hpp"
#include "foundation/ipc/traits.hpp"
#include "foundation/ipc/socket/client.hpp"
#include "foundation/ipc/socket/write_buffer.hpp"

namespace ooe
{
	namespace ipc
	{
		namespace socket
		{
			template< typename >
				class result;

			template<>
				class result< void >;

			class rpc_base;

			template< typename >
				struct rpc;

			inline void except( const u8* );
		}
	}

//--- ipc::socket --------------------------------------------------------------
	inline void ipc::socket::except( const u8* buffer )
	{
		bool executed;
		const c8* what;
		const c8* where;
		stream_read< bool, const c8*, const c8* >::call( buffer, executed, what, where );
		throw error::rpc( executed ) << what << "\n\nServer stack trace:" << where;
	}

//--- ipc::socket::result ------------------------------------------------------
	template<>
		struct ipc::socket::result< void >
	{
	public:
		result( client& client, const client::iterator& i )
			: base( new base_type( client, i ) )
		{
		}

		void operator ()( void )
		{
			if ( base->state == base_type::done )
				return;

			array_type array;

			if ( base->state == base_type::wait )
				array = base->client.wait( *base );

			if ( base->state == base_type::error )
				except( array );
		}

	private:
		typedef result_base< void > base_type;
		shared_ptr< base_type > base;
	};

	template< typename type >
		class ipc::socket::result
	{
	public:
		result( client& client, const client::iterator& i )
			: base( new base_type( client, i ) )
		{
		}

		type& operator ()( void ) const
		{
			if ( base->state == base_type::done )
				return base->value;

			array_type array;

			if ( base->state == base_type::wait )
				array = base->client.wait( *base );

			if ( base->state == base_type::error )
				except( array );

			stream_read< type >::call( array, base->value );
			return base->value;
		}

	private:
		typedef result_base< type > base_type;
		shared_ptr< base_type > base;
	};

//--- ipc::socket::rpc_base ----------------------------------------------------
	class ipc::socket::rpc_base
	{
	protected:
		socket::client& client;
		const u32 index;

		rpc_base( socket::client& client_, u32 index_ )
			: client( client_ ), index( index_ )
		{
		}
	};
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/ipc/socket/rpc_forward.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_IPC_SOCKET_RPC_FORWARD_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

	#if LIMIT < BOOST_PP_DEC( OOE_PP_LIMIT )
namespace ooe
{
	namespace ipc
	{
		namespace socket
		{
			template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
				struct rpc< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;
		}
	}

//--- ipc::socket::rpc ---------------------------------------------------------
	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct ipc::socket::rpc< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		: private rpc_base
	{
		rpc( socket::client& client_, u32 index_ )
			: rpc_base( client_, index_ )
		{
		}

		result< r > operator ()( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, t, a ) ) const
		{
			up_t size = stream_size< u32 BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) >::
				call( index BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
			write_buffer buffer( client.get(), client.size(), size + sizeof( u32 ) );
			u8* data = buffer.get();

			stream_write< u32, u32 BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) >::
				call( data, size, index BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
			client::iterator i = client.insert();
			client.write( data, size + sizeof( u32 ) );

			return result< r >( client, i );
		}
	};
}
	#endif

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
