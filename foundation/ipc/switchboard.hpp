/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_IPC_SWITCHBOARD_HPP
	#define OOE_FOUNDATION_IPC_SWITCHBOARD_HPP

#include <vector>

#include "foundation/ipc/io_buffer.hpp"
#include "foundation/ipc/pool.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

template< typename >
	struct invoke_function;

template< typename, typename >
	struct invoke_member;

//--- switchboard ----------------------------------------------------------------------------------
class switchboard
{
public:
	typedef up_t ( * call_type )( const any&, io_buffer&, pool& );
	typedef tuple< up_t/* result size */, up_t/* error size */ > size_type;

	switchboard( void ) OOE_VISIBLE;

	size_type execute( index_t, io_buffer&, pool& ) const;
	index_t insert_direct( call_type, any ) OOE_VISIBLE;

	template< typename t >
		index_t insert( t function, typename enable_if< is_function_pointer< t > >::type* = 0 )
	{
		typedef typename remove_pointer< t >::type function_type;
		return insert_direct( invoke_function< function_type >::call, function );
	}

	template< typename t >
		index_t insert( t member, typename enable_if< is_member_function_pointer< t > >::type* = 0 )
	{
		typedef typename member_of< t >::type type;
		typedef typename remove_member< t >::type member_type;
		return insert_direct( invoke_member< type, member_type >::call, member );
	}

private:
	typedef tuple< call_type, any > vector_tuple;
	typedef std::vector< vector_tuple > vector_type;

	vector_type vector;
};

//--- return_write ---------------------------------------------------------------------------------
template< typename r >
	up_t return_write( io_buffer& buffer, pool& pool, typename call_traits< r >::param_type value )
{
	verify< r >::call( pool, value, 0 );
	up_t size = stream_size< r >::call( value );
	buffer.allocate( size );

	if ( OOE_UNLIKELY( !buffer.is_internal() ) )
		buffer.preserve( 0 );

	stream_write< r >::call( buffer.get(), value );
	return size;
}

OOE_NAMESPACE_END( ( ooe )( ipc ) )

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/ipc/switchboard.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_IPC_SWITCHBOARD_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()
	#define ARGUMENT( z, n, _ ) typename no_ref< t ## n >::type a ## n;
	#define VERIFY( z, n, x ) verify< t ## n >::call( pool, a ## n, n + x + 1 );

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

//--- invoke_function ------------------------------------------------------------------------------
template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
	struct invoke_function< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
	static up_t call( const any& any, io_buffer& buffer, pool& BOOST_PP_EXPR_IF( LIMIT, pool ) )
	{
		typedef void ( * function_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
		function_type function = reinterpret_cast< function_type >( any.function );

		BOOST_PP_REPEAT( LIMIT, ARGUMENT, ~ )
		stream_read< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >::
			call( buffer.get() BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );

		BOOST_PP_REPEAT( LIMIT, VERIFY, 0 )
		function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
		return 0;
	}
};

template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
	struct invoke_function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
	static up_t call( const any& any, io_buffer& buffer, pool& pool )
	{
		typedef r ( * function_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
		function_type function = reinterpret_cast< function_type >( any.function );

		BOOST_PP_REPEAT( LIMIT, ARGUMENT, ~ )
		stream_read< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >::
			call( buffer.get() BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );

		BOOST_PP_REPEAT( LIMIT, VERIFY, 0 )
		r value = function( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
		return return_write< r >( buffer, pool, value );
	}
};

#if LIMIT != OOE_PP_LIMIT
//--- invoke_member --------------------------------------------------------------------------------
template< typename s BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
	struct invoke_member< s, void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
	static up_t call( const any& any, io_buffer& buffer, pool& pool )
	{
		typedef void ( s::* member_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
		member_type member = reinterpret_cast< member_type >( any.member );

		s* self;
		BOOST_PP_REPEAT( LIMIT, ARGUMENT, ~ )
		stream_read< s* BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) >::
			call( buffer.get(), self BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );

		verify< s* >::call( pool, self, 1 );
		BOOST_PP_REPEAT( LIMIT, VERIFY, 1 )
		( self->*member )( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
		return 0;
	}
};

template< typename r, typename s BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
	struct invoke_member< s, r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
	static up_t call( const any& any, io_buffer& buffer, pool& pool )
	{
		typedef r ( s::* member_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
		member_type member = reinterpret_cast< member_type >( any.member );

		s* self;
		BOOST_PP_REPEAT( LIMIT, ARGUMENT, ~ )
		stream_read< s* BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, t ) >::
			call( buffer.get(), self BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );

		verify< s* >::call( pool, self, 1 );
		BOOST_PP_REPEAT( LIMIT, VERIFY, 1 )
		r value = ( self->*member )( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
		return return_write< r >( buffer, pool, value );
	}
};
#endif

OOE_NAMESPACE_END( ( ooe )( ipc ) )

	#undef VERIFY
	#undef ARGUMENT
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
