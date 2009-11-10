/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_REMOTE_HPP
#define OOE_COMPONENT_REGISTRY_REMOTE_HPP

#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/rpc.hpp"
#include "foundation/ipc/memory/switchboard.hpp"

namespace ooe
{
	namespace facade
	{
		class remote;
	}

//--- facade::remote -----------------------------------------------------------
	class facade::remote
	{
	public:
		typedef ooe::ipc::memory::switchboard::call_type call_type;
		typedef std::vector< call_type > vector_type;

		const vector_type& get( void ) const;
		void insert( up_t, call_type );

		template< typename type >
			void insert( up_t index,
			typename enable_if< is_function_pointer< type > >::type* = 0 )
		{
			typedef typename remove_pointer< type >::type function_type;
			insert( index, ooe::ipc::memory::invoke_function< function_type >::call );
		}

		template< typename type >
			void insert( up_t index,
			typename enable_if< is_member_function_pointer< type > >::type* = 0 )
		{
			typedef typename member_of< type >::type object_type;
			typedef typename remove_member< type >::type member_type;
			insert( index, ooe::ipc::memory::invoke_member< object_type, member_type >::call );
		}

	private:
		vector_type vector;
	};

//--- external -----------------------------------------------------------------
	class external
	{
	public:
		external( const std::string& );

		template< typename type >
			ipc::memory::rpc< type > find( const std::string& name )
		{
			return ipc::memory::call< type >( client, name );
		}

	private:
		ipc::memory::client client;
	};
}

#endif	// OOE_COMPONENT_REGISTRY_REMOTE_HPP
