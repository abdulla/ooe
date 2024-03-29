/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <iostream>

#include <fcntl.h>

#include "foundation/executable/environment.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/scoped.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

s32 signal_code = 0;

void signal_handler( s32 code, siginfo_t* info, void* )
{
    const c8* description;
    std::string trace;

    switch ( code )
    {
    case OOE_SIGFAULT:
    case SIGABRT:
    case SIGFPE:
        description = strsignal( code );
        trace = stack_trace< 2 >();
        OOE_CONSOLE( "executable: " << "Caught signal " << code << " at " << info->si_addr <<
            ": " << description << "\n\nStack trace:\n" << trace );
        std::exit( EXIT_FAILURE );

    default:
        signal_code = code;
        break;
    }
}

void open_log( const std::string& path )
{
    s32 fd = open( path.c_str(), O_WRONLY | O_CREAT | O_TRUNC, 0600 );

    if ( fd == -1 )
        throw error::runtime( "executable::launch: " ) <<
            "Unable to open log \"" << path <<  "\": " << error::number( errno );

    executable::move_fd( fd, STDERR_FILENO );
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( executable ) )

//--- executable -----------------------------------------------------------------------------------
signal_handler_type signal( struct sigaction& action, signal_handler_type handler, s32 code )
{
    std::memset( &action, 0, sizeof( action ) );
    action.sa_sigaction = handler;
    action.sa_flags = SA_SIGINFO;

    struct sigaction prior;
    std::memset( &prior, 0, sizeof( prior ) );

    if ( sigaction( code, &action, &prior ) )
        OOE_CONSOLE( "executable: " << "Handler for signal " << code << " was not registered" );
    else if ( prior.sa_flags & SA_SIGINFO )
        return prior.sa_sigaction;

    return 0;
}

s32 has_signal( void )
{
    return signal_code;
}

void quit( void )
{
    signal_code = SIGQUIT;
}

s32 copy_fd( s32 fd )
{
    s32 copy = dup( fd );

    if ( copy == -1 )
        throw error::runtime( "executable::copy_fd: " ) <<
            "Unable to copy fd " << fd << ": " << error::number( errno );

    return copy;
}

void move_fd( s32 source, s32 target )
{
    if ( dup2( source, target ) == -1 )
        throw error::runtime( "executable::move_fd: " ) << "Unable to replace fd " << target <<
            " with " << source << ": " << error::number( errno );
    else if ( close( source ) )
        throw error::runtime( "executable::move_fd: " ) <<
            "Unable to close fd " << source << ": " << error::number( errno );
}

s32 null_fd( void )
{
    s32 null = open( "/dev/null", O_RDWR );

    if ( null == -1 )
        throw error::runtime( "executable::null_fd: " ) <<
            "Unable to open /dev/null: " << error::number( errno );

    return null;
}

s32 launch( launch_type launch, s32 argc, c8** argv )
{
    std::set_terminate( __gnu_cxx::__verbose_terminate_handler );
    ::signal( SIGCHLD, SIG_IGN );

    struct sigaction action;
    signal( action, signal_handler, OOE_SIGFAULT );
    signal( action, signal_handler, SIGABRT );
    signal( action, signal_handler, SIGFPE );
    signal( action, signal_handler, SIGTERM );

    s32 status = EXIT_FAILURE;

    try
    {
        path_type path = executable::path();
        open_log( path._0 + "../log/" + path._1 + ".log" );
        up_t page_size = executable::page_size();

        if ( page_size != static_page_size )
            throw error::runtime( "executable::launch: " ) <<
                "Incorrect static page size: " << page_size << " != " << static_page_size;
        else if ( platform::launch( launch, path._0, path._1, argc, argv ) )
            status = EXIT_SUCCESS;
    }
    catch ( error::runtime& error )
    {
        OOE_CONSOLE( "executable: " <<
            "Uncaught exception:\n" << error.what() << "\n\nStack trace:" << error.where() );
    }
    catch ( std::exception& error )
    {
        OOE_CONSOLE( "executable: " << "Uncaught exception:\n" << error.what() );
    }
    catch ( ... )
    {
        OOE_CONSOLE( "executable: " << "Uncaught exception:\nUnknown\n" );
    }

    return status;
}

path_type path( void )
{
    c8 path[ PATH_MAX ];

    if ( !executable::path( path, sizeof( path ) ) )
        throw error::runtime( "executable::path: " ) << "Unable to find executable path\n";

    c8* point = std::strrchr( path, '/' );

    if ( !point++ )
        throw error::runtime( "executable::path: " ) << "Unable to find executable path\n";

    return path_type( std::string( path, point ), point );
}

OOE_NAMESPACE_END( ( ooe )( executable ) )
