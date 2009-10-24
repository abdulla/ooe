/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iomanip>
#include <iostream>
#include <list>

#include <csignal>
#include <cstdlib>

#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/timer.hpp"
#include "foundation/utility/error.hpp"
#include "test/unit/group.hpp"
#include "test/unit/runner.hpp"

namespace
{
	using namespace ooe;
	typedef tuple< up_t, fork_io, timer > list_tuple;
	typedef std::list< list_tuple > list_type;
	typedef tuple< bool, std::string > vector_tuple;
	typedef std::vector< vector_tuple > vector_type;

	void run_test( const unit::group_base::iterator_type& i )
	{
		try
		{
			( *i )();
			fork_io::exit( true );
		}
		catch ( error::runtime& error )
		{
			std::cerr << error.what() << "\n\nStack trace:" << error.where() << '\n';
		}
		catch ( std::exception& error )
		{
			std::cerr << error.what() << '\n';
		}
		catch ( ... )
		{
			std::cerr << "Unknown exception\n";
		}

		fork_io::exit( false );
	}

	void collect_tests( vector_type& vector, list_type& list, time_t time_out )
	{
		for ( list_type::iterator i = list.begin(), end = list.end(); i != end; ++i )
		{
			bool passed = false;
			fork_io::wait_type status = i->_1.wait( false );

			if ( status == fork_io::success )
				passed = true;
			else if ( status == fork_io::waiting )
			{
				if ( i->_2.elapsed() < time_out )
					continue;
				else
					i->_1.signal( SIGKILL );
			}

			vector[ i->_0 ] = vector_tuple( passed, read( i->_1 ) );
			i = --list.erase( i );
		}
	}

	vector_type run_group( unit::group_base& group, time_t time_out )
	{
		list_type list;
		up_t j = 0;

		for ( unit::group_base::iterator_type i = group.begin(), end = group.end(); i != end; ++i )
		{
			fork_io fork_io;

			if ( !fork_io.is_child() )
				list.push_back( list_tuple( j++, fork_io, timer() ) );
			else
				run_test( i );
		}

		vector_type vector( j );

		while ( !list.empty() )
			collect_tests( vector, list, time_out );

		return vector;
	}

	void print_tests( const std::string& name, const vector_type& vector )
	{
		std::cout << "Group: " << name << '\n';
		up_t j = 0;

		for ( vector_type::const_iterator i = vector.begin(), end = vector.end(); i != end; ++i )
		{
			if ( !i->_0 )
				++j;

			up_t x = std::distance( vector.begin(), i );
			std::cout << std::setw( 5 ) << x << ": " << ( i->_0 ? "Passed" : "Failed" ) << '\n';
		}

		up_t k = 0;

		for ( vector_type::const_iterator i = vector.begin(), end = vector.end(); i != end; ++i )
		{
			if ( i->_0 )
				continue;

			up_t x = std::distance( vector.begin(), i );
			std::cout << "\n(" << ++k << " of " << j << ") Test " << x << ":\n" << i->_1;
		}
	}

	bool is_successful( const vector_type& vector )
	{
		for ( vector_type::const_iterator i = vector.begin(), end = vector.end(); i != end; ++i )
		{
			if ( !i->_0 )
				return false;
		}

		return true;
	}
}

namespace ooe
{
//--- unit::runner -------------------------------------------------------------
	unit::runner::runner( void )
		: map()
	{
	}

	unit::runner::iterator_type unit::runner::begin( void ) const
	{
		return map.begin();
	}

	unit::runner::iterator_type unit::runner::end( void ) const
	{
		return map.end();
	}

	void unit::runner::insert( const std::string& name, group_base& group )
	{
		map.insert( map_type::value_type( name, &group ) );
	}

	bool unit::runner::run( time_t time_out ) const
	{
		bool success = true;

		for ( map_type::const_iterator i = begin(), j = end(); i != j; ++i )
		{
			vector_type vector = run_group( *i->second, time_out );
			print_tests( i->first, vector );

			if ( !is_successful( vector ) )
				success = false;
		}

		return success;
	}

	bool unit::runner::run( const std::string& name, time_t time_out ) const
	{
		map_type::const_iterator i = map.find( name );

		if ( i == map.end() )
			throw error::runtime( "unit::runner: " ) << "Unable to find group \"" << name << '"';

		vector_type vector = run_group( *i->second, time_out );
		print_tests( name, vector );
		return is_successful( vector );
	}
}
