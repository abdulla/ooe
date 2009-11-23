/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_MODULE_HPP
#define OOE_COMPONENT_REGISTRY_MODULE_HPP

#include <map>
#include <vector>

#include "foundation/utility/macro.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/pointer.hpp"
#include "foundation/utility/string.hpp"

namespace ooe
{
//--- facade_id ----------------------------------------------------------------
	struct OOE_VISIBLE facade_id
	{
		typedef void ( * function_type )( const void* );

		const void* pointer;
		function_type function;

		facade_id( const void*, function_type );
		~facade_id( void );
	};

//--- module -------------------------------------------------------------------
	class module
	{
	public:
		typedef tuple< std::string, std::string > vector_tuple;
		typedef std::vector< vector_tuple > vector_type;
		typedef shared_ptr< facade_id > facade_ptr;
		typedef std::map< std::string, facade_ptr > map_type;

		const vector_type& get( void ) const OOE_VISIBLE;
		up_t insert( const std::string&, const std::string& ) OOE_VISIBLE;

		const void* find( const std::string& ) const OOE_VISIBLE;
		void insert( const std::string&, const facade_ptr& ) OOE_VISIBLE;

		template< typename type >
			void insert( const std::string& name, scoped_ptr< type >& pointer )
		{
			insert( name, new facade_id( pointer, destroy< type > ) );
			pointer.release();
		}

	private:
		vector_type vector;
		map_type map;
	};
}

#endif	// OOE_COMPONENT_REGISTRY_MODULE_HPP
