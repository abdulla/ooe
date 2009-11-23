/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>
#include <vector>

#include <cerrno>
#include <csignal>
#include <cstdarg>

#include <sys/wait.h>

#include "foundation/executable/environment.hpp"
#include "foundation/executable/fork_io.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/scoped.hpp"

namespace
{
	using namespace ooe;

	void swap_io( s32& read, s32& write )
	{
		s32 in = dup( STDIN_FILENO );

		if ( in == -1 )
			throw error::runtime( "fork_io: " ) <<
				"Unable to duplicate fd " << STDIN_FILENO << ": " << error::number( errno );

		scoped< int ( int ) > scoped_in( close, in );
		s32 out = dup( STDERR_FILENO );

		if ( out == -1 )
			throw error::runtime( "fork_io: " ) <<
				"Unable to duplicate fd " << STDERR_FILENO << ": " << error::number( errno );

		scoped< int ( int ) > scoped_out( close, out );

		if ( dup2( read, STDIN_FILENO ) == -1 )
			throw error::runtime( "fork_io: " ) <<
				"Unable to duplicate fd " << read << ": " << error::number( errno );
		else if ( dup2( write, STDERR_FILENO ) == -1 )
		{
			if ( dup2( in, STDIN_FILENO ) == -1 )
				OOE_WARNING( "fork_io", "Unable to restore standard input" );

			throw error::runtime( "fork_io: " ) <<
				"Unable to duplicate fd " << write << ": " << error::number( errno );
		}

		read = in;
		write = out;
		scoped_in.clear();
		scoped_out.clear();
	}
}

namespace ooe
{
//--- fork_id ------------------------------------------------------------------
	fork_id::fork_id( void )
		: read(), write(), pid()
	{
		s32 pipe_1[ 2 ];

		if ( pipe( pipe_1 ) )
			throw error::runtime( "fork_io: " ) <<
				"Unable to create 1st pipe: " << error::number( errno );

		scoped< int ( int ) > scoped_1a( close, pipe_1[ 0 ] );
		scoped< int ( int ) > scoped_1b( close, pipe_1[ 1 ] );
		s32 pipe_2[ 2 ];

		if ( pipe( pipe_2 ) )
			throw error::runtime( "fork_io: " ) <<
				"Unable to create 2nd pipe: " << error::number( errno );

		scoped< int ( int ) > scoped_2a( close, pipe_2[ 0 ] );
		scoped< int ( int ) > scoped_2b( close, pipe_2[ 1 ] );
		pid = fork();

		if ( pid == -1 )
			throw error::runtime( "fork_io: " ) <<
				"Unable to fork process: " << error::number( errno );
		else if ( pid )
		{
			scoped_1a.clear();
			scoped_2b.clear();
			read = pipe_1[ 0 ];
			write = pipe_2[ 1 ];
		}
		else
		{
			scoped_1b.clear();
			scoped_2a.clear();
			read = pipe_2[ 0 ];
			write = pipe_1[ 1 ];
			swap_io( read, write );
		}
	}

	fork_id::~fork_id( void )
	{
		close( write );
		close( read );
	}

//--- fork_io ------------------------------------------------------------------
	fork_io::fork_io( void )
		: id( new fork_id )
	{
	}

	// required to ensure fork_id can be hidden
	fork_io::~fork_io( void )
	{
	}

	up_t fork_io::read( void* buffer, up_t bytes )
	{
		sp_t read_ = ::read( id->read, buffer, bytes );

		if ( read_ == -1 )
			throw error::runtime( "fork_io: " ) <<
				"Unable to read from process " << id->pid << ": " << error::number( errno );

		return read_;
	}

	up_t fork_io::write( const void* buffer, up_t bytes )
	{
		sp_t wrote = ::write( id->write, buffer, bytes );

		if ( wrote == -1 )
			throw error::runtime( "fork_io: " ) <<
				"Unable to write to process " << id->pid << ": " << error::number( errno );

		return wrote;
	}

	fork_io::wait_type fork_io::wait( bool block ) const
	{
		s32 status;
		s32 pid = waitpid( id->pid, &status, block ? 0 : WNOHANG );

		if ( pid == id->pid )
			return WEXITSTATUS( status ) ? failure : success;
		else if ( !pid )
			return waiting;
		else
			throw error::runtime( "fork_io: " ) <<
				"Unable to wait for process " << id->pid << ": " << error::number( errno );
	}

	void fork_io::signal( s32 number ) const
	{
		if ( kill( id->pid, number ) == -1 )
			throw error::runtime( "fork_io: " ) <<
				"Unable to signal process " << id->pid << ": " << error::number( errno );
	}

	bool fork_io::is_child( void ) const
	{
		return !id->pid;
	}

	void fork_io::execute( const std::string& path, ... )
	{
		try
		{
			std::vector< const c8* > argument;
			argument.push_back( path.c_str() );
			const c8* string;

			va_list list;
			va_start( list, path );

			do
			{
				string = va_arg( list, const c8* );
				argument.push_back( string );
			}
			while ( string );

			va_end( list );
			execv( path.c_str(), const_cast< c8** >( &argument[ 0 ] ) );
			OOE_WARNING( "excutable::spawn", "Unable to exec process: " << error::number( errno ) );
		}
		catch ( std::exception& error )
		{
			OOE_WARNING( "excutable::spawn", "Unable to exec process: " << error.what() );
		}

		exit( false );
	}

	void fork_io::exit( bool success )
	{
		_exit( success ? EXIT_SUCCESS : EXIT_FAILURE );
	}

	std::string read( fork_io& in )
	{
		std::string string;
		c8 buffer[ executable::static_page_size ];
		up_t read;

		do
		{
			read = in.read( buffer, sizeof( buffer ) );
			buffer[ read ] = 0;
			string += buffer;
		}
		while ( read == sizeof( buffer ) );

		return string;
	}

	fork_io& operator <<( fork_io& out, const c8* value )
	{
		out.write( value, std::strlen( value ) + 1 );
		return out;
	}

	fork_io& operator <<( fork_io& out, const std::string& value )
	{
		out.write( value.c_str(), value.size() + 1 );
		return out;
	}
}
