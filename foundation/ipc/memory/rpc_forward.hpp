/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_IPC_MEMORY_RPC_FORWARD_HPP
	#define OOE_FOUNDATION_IPC_MEMORY_RPC_FORWARD_HPP

#include "foundation/ipc/error.hpp"
#include "foundation/ipc/io_buffer.hpp"
#include "foundation/ipc/traits.hpp"
#include "foundation/ipc/memory/transport.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

template< typename >
	struct rpc;

//--- rpc_base -------------------------------------------------------------------------------------
class rpc_base
{
protected:
	memory::transport& transport;
	const index_t index;

	rpc_base( memory::transport& transport_, index_t index_ )
		: transport( transport_ ), index( index_ )
	{
	}
};

//--- validate -------------------------------------------------------------------------------------
inline void validate( error_t error, const u8* data )
{
	switch ( error )
	{
	case error::none:
		return;

	case error::exception:
		bool executed;
		const c8* what;
		const c8* where;
		stream_read< bool, const c8*, const c8* >::call( data, executed, what, where );
		throw error::rpc( executed ) << what << "\n\nServer stack trace:" << where;

	case error::link:
		throw error::connection();

	default:
		throw error::rpc( false ) << "Unknown error: " << error;
	}
}

//--- store_header ---------------------------------------------------------------------------------
inline void store_header( shared_allocator& allocator, io_buffer& buffer, up_t size, index_t index )
{
	u8* data = buffer.get();
	up_t preserve = stream_size< bool_t, index_t >::call( true, index );
	buffer.preserve( preserve );

	buffer.allocate( size );
	bool_t internal = buffer.is_internal();

	if ( OOE_LIKELY( internal ) )
		stream_write< bool_t, index_t >::call( data, internal, index );
	else
	{
		stream_write< bool_t, index_t, std::string >::
			call( data, internal, index, allocator.name() );
		buffer.preserve( 0 );
	}
}

//--- load_header ----------------------------------------------------------------------------------
inline void load_header( shared_allocator& allocator, io_buffer& buffer )
{
	buffer.reset();

	bool_t internal;
	error_t error;
	const c8* name;
	stream_read< bool_t, error_t, const c8* >::call( buffer.get(), internal, error, name );

	up_t preserve = stream_size< bool_t, error_t >::call( internal, error );
	buffer.preserve( preserve );

	validate( error, buffer.get() );

	if ( OOE_UNLIKELY( !internal ) )
	{
		allocator.set( name );
		buffer.external();
	}
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/ipc/memory/rpc_forward.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_IPC_MEMORY_RPC_FORWARD_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- rpc ------------------------------------------------------------------------------------------
template< BOOST_PP_ENUM_PARAMS( LIMIT, typename t ) >
	struct rpc< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	: private rpc_base
{
	typedef void result_type;

	rpc( memory::transport& transport_, index_t index_ )
		: rpc_base( transport_, index_ )
	{
	}

	result_type operator ()
		( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, typename call_traits< t, >::param_type a ) ) const
	{
		shared_allocator allocator;
		io_buffer buffer( transport.get(), transport.size(), allocator );

		up_t size = stream_size< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >::
			call( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
		store_header( allocator, buffer, size, index );
		stream_write< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >::
			call( buffer.get() BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );

		transport.notify();

		load_header( allocator, buffer );
	}
};

template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
	struct rpc< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	: private rpc_base
{
	typedef typename no_ref< r >::type result_type;

	rpc( memory::transport& transport_, index_t index_ )
		: rpc_base( transport_, index_ )
	{
	}

	result_type operator ()
		( BOOST_PP_ENUM_BINARY_PARAMS( LIMIT, typename call_traits< t, >::param_type a ) ) const
	{
		shared_allocator allocator;
		io_buffer buffer( transport.get(), transport.size(), allocator );

		up_t size = stream_size< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >::
			call( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
		store_header( allocator, buffer, size, index );
		stream_write< BOOST_PP_ENUM_PARAMS( LIMIT, t ) >::
			call( buffer.get() BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, a ) );

		transport.notify();

		load_header( allocator, buffer );
		result_type value;
		stream_read< result_type >::call( buffer.get(), value );
		return value;
	}
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
