/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cerrno>
#include <climits>
#include <csignal>
#include <cstdlib>

#include <fcntl.h>

#include "foundation/executable/program.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/scoped.hpp"

namespace
{
	using namespace ooe;
	s32 signal_code = 0;

	void signal_handler( s32 code )
	{
		const c8* description;
		std::string trace;

		switch ( code )
		{
		case SIGFPE:
		case SIGBUS:
		case SIGSEGV:
		case SIGABRT:
			description = strsignal( code );
			trace = stack_trace();
			OOE_WARNING( "executable",
				"Caught signal " << code << ": " << description << "\n\nStack trace:\n" << trace );
			std::exit( EXIT_FAILURE );

		default:
			signal_code = code;
			break;
		}
	}
}

namespace ooe
{
//--- executable ---------------------------------------------------------------
	executable::signal_handler_type
		executable::signal( struct sigaction& action, signal_handler_type handler, s32 code )
	{
		std::memset( &action, 0, sizeof( action ) );
		action.sa_handler = handler;

		struct sigaction prior;
		std::memset( &prior, 0, sizeof( prior ) );

		if ( sigaction( code, &action, &prior ) )
			OOE_WARNING( "executable", "Handler for signal " << code << " was not registered" );
		else if ( prior.sa_flags & SA_SIGINFO )
			OOE_WARNING( "executable",
				"Previous handler for signal " << code << " is unsupported" );
		else
			return prior.sa_handler;

		return 0;
	}

	s32 executable::signal( void )
	{
		return signal_code;
	}

	void executable::quit( void )
	{
		signal_code = SIGQUIT;
	}

	s32 executable::launch( launch_type launch, s32 argc, c8** argv )
	{
		std::set_terminate( __gnu_cxx::__verbose_terminate_handler );
		struct sigaction action;
		executable::signal( action, SIG_IGN, SIGCHLD );
		executable::signal( action, signal_handler, SIGFPE );
		executable::signal( action, signal_handler, SIGBUS );
		executable::signal( action, signal_handler, SIGSEGV );
		executable::signal( action, signal_handler, SIGTERM );

		s32 status = EXIT_FAILURE;

		try
		{
			path_tuple path = executable::path();
			std::string log = path._0 + path._1 + ".log";

			if ( close( STDERR_FILENO ) == -1 )
				throw error::runtime( "executable::launch: " ) <<
					"Unable to close standard error: " << error::number( errno );
			else if ( open( log.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600 ) != STDERR_FILENO )
				throw error::runtime( "executable::launch: " ) <<
					"Unable to override standard error: " << error::number( errno );
			else if ( platform::launch( launch, path._0, path._1, argc, argv ) )
				status = EXIT_SUCCESS;
		}
		catch ( error::runtime& error )
		{
			OOE_WARNING( "executable",
				"Uncaught exception:\n" << error.what() << "\n\nStack trace:" << error.where() );
		}
		catch ( std::exception& error )
		{
			OOE_WARNING( "executable", "Uncaught exception:\n" << error.what() );
		}
		catch ( ... )
		{
			OOE_WARNING( "executable", "Uncaught exception:\nUnknown\n" );
		}

		return status;
	}

	void executable::null_fd( s32 fd )
	{
		s32 null = open( "/dev/null", O_RDWR );

		if ( null == -1 )
			throw error::runtime( "executable::null_fd: " ) <<
				"Unable to open /dev/null: " << error::number( errno );

		s32 result = dup2( null, fd );

		if ( close( null ) )
			OOE_WARNING( "executable::null_fd",
				"Unable to close /dev/null" << error::number( errno ) );
		else if ( result == -1 )
			throw error::runtime( "executable::null_fd: " ) <<
				"Unable to duplicate fd: " << error::number( errno );
	}

	executable::path_tuple executable::path( void )
	{
		c8 path[ PATH_MAX ];

		if ( !executable::path( path, sizeof( path ) ) )
			throw error::runtime( "executable::path: " ) << "Unable to find executable path\n";

		c8* point = std::strrchr( path, '/' );

		if ( !point++ )
			throw error::runtime( "executable::path: " ) << "Unable to find executable path\n";

		return path_tuple( std::string( path, point ), point );
	}
}
