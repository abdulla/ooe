/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_MODULE_HPP
#define OOE_COMPONENT_REGISTRY_MODULE_HPP

#include <typeinfo>
#include <vector>

#include "foundation/utility/macro.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/string.hpp"
#include "foundation/utility/traits.hpp"
#include "foundation/utility/tuple.hpp"

namespace ooe
{
//--- module -------------------------------------------------------------------
	class OOE_VISIBLE module
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

		module( type, const std::string& );

		const info_tuple& info( void ) const;
		const name_vector& names( void ) const;
		const datum_vector& data( void ) const;

		template< typename type >
			void insert( const std::string& name, type function,
			typename enable_if< is_function_pointer< type > >::type* = 0 )
		{
			typedef typename remove_pointer< type >::type function_type;
			names_.push_back( name_tuple( name, typeid( function_type ).name() ) );
			data_.push_back( function );
		}

		template< typename type >
			void insert( const std::string& name, type member,
			typename enable_if< is_member_function_pointer< type > >::type* = 0 )
		{
			typedef typename function_of< type >::type signature_type;
			names_.push_back( name_tuple( name, typeid( signature_type ).name() ) );
			data_.push_back( member );
		}

	private:
		info_tuple info_;
		name_vector names_;
		datum_vector data_;
	};
}

#endif	// OOE_COMPONENT_REGISTRY_MODULE_HPP
