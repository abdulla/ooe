/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_IPC_TRAITS_HPP
	#define OOE_FOUNDATION_IPC_TRAITS_HPP

#include "foundation/ipc/traits_forward.hpp"
#include "foundation/utility/tuple.hpp"

namespace ooe
{
	namespace ipc
	{
//--- ipc::size ----------------------------------------------------------------
		template< typename t >
			struct size< t, typename enable_if< is_stdcontainer< t > >::type >;

		template< BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT( OOE_PP_LIMIT, typename t, no_t ) >
			struct stream_size;

//--- ipc::read ----------------------------------------------------------------
		template< typename t >
			struct read< t, typename enable_if< is_stdcontainer< t > >::type >;

		template< BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT( OOE_PP_LIMIT, typename t, no_t ) >
			struct stream_read;

//--- ipc::write ---------------------------------------------------------------
		template< typename t >
			struct write< t, typename enable_if< is_stdcontainer< t > >::type >;

		template< BOOST_PP_ENUM_PARAMS_WITH_A_DEFAULT( OOE_PP_LIMIT, typename t, no_t ) >
			struct stream_write;
	}

//--- ipc::traits: container ---------------------------------------------------
	template< typename t >
		struct ipc::size< t, typename enable_if< is_stdcontainer< t > >::type >
	{
		static up_t call( typename call_traits< t >::param_type value ) OOE_PURE
		{
			typedef typename no_ref< t >::type type;
			up_t sum = sizeof( up_t );

			for ( typename type::const_iterator i = value.begin(), end = value.end();
				i != end; ++i )
				sum += size< typename type::value_type >::call( *i );

			return sum;
		}
	};

	template< typename t >
		struct ipc::read< t, typename enable_if< is_stdcontainer< t > >::type >
	{
		static up_t call( const u8* buffer, typename call_traits< t >::reference value )
		{
			typedef typename no_ref< t >::type type;
			up_t size = *reinterpret_cast< const up_t* >( buffer );
			type container;
			reserve( container, size );
			const u8* pointer = buffer + sizeof( up_t );

			for ( up_t i = 0; i != size; ++i )
			{
				typename type::value_type element;
				pointer += read< typename type::value_type >::call( pointer, element );
				container.push_back( element );
			}

			value.swap( container );
			return pointer - buffer;
		}
	};

	template< typename t >
		struct ipc::write< t, typename enable_if< is_stdcontainer< t > >::type >
	{
		static up_t call( u8* buffer, typename call_traits< t >::param_type value )
		{
			typedef typename no_ref< t >::type type;
			*reinterpret_cast< up_t* >( buffer ) = value.size();
			u8* pointer = buffer + sizeof( up_t );

			for ( typename type::const_iterator i = value.begin(), end = value.end();
				i != end; ++i )
				pointer += write< typename type::value_type >::call( pointer, *i );

			return pointer - buffer;
		}
	};
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/ipc/traits.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_IPC_TRAITS_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

	#define SIZE( z, n, _ ) + size< t ## n >::call( a ## n )
	#define READ( z, n, _ ) buffer +=\
		read< typename no_ref< t ## n >::type >::call( buffer, a ## n );
	#define WRITE( z, n, _ ) buffer += write< t ## n >::call( buffer, a ## n );

	#define TUPLE_SIZE( z, n, d ) +\
		size< typename tuple_element< n, t >::type >::call( value._ ## n )
	#define TUPLE_READ( z, n, _ ) pointer +=\
		read< typename tuple_element< n, t >::type >::call( pointer, at< n >( value ) );
	#define TUPLE_WRITE( z, n, _ ) pointer +=\
		write< typename tuple_element< n, t >::type >::call( pointer, at< n >( value ) );

namespace ooe
{
	namespace ipc
	{
//--- ipc::size ----------------------------------------------------------------
		template< typename t >
			struct size< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >;

		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
			struct stream_size;
#else
			struct stream_size< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >;
#endif

//--- ipc::read ----------------------------------------------------------------
		template< typename t >
			struct read< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >;

		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
			struct stream_read;
#else
			struct stream_read< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >;
#endif

//--- ipc::write ---------------------------------------------------------------
		template< typename t >
			struct write< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >;

		template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
			struct stream_write;
#else
			struct stream_write< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >;
#endif
	}

//--- ipc::traits: tuple -------------------------------------------------------
	template< typename t >
		struct ipc::size< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
	{
		static up_t call( typename call_traits< t >::param_type BOOST_PP_EXPR_IF( LIMIT, value ) )
			OOE_PURE
		{
			return 0 BOOST_PP_REPEAT( LIMIT, TUPLE_SIZE, ~ );
		}
	};

	template< typename t >
		struct ipc::read< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
	{
		static up_t call( const u8* buffer,
			typename call_traits< t >::reference BOOST_PP_EXPR_IF( LIMIT, value ) )
		{
			const u8* pointer = buffer;
			BOOST_PP_REPEAT( LIMIT, TUPLE_READ, ~ )
			return pointer - buffer;
		}
	};

	template< typename t >
		struct ipc::write< t, typename enable_if_c< tuple_size< t >::value == LIMIT >::type >
	{
		static up_t call( u8* buffer,
			typename call_traits< t >::param_type BOOST_PP_EXPR_IF( LIMIT, value ) )
		{
			u8* pointer = buffer;
			BOOST_PP_REPEAT( LIMIT, TUPLE_WRITE, ~ )
			return pointer - buffer;
		}
	};

//--- ipc::stream_size ---------------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
		struct ipc::stream_size
#else
		struct ipc::stream_size< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >
#endif
	{
		static up_t
			call( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, typename call_traits< t, >::param_type a ) )
			OOE_PURE
		{
			return 0 BOOST_PP_REPEAT( LIMIT, SIZE, ~ );
		}
	};

//--- ipc::stream_read ---------------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
		struct ipc::stream_read
#else
		struct ipc::stream_read< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >
#endif
	{
		static void call( const u8* BOOST_PP_EXPR_IF( LIMIT, buffer )
			BOOST_PP_ENUM_TRAILING_BINARY_PARAMS( LIMIT, typename no_ref< t, >::type& a ) )
		{
			BOOST_PP_REPEAT( LIMIT, READ, ~ )
		}
	};

//--- ipc::stream_write --------------------------------------------------------
	template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
#if LIMIT == OOE_PP_LIMIT
		struct ipc::stream_write
#else
		struct ipc::stream_write< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >
#endif
	{
		static void call( u8* BOOST_PP_EXPR_IF( LIMIT, buffer ) BOOST_PP_ENUM_TRAILING_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a ) )
		{
			BOOST_PP_REPEAT( LIMIT, WRITE, ~ )
		}
	};
}

	#undef TUPLE_WRITE
	#undef TUPLE_READ
	#undef WRITE
	#undef READ
	#undef SIZE
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
