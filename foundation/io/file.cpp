/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include <fcntl.h>
#include <unistd.h>

#include "foundation/io/error.hpp"
#include "foundation/io/file.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

const descriptor& check( const descriptor& desc )
{
    descriptor::node_type type = desc.type();

    if ( type != descriptor::file && type != descriptor::pipe && type != descriptor::character )
        throw error::io( "file: " ) << "Descriptor is not a file, pipe or character device";

    return desc;
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- file -----------------------------------------------------------------------------------------
file::file( const descriptor& desc )
    : descriptor( check( desc ) )
{
}

up_t file::read( void* buffer, up_t bytes )
{
    sp_t read_ = ::read( get(), buffer, bytes );

    if ( read_ == -1 )
        throw error::io( "file: " ) << "Unable to read: " << error::number( errno );

    return read_;
}

up_t file::write( const void* buffer, up_t bytes )
{
    sp_t wrote = ::write( get(), buffer, bytes );

    if ( wrote == -1 )
        throw error::io( "file: " ) << "Unable to write: " << error::number( errno );

    return wrote;
}

up_t file::tell( void ) const
{
    sp_t told = static_cast< s32 >( lseek( get(), 0, SEEK_CUR ) );

    if ( told == -1 )
        throw error::io( "file: " ) << "Unable to tell: " << error::number( errno );

    return told;
}

void file::seek( sp_t offset, seek_type point )
{
    s32 whence;

    switch ( point )
    {
    case current:
        whence = SEEK_CUR;
        break;

    case begin:
        whence = SEEK_SET;
        break;

    case end:
        whence = SEEK_END;
        break;

    default:
        throw error::io( "file: " ) << "Unknown seek type: " << point;
    }

    if ( lseek( get(), offset, whence ) == -1 )
        throw error::io( "file: " ) << "Unable to seek: " << error::number( errno );
}

//--- make_pipe ------------------------------------------------------------------------------------
file_pair make_pipe( void )
{
    s32 fd[ 2 ];

    if ( pipe( fd ) )
        throw error::io( "make_pipe: " ) << "Unable to create pipe: " << error::number( errno );

    return file_pair( descriptor( fd[ 0 ] ), descriptor( fd[ 1 ] ) );
}

OOE_NAMESPACE_END( ( ooe ) )
