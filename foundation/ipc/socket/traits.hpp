/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_IPC_SOCKET_TRAITS_HPP
	#define OOE_FOUNDATION_IPC_SOCKET_TRAITS_HPP

#include "foundation/io/socket.hpp"
#include "foundation/utility/traits.hpp"

#include "external/nipc/traits_forward.hpp"

namespace ooe
{
	namespace nipc
	{
		template< BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT( OOE_PP_LIMIT, typename t, no_t ) >
			struct layout_read;

		template< BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT( OOE_PP_LIMIT, typename t, no_t ) >
			struct layout_write;
	}
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/ipc/socket/traits.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_IPC_SOCKET_TRAITS_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

	#define SIZE( z, n, _ )	u32 s ## n = size< t ## n >::call( a ## n );
	#define ADD( z, n, _ ) + s ## n
	#define WRITE( z, n, _ ) write< t ## n >::call( socket, a ## n, s ## n );

	#define READ( z, n, _ )\
		read< t ## n >::call( buffer, a ## n );\
		BOOST_PP_EXPR_IF( BOOST_PP_NOT_EQUAL( BOOST_PP_INC( n ), LIMIT ),\
		buffer += size< t ## n >::call( a ## n ); )

namespace ooe
{
	namespace nipc
	{
		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
			struct layout_read;
#else
			struct layout_read< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >;
#endif

		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
			struct layout_write;
#else
			struct layout_write< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >;
#endif
	}

//--- nipc::layout_read --------------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
		struct nipc::layout_read
#else
		struct nipc::layout_read< BOOST_PP_ENUM_PARAMS( LIMIT, t )
		BOOST_PP_COMMA_IF( BOOST_PP_MOD( LIMIT, OOE_PP_LIMIT ) )
		BOOST_PP_ENUM_PARAMS( BOOST_PP_SUB( OOE_PP_LIMIT, LIMIT ), no_t BOOST_PP_INTERCEPT ) >
#endif
	{
		static void call( const u8* BOOST_PP_EXPR_IF( LIMIT, buffer )
			BOOST_PP_ENUM_TRAILING_BINARY_PARAMS( LIMIT, typename no_ref< t, >::type& a ) )
		{
			BOOST_PP_REPEAT( LIMIT, READ, ~ )
		}
	};

//--- nipc::layout_write -------------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
		struct nipc::layout_write
#else
		struct nipc::layout_write< BOOST_PP_ENUM_PARAMS( LIMIT, t )
		BOOST_PP_COMMA_IF( BOOST_PP_MOD( LIMIT, OOE_PP_LIMIT ) )
		BOOST_PP_ENUM_PARAMS( BOOST_PP_SUB( OOE_PP_LIMIT, LIMIT ), no_t BOOST_PP_INTERCEPT ) >
#endif
	{
		static void call( socket& socket BOOST_PP_ENUM_TRAILING_BINARY_PARAMS( LIMIT, t, a ) )
		{
			BOOST_PP_REPEAT( LIMIT, SIZE, ~ );

			u32 size = 0 BOOST_PP_REPEAT( LIMIT, ADD, ~ );
			write< u32 >::call( socket, size, sizeof( u32 ) );

			BOOST_PP_REPEAT( LIMIT, WRITE, ~ );
		}
	};
}

	#undef READ
	#undef WRITE
	#undef ADD
	#undef SIZE

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
