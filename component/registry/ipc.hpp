/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_IPC_HPP
#define OOE_COMPONENT_REGISTRY_IPC_HPP

#include "component/registry/module.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/rpc.hpp"
#include "foundation/ipc/memory/switchboard.hpp"

namespace ooe
{
	namespace facade
	{
		class ipc;
	}

//--- facade::ipc --------------------------------------------------------------
	class facade::ipc
	{
	public:
		typedef ooe::ipc::memory::switchboard::call_type call_type;

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
		typedef std::vector< call_type > vector_type;

		vector_type vector;
	};

//--- external -----------------------------------------------------------------
	class external
	{
	public:
		external( const module::info_tuple& );

		template< typename type >
			ipc::memory::rpc< type > find( const std::string& name )
		{
			return ipc::memory::call< type >( client, name );
		}

	private:
		ipc::memory::client client;
	};
}

#endif	// OOE_COMPONENT_REGISTRY_IPC_HPP
