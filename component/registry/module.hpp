/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_MODULE_HPP
#define OOE_COMPONENT_REGISTRY_MODULE_HPP

#include <typeinfo>
#include <vector>

#include "foundation/executable/library.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/utility/macro.hpp"
#include "foundation/utility/string.hpp"
#include "foundation/utility/traits.hpp"
#include "foundation/utility/tuple.hpp"

namespace ooe
{
//--- module -------------------------------------------------------------------
	class module
	{
	public:
		enum type
		{
			library,
			server
		};

		typedef tuple< type, std::string > info_tuple;
		typedef tuple< std::string, std::string > name_tuple;
		typedef std::vector< name_tuple > name_vector;
		typedef std::vector< any > datum_vector;

		module( type, const std::string& ) OOE_VISIBLE;

		const info_tuple& info( void ) const OOE_VISIBLE;
		const name_vector& names( void ) const OOE_VISIBLE;
		const datum_vector& data( void ) const OOE_VISIBLE;
		void insert( const name_tuple&, any ) OOE_VISIBLE;

		template< typename type >
			void insert( const std::string& name, type function,
			typename enable_if< is_function_pointer< type > >::type* = 0 )
		{
			typedef typename remove_pointer< type >::type function_type;
			insert( name_tuple( name, typeid( function_type ).name() ), function );
		}

		template< typename type >
			void insert( const std::string& name, type member,
			typename enable_if< is_member_function_pointer< type > >::type* = 0 )
		{
			typedef typename function_of< type >::type signature_type;
			insert( name_tuple( name, typeid( signature_type ).name() ), member );
		}

	private:
		info_tuple info_;
		name_vector names_;
		datum_vector data_;
	};

//--- interface ----------------------------------------------------------------
	class interface
	{
	public:
		const module::name_vector& names( void ) const OOE_VISIBLE;
		void insert( const module::name_tuple& ) OOE_VISIBLE;

		template< typename type >
			void insert( const std::string& name,
			typename enable_if< is_function_pointer< type > >::type* = 0 )
		{
			typedef typename remove_pointer< type >::type function_type;
			insert( module::name_tuple( name, typeid( function_type ).name() ) );
		}

		template< typename type >
			void insert( const std::string& name,
			typename enable_if< is_member_function_pointer< type > >::type* = 0 )
		{
			typedef typename function_of< type >::type signature_type;
			insert( module::name_tuple( name, typeid( signature_type ).name() ) );
		}

	private:
		module::name_vector names_;
	};

//--- internal -----------------------------------------------------------------
	class internal
	{
	public:
		internal( const module::info_tuple& ) OOE_VISIBLE;
		any search( const module::name_tuple& ) OOE_VISIBLE;

		template< typename type >
			type find( const module::name_tuple& name,
			typename enable_if< is_function_pointer< type > >::type* = 0 )
		{
			return search( name );
		}

		template< typename type >
			type find( const module::name_tuple& name,
			typename enable_if< is_member_function_pointer< type > >::type* = 0 )
		{
			return search( name );
		}

	private:
		ooe::library library;
		ooe::module module;
	};

//--- external -----------------------------------------------------------------
	class external
	{
	public:
		external( const module::info_tuple& );

	private:
		ipc::memory::client client;
	};
}

#endif	// OOE_COMPONENT_REGISTRY_MODULE_HPP
