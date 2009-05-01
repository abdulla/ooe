/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_IPC_SWITCHBOARD_HPP
	#define OOE_FOUNDATION_IPC_SWITCHBOARD_HPP

#include <vector>

#include "foundation/ipc/pool.hpp"

namespace ooe
{
	namespace ipc
	{
		class switchboard;

		template< typename >
			struct invoke_function;

		template< typename, typename >
			struct invoke_member;
	}

//--- ipc::switchboard ---------------------------------------------------------
	class ipc::switchboard
	{
	public:
		typedef void ( * call_type )( any, const buffer_unpack&, buffer_pack&, pool& );

		switchboard( void ) OOE_VISIBLE;

		void execute( u8*, up_t, memory_type&, pool& ) const;
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
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/ipc/switchboard.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_IPC_SWITCHBOARD_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()
	#define COMMA BOOST_PP_COMMA_IF( BOOST_PP_GREATER( LIMIT, 1 ) )
	#define ARGUMENT( z, n, _ ) typename no_ref< t ## n >::type a ## n;
	#define VERIFY( z, n, _ ) verify< t ## n >::call( pool, a ## n, n + 1 );

namespace ooe
{
	namespace ipc
	{
		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
			struct invoke_function< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

		template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			struct invoke_function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

#if LIMIT
		template< typename t0 COMMA BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, typename t ) >
			struct invoke_member< t0, void ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >;

		template< typename r, typename t0 COMMA BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, typename t ) >
			struct invoke_member< t0, r ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >;
#endif
	}

//--- ipc::invoke_function -----------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
		struct ipc::invoke_function< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static void call( any any, const buffer_unpack& buffer_unpack, buffer_pack&,
			pool& BOOST_PP_EXPR_IF( LIMIT, pool ) )
		{
			typedef void ( * function_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			function_type function = reinterpret_cast< function_type >( any.function );

			BOOST_PP_REPEAT( LIMIT, ARGUMENT, ~ )
			layout_unpack< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >::
				call( buffer_unpack BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );

			BOOST_PP_REPEAT( LIMIT, VERIFY, ~ )
			function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
		}
	};

	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct ipc::invoke_function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static void call( any any, const buffer_unpack& buffer_unpack, buffer_pack& buffer_pack,
			pool& pool )
		{
			typedef r ( * function_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			function_type function = reinterpret_cast< function_type >( any.function );

			BOOST_PP_REPEAT( LIMIT, ARGUMENT, ~ )
			layout_unpack< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >::
				call( buffer_unpack BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );

			BOOST_PP_REPEAT( LIMIT, VERIFY, ~ )
			r value = function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
			verify< r >::call( pool, value, 0 );
			layout_pack< r >::call( buffer_pack, value );
		}
	};

#if LIMIT
//--- ipc::invoke_member -------------------------------------------------------
	template< typename t0 COMMA BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, typename t ) >
		struct ipc::invoke_member< t0, void ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >
	{
		static void call( any any, const buffer_unpack& buffer_unpack, buffer_pack&,
			pool& pool )
		{
			typedef void ( t0::* member_type )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) );
			member_type member = reinterpret_cast< member_type >( any.member );

			t0* a0;
			BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, ARGUMENT, ~ )
			layout_unpack< t0* COMMA BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) >::
				call( buffer_unpack BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );

			verify< t0* >::call( pool, a0, 1 );
			BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, VERIFY, ~ )
			( a0->*member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, a ) );
		}
	};

	template< typename r, typename t0 COMMA BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, typename t ) >
		struct ipc::invoke_member< t0, r ( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) ) >
	{
		static void call( any any, const buffer_unpack& buffer_unpack, buffer_pack& buffer_pack,
			pool& pool )
		{
			typedef r ( t0::* member_type )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) );
			member_type member = reinterpret_cast< member_type >( any.member );

			t0* a0;
			BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, ARGUMENT, ~ )
			layout_unpack< t0* COMMA BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, t ) >::
				call( buffer_unpack BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );

			verify< t0* >::call( pool, a0, 1 );
			BOOST_PP_REPEAT_FROM_TO( 1, LIMIT, VERIFY, ~ )
			r value = ( a0->*member )( BOOST_PP_ENUM_SHIFTED_PARAMS( LIMIT, a ) );
			verify< r >::call( pool, value, 0 );
			layout_pack< r >::call( buffer_pack, value );
		}
	};
#endif
}

	#undef VERIFY
	#undef ARGUMENT
	#undef COMMA
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
