/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_IPC_SOCKET_RPC_HPP
	#define OOE_FOUNDATION_IPC_SOCKET_RPC_HPP

#include "foundation/ipc/socket/rpc_forward.hpp"

namespace ooe
{
	namespace ipc
	{
		namespace socket
		{
			class find_result;
			struct find;

			template< typename >
				struct call;
		}
	}

//--- ipc::socket::find_result -------------------------------------------------
	class ipc::socket::find_result
	{
	public:
		find_result( socket::result< u32 > result_, const c8* name_, const c8* type_ )
			: result( result_ ), name( name_ ), type( type_ )
		{
		}

		u32 operator ()( void ) const
		{
			u32 i = result();

			if ( i != static_cast< u32 >( -1 ) )
				return i;

			throw error::runtime( "ipc::socket::find: " ) <<
				"Unable to find \"" << name << "\" of type \"" << type << '\"';
		}

	private:
		socket::result< u32 > result;
		const c8* name;
		const c8* type;
	};

//--- ipc::socket::find --------------------------------------------------------
	struct ipc::socket::find
		: public rpc< u32 ( const c8*, const c8* ) >
	{
		typedef rpc< u32 ( const c8*, const c8* ) > base_type;

		find( socket::client& client_ )
			: base_type( client_, 1 )
		{
		}

		find_result operator ()( const c8* name, const c8* type ) const
		{
			return find_result( base_type::operator ()( name, type ), name, type );
		}
	};
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/ipc/socket/rpc.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_IPC_SOCKET_RPC_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

	#if LIMIT != OOE_PP_LIMIT
namespace ooe
{
	namespace ipc
	{
		namespace socket
		{
			template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
				struct call< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;
		}
	}

//--- ipc::socket::rpc ---------------------------------------------------------
	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct ipc::socket::call< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		: public rpc< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		typedef r signature_type( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
		typedef rpc< signature_type > base_type;

		call( socket::client& client_, const c8* name )
			: base_type( client_, find( client_ )( name, typeid( signature_type ).name() )() )
		{
		}
	};
}
	#endif

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
