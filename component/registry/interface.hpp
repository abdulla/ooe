/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_INTERFACE_HPP
#define OOE_COMPONENT_REGISTRY_INTERFACE_HPP

#include <typeinfo>
#include <vector>

#include "foundation/utility/macro.hpp"
#include "foundation/utility/string.hpp"
#include "foundation/utility/traits.hpp"

namespace ooe
{
//--- interface ----------------------------------------------------------------
	class interface
	{
	public:
		typedef tuple< std::string, std::string > vector_tuple;
		typedef std::vector< vector_tuple > vector_type;

		const vector_type& get( void ) const OOE_VISIBLE;
		void insert( const std::string&, const std::string& ) OOE_VISIBLE;

		template< typename type >
			void insert( const std::string& name,
			typename enable_if< is_function_pointer< type > >::type* = 0 )
		{
			typedef typename remove_pointer< type >::type function_type;
			insert( name, typeid( function_type ).name() );
		}

		template< typename type >
			void insert( const std::string& name,
			typename enable_if< is_member_function_pointer< type > >::type* = 0 )
		{
			typedef typename function_of< type >::type signature_type;
			insert( name, typeid( signature_type ).name() );
		}

	private:
		vector_type vector;
	};
}

#endif	// OOE_COMPONENT_REGISTRY_INTERFACE_HPP
