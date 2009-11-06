/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_REGISTRY_HPP
#define OOE_COMPONENT_REGISTRY_REGISTRY_HPP

#include "component/registry/module.hpp"
#include "foundation/ipc/memory/client.hpp"

namespace ooe
{
//--- registry -----------------------------------------------------------------
	class OOE_VISIBLE registry
	{
	public:
		typedef std::vector< module::info_tuple > info_vector;

		registry( void );

		info_vector find( const interface& );
		void insert( const module::info_tuple& );

	private:
		mutable ipc::memory::client client;
	};
}

#endif	// OOE_COMPONENT_REGISTRY_REGISTRY_HPP
