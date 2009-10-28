/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_INTERFACE_HPP
#define OOE_COMPONENT_REGISTRY_INTERFACE_HPP

#include <typeinfo>
#include <vector>

#include "foundation/utility/macro.hpp"
#include "foundation/utility/string.hpp"
#include "foundation/utility/traits.hpp"
#include "foundation/utility/tuple.hpp"

namespace ooe
{
//--- interface ----------------------------------------------------------------
	class interface
	{
	public:
		typedef tuple< std::string, std::string > name_tuple;
		typedef std::vector< name_tuple > name_vector;

		template< typename type >
			void insert( const std::string& name,
			typename enable_if< is_function_pointer< type > >::type* = 0 )
		{
			typedef typename remove_pointer< type >::type function_type;
			names_.push_back( name_tuple( name, typeid( function_type ).name() ) );
		}

		template< typename type >
			void insert( const std::string& name,
			typename enable_if< is_member_function_pointer< type > >::type* = 0 )
		{
			typedef typename function_of< type >::type signature_type;
			names_.push_back( name_tuple( name, typeid( signature_type ).name() ) );
		}

		const name_vector& names( void ) const OOE_VISIBLE;

	private:
		name_vector names_;
	};
}

#endif	// OOE_COMPONENT_REGISTRY_INTERFACE_HPP
