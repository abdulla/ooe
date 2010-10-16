/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>
#include <vector>

#include <cerrno>
#include <csignal>
#include <cstdarg>
#include <cstdlib>

#include <sys/wait.h>

#include "foundation/executable/fork_io.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

void replace( s32 index, s32 fd )
{
    if ( dup2( fd, index ) == -1 )
        throw error::runtime( "fork_io: " ) << "Unable to replace fd " << index << " with " << fd;
    else if ( close( fd ) )
        throw error::runtime( "fork_io: " ) << "Unable to close fd " << fd;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- fork_id --------------------------------------------------------------------------------------
fork_id::fork_id( void )
    : pid( fork() )
{
    if ( pid == -1 )
        throw error::runtime( "fork_io: " ) << "Unable to fork process: " << error::number( errno );
}

fork_id::fork_id( const io_triplet& io )
    : pid( fork() )
{
    if ( pid == -1 )
        throw error::runtime( "fork_io: " ) << "Unable to fork process: " << error::number( errno );
    else if ( pid )
        return;

    if ( io._0 != -1 )
        replace( STDIN_FILENO, io._0 );

    if ( io._1 != -1 )
        replace( STDOUT_FILENO, io._1 );

    if ( io._2 != -1 )
        replace( STDERR_FILENO, io._2 );
}

//--- fork_io --------------------------------------------------------------------------------------
fork_io::fork_io( void )
    : id( new fork_id )
{
}

fork_io::fork_io( const io_triplet& io )
    : id( new fork_id( io ) )
{
}

// required to ensure fork_id can be hidden
fork_io::~fork_io( void )
{
}

fork_io::wait_type fork_io::wait( bool block ) const
{
    s32 status;
    s32 pid = waitpid( id->pid, &status, block ? 0 : WNOHANG );

    if ( !pid )
        return waiting;
    else if ( pid == id->pid )
    {
        if ( WIFEXITED( status ) )
            return WEXITSTATUS( status ) ? failure : success;
        else if ( WIFSIGNALED( status ) )
            return failure;
        else if ( WIFSTOPPED( status ) || WIFCONTINUED( status ) )
            return waiting;
    }

    throw error::runtime( "fork_io: " ) << "Unable to wait for process " << id->pid <<
        " with status " << status << ": " << error::number( errno );
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
        OOE_CONSOLE( "fork_io: " <<
            "Unable to execute process \"" << path << "\": " << error::number( errno ) );
    }
    catch ( std::exception& error )
    {
        OOE_CONSOLE( "fork_io: " <<
            "Unable to execute process \"" << path << "\": " << error.what() );
    }

    exit( false );
}

void fork_io::exit( bool success )
{
    _exit( success ? EXIT_SUCCESS : EXIT_FAILURE );
}

//--- scoped_fork ----------------------------------------------------------------------------------
scoped_fork::scoped_fork( void )
    : fork_io()
{
}

scoped_fork::scoped_fork( const io_triplet& io )
    : fork_io( io )
{
}

scoped_fork::~scoped_fork( void )
{
    signal( SIGKILL );
}

OOE_NAMESPACE_END( ( ooe ) )
