/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_IPC_SOCKET_SWITCHBOARD_HPP
	#define OOE_FOUNDATION_IPC_SOCKET_SWITCHBOARD_HPP

#include <vector>

#include "foundation/io/socket.hpp"
#include "foundation/ipc/pool.hpp"
#include "foundation/ipc/socket/write_buffer.hpp"

namespace ooe
{
	namespace ipc
	{
		namespace socket
		{
			class switchboard;

			template< typename >
				struct invoke_function;

			template< typename, typename >
				struct invoke_member;

			inline void return_write( const buffer_tuple&, ooe::socket& );

			template< typename r >
				void return_write( const buffer_tuple&, ooe::socket&,
				typename call_traits< r >::param_type );
		}
	}

//--- ipc::socket::switchboard -------------------------------------------------
	class ipc::socket::switchboard
	{
	public:
		typedef void ( * call_type )
			( const any&, const u8*, const buffer_tuple&, ooe::socket&, pool& );

		switchboard( void ) OOE_VISIBLE;

		void execute( const u8*, ooe::socket&, pool& ) const;
		u32 insert_direct( call_type, any ) OOE_VISIBLE;

		template< typename type >
			u32 insert( type function,
			typename enable_if< is_function_pointer< type > >::type* = 0 )
		{
			typedef typename remove_pointer< type >::type function_type;
			return insert_direct( invoke_function< function_type >::call, function );
		}

		template< typename type >
			u32 insert( type member,
			typename enable_if< is_member_function_pointer< type > >::type* = 0 )
		{
			typedef typename member_of< type >::type object_type;
			typedef typename remove_member< type >::type member_type;
			return insert_direct( invoke_member< object_type, member_type >::call, member );
		}

	private:
		typedef tuple< call_type, any > vector_tuple;
		typedef std::vector< vector_tuple > vector_type;

		vector_type vector;
	};

//--- ipc::socket --------------------------------------------------------------
	inline void ipc::socket::return_write( const buffer_tuple& tuple, ooe::socket& socket )
	{
		stream_write< u32 >::call( tuple._0, 0 );

		if ( socket.send( tuple._0, sizeof( u32 ) ) != sizeof( u32 ) )
			throw error::runtime( "ipc::socket::switchboard: " ) << "Unable to write data";
	}

	template< typename r >
		void ipc::socket::return_write( const buffer_tuple& tuple, ooe::socket& socket,
		typename call_traits< r >::param_type value )
	{
		up_t size = stream_size< r >::call( value );
		up_t total = size + sizeof( u32 );
		write_buffer buffer( tuple, total );
		u8* data = buffer.get();
		stream_write< u32, r >::call( data, size, value );

		if ( socket.send( data, total ) != total )
			throw error::runtime( "ipc::socket::switchboard: " ) << "Unable to write data";
	}
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/ipc/socket/switchboard.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_IPC_SOCKET_SWITCHBOARD_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()
	#define COMMA BOOST_PP_COMMA_IF( BOOST_PP_GREATER( LIMIT, 1 ) )
	#define ARGUMENT( z, n, _ ) typename no_ref< t ## n >::type a ## n;
	#define VERIFY( z, n, _ ) verify< t ## n >::call( pool, a ## n, n + 1 );

namespace ooe
{
	namespace ipc
	{
		namespace socket
		{
			template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
				struct invoke_function< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

			template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
				struct invoke_function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

#if LIMIT
			template< typename t0 COMMA BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, typename t ) >
				struct invoke_member< t0, void ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >;

			template< typename r,
				typename t0 COMMA BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, typename t ) >
				struct invoke_member< t0, r ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >;
#endif
		}
	}

//--- ipc::socket::invoke_function ---------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		struct ipc::socket::invoke_function< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static void call( const any& any, const u8* data, const buffer_tuple& tuple,
			ooe::socket& socket, pool& BOOST_PP_EXPR_IF( LIMIT, pool ) )
		{
			typedef void ( * function_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			function_type function = reinterpret_cast< function_type >( any.function );

			BOOST_PP_REPEAT( LIMIT, ARGUMENT, ~ )
			stream_read< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >::
				call( data BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );

			BOOST_PP_REPEAT( LIMIT, VERIFY, ~ );
			function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
			return_write( tuple, socket );
		}
	};

	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct ipc::socket::invoke_function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static void call( const any& any, const u8* data, const buffer_tuple& tuple,
			ooe::socket& socket, pool& pool )
		{
			typedef r ( * function_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			function_type function = reinterpret_cast< function_type >( any.function );

			BOOST_PP_REPEAT( LIMIT, ARGUMENT, ~ )
			stream_read< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >::
				call( data BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );

			BOOST_PP_REPEAT( LIMIT, VERIFY, ~ )
			r value = function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
			verify< r >::call( pool, value, 0 );
			return_write< r >( tuple, socket, value );
		}
	};

#if LIMIT
//--- ipc::socket::invoke_member -----------------------------------------------
	template< typename t0 COMMA BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, typename t ) >
		struct ipc::socket::invoke_member< t0, void ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >
	{
		static void call( const any& any, const u8* data, const buffer_tuple& tuple,
			ooe::socket& socket, pool& pool )
		{
			typedef void ( t0::* member_type )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) );
			member_type member = reinterpret_cast< member_type >( any.member );

			t0* a0;
			BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, ARGUMENT, ~ )
			stream_read< t0* COMMA BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) >::
				call( data BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );

			verify< t0* >::call( pool, a0, 1 );
			BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, VERIFY, ~ )
			( a0->*member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, a ) );
			return_write( tuple, socket );
		}
	};

	template< typename r, typename t0 COMMA BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, typename t ) >
		struct ipc::socket::invoke_member< t0, r ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >
	{
		static void call( const any& any, const u8* data, const buffer_tuple& tuple,
			ooe::socket& socket, pool& pool )
		{
			typedef r ( t0::* member_type )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) );
			member_type member = reinterpret_cast< member_type >( any.member );

			t0* a0;
			BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, ARGUMENT, ~ )
			stream_read< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >::
				call( data BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );

			verify< t0* >::call( pool, a0, 1 );
			BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, VERIFY, ~ )
			r value = ( a0->*member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, a ) );
			verify< r >::call( pool, value, 0 );
			return_write< r >( tuple, socket, value );
		}
	};
#endif
}

	#undef VERIFY
	#undef ARGUMENT
	#undef COMMA
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
