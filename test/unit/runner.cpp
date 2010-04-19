/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iomanip>
#include <iostream>
#include <list>

#include <cstdlib>

#include <fcntl.h>

#include "foundation/io/file.hpp"
#include "foundation/executable/environment.hpp"
#include "foundation/executable/fork_io.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/executable/timer.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/scoped.hpp"
#include "test/unit/group.hpp"
#include "test/unit/runner.hpp"

namespace
{
	using namespace ooe;
	typedef tuple< up_t, fork_io, file, timer > list_tuple;
	typedef std::list< list_tuple > list_type;
	typedef tuple< bool, std::string > vector_tuple;
	typedef std::vector< vector_tuple > vector_type;
	bool test_status;

	void run_test( const unit::group_base::iterator_type& i, void* pointer, bool no_stdout )
	{
		try
		{
			if ( no_stdout )
				executable::null_fd( STDOUT_FILENO );

			test_status = true;
			( *i )( pointer );

			if ( test_status )
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
			std::cerr << "An unknown exception was thrown\n";
		}

		fork_io::exit( false );
	}

	std::string read( file& in )
	{
		std::string out;
		c8 buffer[ executable::static_page_size ];
		up_t read;

		do
		{
			read = in.read( buffer, sizeof( buffer ) );
			buffer[ read ] = 0;
			out += buffer;
		}
		while ( read == sizeof( buffer ) );

		return out;
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
				if ( i->_3.elapsed() < time_out )
				{
					vector[ i->_0 ]._1 += read( i->_2 );
					continue;
				}

				i->_1.signal( SIGKILL );
			}

			vector[ i->_0 ]._0 = passed;
			vector[ i->_0 ]._1 += read( i->_2 );
			i = --list.erase( i );
		}
	}

	vector_type run_group( unit::group_base& group, time_t time_out, bool no_stdout )
	{
		opaque_ptr pointer = group.create_setup();
		list_type list;
		up_t j = 0;

		for ( unit::group_base::iterator_type i = group.begin(), end = group.end(); i != end; ++i )
		{
			file_pair pair = make_pipe();
			fork_io fork_io( io_triplet( -1, -1, pair._1.get() ) );

			if ( !fork_io.is_child() )
				list.push_back( list_tuple( j++, fork_io, pair._0, timer() ) );
			else
				run_test( i, pointer, no_stdout );
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

	bool safe_run( const unit::runner::map_type::const_iterator i, time_t time_out, bool no_stdout )
	{
		std::string output;

		try
		{
			vector_type vector = run_group( *i->second, time_out, no_stdout );
			print_tests( i->first, vector );
			return is_successful( vector );
		}
		catch ( error::runtime& error )
		{
			output << error.what() << "\n\nStack trace:" << error.where();
		}
		catch ( std::exception& error )
		{
			output << error.what();
		}
		catch ( ... )
		{
			output << "An unknown exception was thrown";
		}

		std::cout <<
			"Group: " << i->first << "\n"
			"    Failed setup:\n" << output << '\n';

		return false;
	}
}

namespace ooe
{
//--- unit ---------------------------------------------------------------------
	void unit::fail( void )
	{
		test_status = false;
	}

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

	bool unit::runner::run( time_t time_out, bool no_stdout ) const
	{
		bool success = true;

		for ( map_type::const_iterator i = begin(), j = end(); i != j; ++i )
			success = safe_run( i, time_out, no_stdout );

		return success;
	}

	bool unit::runner::run( const std::string& name, time_t time_out, bool no_stdout ) const
	{
		map_type::const_iterator i = map.find( name );

		if ( i == map.end() )
			throw error::runtime( "unit::runner: " ) << "Unable to find group \"" << name << '"';

		return safe_run( i, time_out, no_stdout );
	}
}
