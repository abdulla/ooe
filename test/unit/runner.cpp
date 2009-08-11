/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <list>

#include <csignal>
#include <cstdlib>

#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/executable/timer.hpp"
#include "foundation/utility/error.hpp"
#include "test/unit/group.hpp"
#include "test/unit/runner.hpp"

namespace
{
	using namespace ooe;
	typedef tuple< executable::fork_io, timer > tuple_type;
	typedef std::list< tuple_type > list_type;

	void run_test( const unit::group_base::iterator_type& i )
	{
		try
		{
			( *i )();
			executable::fork_io::exit( true );
		}
		catch ( error::runtime& )
		{
		}
		catch ( std::exception )
		{
		}
		catch ( ... )
		{
		}

		executable::fork_io::exit( false );
	}

	up_t collect_tests( list_type& list, time_t time_out )
	{
		up_t passed = 0;

		for ( list_type::iterator i = list.begin(), end = list.end(); i != end; ++i )
		{
			executable::fork_io::wait_type status = i->_0.wait( false );

			if ( status == executable::fork_io::success )
				++passed;
			else if ( status == executable::fork_io::waiting )
			{
				if ( i->_1.elapsed() < time_out )
					continue;
				else
					i->_0.signal( SIGKILL );
			}

			// add to new list, with information
			list.erase( i );
		}

		return passed;
	}

	void run_group( unit::group_base& group, time_t time_out )
	{
		list_type list;

		for ( unit::group_base::iterator_type i = group.begin(), end = group.end(); i != end; ++i )
		{
			executable::fork_io fork_io;

			if ( !fork_io.is_child() )
				list.push_back( tuple_type( fork_io, timer() ) );
			else
				run_test( i );
		}

		up_t passed = 0;

		do
			passed += collect_tests( list, time_out );
		while ( !list.empty() );
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

	void unit::runner::run( time_t time_out ) const
	{
		for ( map_type::const_iterator i = begin(), j = end(); i != j; ++i )
			run_group( *i->second, time_out );
	}

	void unit::runner::run( const std::string& name, time_t time_out ) const
	{
		map_type::const_iterator i = map.find( name );

		if ( i == map.end() )
			throw error::runtime( "unit::runner: " ) << "Unable to find group \"" << name << '"';

		run_group( *i->second, time_out );
	}
}
