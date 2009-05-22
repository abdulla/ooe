/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_REGISTRY_HPP
#define OOE_COMPONENT_REGISTRY_REGISTRY_HPP

#include <vector>

#include "foundation/executable/library.hpp"
#include "foundation/ipc/memory/client.hpp"

namespace ooe
{
	typedef any* ( * module_find_type )( void*, const std::string& );
	typedef void ( * module_close_type )( void* );
	typedef tuple< void*, module_close_type, module_find_type > module_tuple;

	typedef shared_ptr< struct module > module_type;

//--- builder ------------------------------------------------------------------
	class builder_internal
	{
	};

	class builder_external
	{
	};

//--- module -------------------------------------------------------------------
	struct module
	{
		enum type
		{
			internal,
			external,
			either
		};

		typedef std::vector< std::string > list_type;

		virtual ~module( void ) {}

		virtual type state( void ) const = 0;
		virtual list_type interface_list( void ) const = 0;
	};

	class module_internal
		: public module
	{
	public:

		module_internal( const std::string& );

		virtual type state( void ) const;
		virtual list_type interface_list( void ) const;

		any* find( const std::string& ) const;

	private:
		const ooe::library library;
		builder_internal builder;
	};

	class module_external
		: public module
	{
	public:
		module_external( const std::string& );

		virtual type state( void ) const;
		virtual list_type interface_list( void ) const;

		u32 find( const std::string& ) const;
		ipc::memory::client& get( void ) const;

	private:
		mutable ipc::memory::client client;
	};

//--- registry -----------------------------------------------------------------
	class OOE_VISIBLE registry
	{
	public:
		registry( void );

		void insert( const std::string&, module::type );
		module_type find( const std::string&, module::type ) const;

	private:
		mutable ipc::memory::client client;
	};
}

#endif	// OOE_COMPONENT_REGISTRY_REGISTRY_HPP
