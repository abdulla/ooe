/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_SERVICE_SERVICE_HPP
#define OOE_EXTERNAL_SERVICE_SERVICE_HPP

#include <map>
#include <vector>

#include "foundation/utility/macro.hpp"
#include "foundation/utility/string.hpp"

namespace ooe
{
	class signal_base;

	class OOE_VISIBLE service
	{
	public:
		typedef void ( * function_type )( void* );

		void insert( function_type, void* );
		void insert( const std::string&, signal_base& );

		signal_base* operator []( const std::string& ) const;
		void operator ()( void ) const;

	private:
		typedef std::vector< tuple< function_type, void* > > vector_type;
		typedef std::map< std::string, signal_base* > map_type;

		vector_type vector;
		map_type map;
	};
}

#endif	// OOE_EXTERNAL_SERVICE_SERVICE_HPP
