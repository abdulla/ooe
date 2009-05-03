/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_IPC_SOCKET_RPC_FORWARD_HPP
	#define OOE_FOUNDATION_IPC_SOCKET_RPC_FORWARD_HPP

#include "external/nipc/client.hpp"
#include "external/nipc/error.hpp"
#include "external/nipc/traits.hpp"

namespace ooe
{
	namespace nipc
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

//--- nipc::result -------------------------------------------------------------
	template<>
		struct nipc::result< void >
	{
	public:
		result( nipc::client& client, const nipc::client::map_type::iterator& i )
			: base( new base_type( client, i ) )
		{
		}

		void operator ()( void )
		{
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
		class nipc::result
	{
	public:
		result( nipc::client& client, const nipc::client::map_type::iterator& i )
			: base( new base_type( client, i ) )
		{
		}

		type& operator ()( void ) const
		{
			array_type array;

			if ( base->state == base_type::wait )
				array = base->client.wait( *base );

			if ( base->state == base_type::error )
				except( array );

			layout_read< type >::call( array, base->value );
			return base->value;
		}

	private:
		typedef result_base< type > base_type;
		shared_ptr< base_type > base;
	};

//--- nipc::rpc_base -----------------------------------------------------------
	class nipc::rpc_base
	{
	protected:
		nipc::client& client;
		const u32 index;

		rpc_base( nipc::client& client_, u32 index_ )
			: client( client_ ), index( index_ )
		{
		}
	};

//--- nipc ---------------------------------------------------------------------
	inline void nipc::except( const u8* buffer )
	{
		const c8* what;
		const c8* where;
		layout_read< const c8*, const c8* >::call( buffer, what, where );
		throw error::nrpc() << what << "\n\nServer stack trace:" << where;
	}
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/ipc/socket/rpc_forward.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_IPC_SOCKET_RPC_FORWARD_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

	#if LIMIT != OOE_PP_LIMIT
namespace ooe
{
	namespace nipc
	{
		template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			struct rpc< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;
	}

//--- nipc::rpc ----------------------------------------------------------------
	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct nipc::rpc< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		: private rpc_base
	{
		rpc( nipc::client& client_, u32 index_ )
			: rpc_base( client_, index_ )
		{
		}

		result< r > operator ()( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, t, a ) ) const
		{
			client::map_type::iterator i = client.insert();
			layout_write< u32 BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) >::
				call( client, index BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
			return result< r >( client, i );
		}
	};
}
	#endif

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
