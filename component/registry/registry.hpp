/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_REGISTRY_HPP
#define OOE_COMPONENT_REGISTRY_REGISTRY_HPP

#include "component/registry/interface.hpp"
#include "foundation/ipc/memory/client.hpp"

namespace ooe
{
//--- registry -----------------------------------------------------------------
	class OOE_VISIBLE registry
	{
	public:
		enum type
		{
			server,
			library
		};

		typedef tuple< type, std::string > info_tuple;
		typedef std::vector< info_tuple > info_vector;

		registry( void );

		info_vector find( const interface& );
		void insert( type, const std::string& );
		void scan( const std::string& );
		std::string surrogate( const std::string& );

	private:
		mutable ipc::memory::client client;
	};
}

#endif	// OOE_COMPONENT_REGISTRY_REGISTRY_HPP
