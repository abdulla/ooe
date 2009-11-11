/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_LOCAL_HPP
#define OOE_COMPONENT_REGISTRY_LOCAL_HPP

#include <typeinfo>
#include <vector>

#include "component/registry/module.hpp"
#include "foundation/executable/library.hpp"

namespace ooe
{
	namespace facade
	{
		class local;
	}

//--- facade::local ------------------------------------------------------------
	class facade::local
	{
	public:
		typedef std::vector< any > vector_type;

		const vector_type& get( void ) const OOE_VISIBLE;
		void insert( up_t, any ) OOE_VISIBLE;

	private:
		vector_type vector;
	};

//--- local --------------------------------------------------------------------
	class local
	{
	public:
		local( const std::string& ) OOE_VISIBLE;
		any find( const std::string&, const std::string& ) const OOE_VISIBLE;

		template< typename type >
			type find( const std::string& name,
			typename enable_if< is_function_pointer< type > >::type* = 0 ) const
		{
			typedef typename remove_pointer< type >::type function_type;
			any any = find( name, typeid( function_type ).name() );
			return reinterpret_cast< type >( any.function );
		}

		template< typename type >
			type find( const std::string& name,
			typename enable_if< is_member_function_pointer< type > >::type* = 0 ) const
		{
			typedef typename function_of< type >::type signature_type;
			any any = find( name, typeid( signature_type ).name() );
			return reinterpret_cast< type >( any.member );
		}

	private:
		ooe::library library;
		ooe::module module;
		const facade::local::vector_type& vector;
	};
}

#endif	// OOE_COMPONENT_REGISTRY_LOCAL_HPP