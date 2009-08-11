/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <cerrno>
#include <csignal>

#include "foundation/executable/fork_io.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/guard.hpp"

namespace
{
	using namespace ooe;

	void swap_io( s32& read, s32& write )
	{
		s32 in = dup( STDIN_FILENO );

		if ( in == -1 )
			throw error::runtime( "executable::fork_io: " ) <<
				"Unable to duplicate fd " << STDIN_FILENO << ": " << error::number( errno );

		guard< int ( int ) > guard_in( close, in );
		s32 out = dup( STDOUT_FILENO );

		if ( out == -1 )
			throw error::runtime( "executable::fork_io: " ) <<
				"Unable to duplicate fd " << STDOUT_FILENO << ": " << error::number( errno );

		guard< int ( int ) > guard_out( close, out );

		if ( dup2( read, STDIN_FILENO ) == -1 )
			throw error::runtime( "executable::fork_io: " ) <<
				"Unable to duplicate fd " << read << ": " << error::number( errno );
		else if ( dup2( write, STDOUT_FILENO ) == -1 )
		{
			// attempt to restore stdin
			dup2( in, STDIN_FILENO );
			throw error::runtime( "executable::fork_io: " ) <<
				"Unable to duplicate fd " << write << ": " << error::number( errno );
		}

		read = in;
		write = out;
		guard_in.clear();
		guard_out.clear();
	}
}

namespace ooe
{
//--- executable::fork_id ------------------------------------------------------
	struct executable::fork_id
	{
		s32 read;
		s32 write;
		pid_t pid;

		fork_id( void );
		~fork_id( void );
	};

	executable::fork_id::fork_id( void )
		: read(), write(), pid()
	{
		s32 pipe_1[ 2 ];

		if ( pipe( pipe_1 ) )
			throw error::runtime( "executable::fork_io: " ) <<
				"Unable to create 1st pipe: " << error::number( errno );

		guard< int ( int ) > guard_1a( close, pipe_1[ 0 ] );
		guard< int ( int ) > guard_1b( close, pipe_1[ 1 ] );
		s32 pipe_2[ 2 ];

		if ( pipe( pipe_2 ) )
			throw error::runtime( "executable::fork_io: " ) <<
				"Unable to create 2nd pipe: " << error::number( errno );

		guard< int ( int ) > guard_2a( close, pipe_2[ 0 ] );
		guard< int ( int ) > guard_2b( close, pipe_2[ 1 ] );
		pid = fork();

		if ( pid == -1 )
			throw error::runtime( "executable::fork_io: " ) <<
				"Unable to fork process: " << error::number( errno );
		else if ( pid )
		{
			guard_1a.clear();
			guard_2b.clear();
			read = pipe_1[ 0 ];
			write = pipe_2[ 1 ];
		}
		else
		{
			guard_1b.clear();
			guard_2a.clear();
			read = pipe_2[ 0 ];
			write = pipe_1[ 1 ];
			swap_io( read, write );
		}
	}

	executable::fork_id::~fork_id( void )
	{
		close( write );
		close( read );
	}

//--- executable::fork_io ------------------------------------------------------
	executable::fork_io::fork_io( void )
		: id( new fork_id )
	{
	}

	executable::fork_io::~fork_io( void )
	{
	}

	executable::fork_io::fork_io( const fork_io& copy )
		: id( copy.id )
	{
	}

	executable::fork_io& executable::fork_io::operator =( const fork_io& copy )
	{
		id = copy.id;
		return *this;
	}

	up_t executable::fork_io::read( void* buffer, up_t bytes )
	{
		sp_t read_ = ::read( id->read, buffer, bytes );

		if ( read_ )
			throw error::runtime( "executable::fork_io: " ) <<
				"Unable to read from process " << id->pid << ": " << error::number( errno );

		return read_;
	}

	up_t executable::fork_io::write( const void* buffer, up_t bytes )
	{
		sp_t wrote = ::write( id->write, buffer, bytes );

		if ( wrote == -1 )
			throw error::runtime( "executable::fork_io: " ) <<
				"Unable to write to process " << id->pid << ": " << error::number( errno );

		return wrote;
	}

	executable::fork_io::wait_type executable::fork_io::wait( bool block ) const
	{
		s32 status;
		s32 pid = waitpid( id->pid, &status, block ? 0 : WNOHANG );

		if ( pid == id->pid )
			return WEXITSTATUS( status ) ? failure : success;
		else if ( !pid )
			return waiting;
		else
			throw error::runtime( "executable::fork_io: " ) <<
				"Unable to wait for process " << id->pid << ": " << error::number( errno );
	}

	void executable::fork_io::signal( s32 number ) const
	{
		if ( kill( id->pid, number ) == -1 )
			throw error::runtime( "executable::fork_io: " ) <<
				"Unable to signal process " << id->pid << ": " << error::number( errno );
	}

	bool executable::fork_io::is_child( void ) const
	{
		return !id->pid;
	}

	void executable::fork_io::exit( bool success )
	{
		_exit( success ? EXIT_SUCCESS : EXIT_FAILURE );
	}
}
