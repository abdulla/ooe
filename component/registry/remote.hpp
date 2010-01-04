/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_REMOTE_HPP
#define OOE_COMPONENT_REGISTRY_REMOTE_HPP

#include "component/registry/interface.hpp"
#include "foundation/ipc/switchboard.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/rpc.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( facade ) )

//--- remote ---------------------------------------------------------------------------------------
class remote
{
public:
	typedef ipc::switchboard::call_type call_type;
	typedef std::vector< call_type > vector_type;

	const vector_type& get( void ) const OOE_VISIBLE;
	void insert( up_t, call_type ) OOE_VISIBLE;

	template< typename type >
		void insert( up_t index, typename enable_if< is_function_pointer< type > >::type* = 0 )
	{
		typedef typename remove_pointer< type >::type function_type;
		insert( index, ipc::invoke_function< function_type >::call );
	}

	template< typename type >
		void insert( up_t index,
		typename enable_if< is_member_function_pointer< type > >::type* = 0 )
	{
		typedef typename member_of< type >::type object_type;
		typedef typename remove_member< type >::type member_type;
		insert( index, ipc::invoke_member< object_type, member_type >::call );
	}

private:
	vector_type vector;
};

OOE_NAMESPACE_END( ( ooe )( facade ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- remote ---------------------------------------------------------------------------------------
class remote
{
public:
	remote( const std::string& ) OOE_VISIBLE;
	bool supports( const interface& ) const OOE_VISIBLE;

	template< typename type >
		ipc::memory::rpc< type > find( const c8* name )
	{
		return ipc::memory::call< type >( client, name );
	}

private:
	mutable ipc::memory::client client;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_REGISTRY_REMOTE_HPP
