/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_IPC_RPC_FORWARD_HPP
	#define OOE_FOUNDATION_IPC_RPC_FORWARD_HPP

#include "foundation/ipc/error.hpp"
#include "foundation/ipc/traits.hpp"
#include "foundation/ipc/transport.hpp"

namespace ooe
{
	namespace ipc
	{
		class rpc_base;

		template< typename >
			class rpc_operator;

		template< typename >
			struct rpc;

		inline void validate( u32, const buffer_unpack& );
	}

//--- ipc::rpc_base ------------------------------------------------------------
	class ipc::rpc_base
	{
	protected:
		ipc::transport& transport;
		const u32 index;

		rpc_base( ipc::transport& transport_, u32 index_ )
			: transport( transport_ ), index( index_ )
		{
		}
	};

//--- ipc ----------------------------------------------------------------------
	inline void ipc::validate( u32 type, const buffer_unpack& buffer_unpack )
	{
		const c8* what;
		const c8* where;

		switch ( type )
		{
		case error::none:
			return;

		case error::link:
			throw error::connection();

		case error::exception:
			layout_unpack< const c8*, const c8* >::call( buffer_unpack, what, where );
			throw error::rpc() << what << "\n\nServer stack trace:" << where;

		default:
			throw error::rpc() << "Unknown error code";
		}
	}
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/ipc/rpc_forward.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_IPC_RPC_FORWARD_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

namespace ooe
{
	namespace ipc
	{
		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
			struct rpc_operator< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
			struct rpc< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

		template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			struct rpc< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;
	}

//--- ipc::rpc_operator --------------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		class ipc::rpc_operator< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		: public rpc_base
	{
	public:
		buffer_unpack operator ()( BOOST_PP_ENUM_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a ) ) const
		{
			transport::tuple_type tuple = transport.get();
			buffer_pack buffer_pack( tuple._0, tuple._1, sizeof( header_type ) );
			layout_pack< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >::
				call( buffer_pack BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );
			header_type& header = *reinterpret_cast< header_type* >( tuple._0 );
			memory_type memory( 0 );

			header._0 = index;
			buffer_pack.store( header, memory );
			transport.notify();
			buffer_unpack buffer_unpack( tuple._0, sizeof( header_type ), header, memory );
			validate( header._0, buffer_unpack );
			return buffer_unpack;
		}

	protected:
		rpc_operator( ipc::transport& transport_, u32 index_ )
			: rpc_base( transport_, index_ )
		{
		}
	};

//--- ipc::rpc -----------------------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		struct ipc::rpc< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		: public rpc_operator< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		typedef rpc_operator< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) > base_type;
		typedef void result_type;

		rpc( ipc::transport& transport_, u32 index_ )
			: base_type( transport_, index_ )
		{
		}

		result_type operator ()( BOOST_PP_ENUM_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a ) ) const
		{
			base_type::operator ()( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
		}
	};

	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct ipc::rpc< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		: public rpc_operator< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		typedef rpc_operator< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) > base_type;
		typedef typename no_ref< r >::type result_type;

		rpc( ipc::transport& transport_, u32 index_ )
			: base_type( transport_, index_ )
		{
		}

		result_type operator ()( BOOST_PP_ENUM_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a ) ) const
		{
			buffer_unpack buffer_unpack =
				base_type::operator ()( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );

			result_type value;
			layout_unpack< result_type >::call( buffer_unpack, value );
			return value;
		}
	};
}

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
