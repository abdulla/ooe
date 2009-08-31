/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_TEST_UNIT_RUNNER_HPP
#define OOE_TEST_UNIT_RUNNER_HPP

#include <map>

#include "foundation/utility/string.hpp"

namespace ooe
{
	namespace unit
	{
		class group_base;
		class runner;

		extern runner global_runner;
	}

//--- unit::runner -------------------------------------------------------------
	class unit::runner
	{
	public:
		typedef std::map< std::string, group_base* > map_type;
		typedef map_type::const_iterator iterator_type;

		runner( void );

		iterator_type begin( void ) const;
		iterator_type end( void ) const;
		void insert( const std::string&, group_base& );
		bool run( time_t = 60 ) const;
		bool run( const std::string&, time_t = 60 ) const;

	private:
		map_type map;
	};
}

#endif	// OOE_TEST_UNIT_RUNNER_HPP
