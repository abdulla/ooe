/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_MODULE_HPP
#define OOE_COMPONENT_REGISTRY_MODULE_HPP

#include <typeinfo>
#include <map>
#include <vector>

#include "foundation/executable/library.hpp"
#include "foundation/utility/macro.hpp"
#include "foundation/utility/pointer.hpp"
#include "foundation/utility/string.hpp"
#include "foundation/utility/traits.hpp"
#include "foundation/utility/tuple.hpp"

namespace ooe
{
//--- facade_id ----------------------------------------------------------------
	struct facade_id
	{
		typedef void ( * function_type )( const void* );

		const void* pointer;
		function_type function;

		facade_id( const void*, function_type );
		~facade_id( void );
	};

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
		typedef shared_ptr< facade_id > facade_ptr;
		typedef std::map< std::string, facade_ptr > facade_map;

		const name_vector& names( void ) const;
		const datum_vector& data( void ) const;
		up_t insert( const name_tuple&, any );

		const void* facade( const std::string& ) const;
		void insert( const std::string&, const facade_ptr& );

		template< typename type >
			void insert( const std::string& name, const type* pointer )
		{
			insert( name, new facade_id( pointer, destroy< type > ) );
		}

	private:
		name_vector names_;
		datum_vector data_;
		facade_map facades;
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
		any find( const module::name_tuple& ) const OOE_VISIBLE;

		template< typename type >
			type find( const std::string& name,
			typename enable_if< is_function_pointer< type > >::type* = 0 ) const
		{
			typedef typename remove_pointer< type >::type function_type;
			module::name_tuple tuple( name, typeid( function_type ).name() );
			return reinterpret_cast< type >( find( tuple ).function );
		}

		template< typename type >
			type find( const std::string& name,
			typename enable_if< is_member_function_pointer< type > >::type* = 0 ) const
		{
			typedef typename function_of< type >::type signature_type;
			module::name_tuple tuple( name, typeid( signature_type ).name() );
			return reinterpret_cast< type >( find( tuple ).member );
		}

	private:
		ooe::library library;
		ooe::module module;
	};
}

#endif	// OOE_COMPONENT_REGISTRY_MODULE_HPP
