/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_IPC_MEMORY_RPC_HPP
	#define OOE_FOUNDATION_IPC_MEMORY_RPC_HPP

#include <vector>

#include "foundation/ipc/memory/rpc_forward.hpp"

namespace ooe
{
	namespace ipc
	{
		namespace memory
		{
			struct find;
			struct list;
			struct find_all;

			template< typename >
				struct call;
		}
	}

//--- ipc::memory::find --------------------------------------------------------
	struct ipc::memory::find
		: private rpc< u32 ( const c8*, const c8* ) >
	{
		typedef rpc< u32 ( const c8*, const c8* ) > base_type;
		using base_type::result_type;

		find( memory::transport& transport_ )
			: base_type( transport_, 1 )
		{
		}

		result_type operator ()( const c8* name, const c8* type ) const
		{
			u32 i = base_type::operator ()( name, type );

			if ( i != static_cast< u32 >( -1 ) )
				return i;

			throw error::runtime( "ipc::memory::find: " ) <<
				"Unable to find \"" << name << "\" of type \"" << type << '\"';
		}
	};

//--- ipc::memory::list --------------------------------------------------------
	struct ipc::memory::list
		: public rpc< std::vector< tuple< std::string, std::string > > ( void ) >
	{
		list( memory::transport& transport_ )
			: rpc< std::vector< tuple< std::string, std::string > > ( void ) >( transport_, 2 )
		{
		}
	};

//--- ipc::memory::find_all ----------------------------------------------------
	struct ipc::memory::find_all
		: private
		rpc< std::vector< u32 > ( const std::vector< tuple< std::string, std::string > >& ) >
	{
		typedef tuple< std::string, std::string > tuple_type;
		typedef std::vector< tuple_type > parameter_type;
		typedef rpc< result_type ( const parameter_type& ) > base_type;
		using base_type::result_type;

		find_all( memory::transport& transport_ )
			: base_type( transport_, 3 )
		{
		}

		result_type operator ()( const parameter_type& parameter ) const
		{
			result_type result = base_type::operator ()( parameter );

			for ( up_t i = 0, end = result.size(); i != end; ++i )
			{
				if ( result[ i ] != static_cast< u32 >( -1 ) )
					continue;

				throw error::runtime( "ipc::memory::find_all: " ) << "Unable to find \"" <<
					parameter[ i ]._0 << "\" of type \"" << parameter[ i ]._1 << '\"';
			}

			return result;
		}
	};
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/ipc/memory/rpc.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_IPC_MEMORY_RPC_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

namespace ooe
{
	namespace ipc
	{
		namespace memory
		{
			template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
				struct call< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;
		}
	}

//--- ipc::memory::call --------------------------------------------------------
	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct ipc::memory::call< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		: public rpc< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		typedef r signature_type( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
		typedef rpc< signature_type > base_type;

		call( memory::transport& transport_, const c8* name )
			: base_type( transport_, find( transport_ )( name, typeid( signature_type ).name() ) )
		{
		}
	};
}

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
