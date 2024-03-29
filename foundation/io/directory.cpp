/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <cerrno>
#include <climits>
#include <cstdlib>
#include <iostream>

#include <sys/stat.h>

#include "foundation/io/directory.hpp"
#include "foundation/io/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

const descriptor& check( const descriptor& desc )
{
    if ( desc.type() != descriptor::directory )
        throw error::io( "directory: " ) << "Descriptor is not a directory";

    return desc;
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- directory_iterator ---------------------------------------------------------------------------
directory_iterator::directory_iterator( DIR* dir_, dirent* entry_ )
    : dir( dir_ ), entry( entry_ )
{
}

void directory_iterator::increment( void )
{
    entry = readdir( dir );
}

bool directory_iterator::equal( const directory_iterator& other ) const
{
    return entry == other.entry;
}

std::string directory_iterator::dereference( void ) const
{
    return entry->d_name;
}

//--- directory ------------------------------------------------------------------------------------
directory::directory( const descriptor& desc )
    : dir( platform::directory_open( check( desc ) ) )
{
    if ( !dir )
        throw error::io( "directory: " ) << "Unable to open directory: " << error::number( errno );
}

directory::~directory( void )
{
    if ( closedir( dir ) )
        OOE_CONSOLE( "directory: " << "Unable to close directory: " << error::number( errno ) );
}

directory::iterator directory::begin( void ) const
{
    rewinddir( dir );
    return iterator( dir, readdir( dir ) );
}

directory::iterator directory::end( void ) const
{
    return iterator( 0, 0 );
}

//--- exists ---------------------------------------------------------------------------------------
bool exists( const std::string& path )
{
    return !access( path.c_str(), F_OK );
}

//--- erase ----------------------------------------------------------------------------------------
void erase( const std::string& path )
{
    if ( unlink( path.c_str() ) )
        throw error::io( "erase: " ) <<
            "Unable to erase \"" << path << "\": " << error::number( errno );
}

//--- canonical_path -------------------------------------------------------------------------------
std::string canonical_path( const std::string& path )
{
    c8 buffer[ PATH_MAX ];

    if ( !realpath( path.c_str(), buffer ) )
        throw error::io( "canonical_path: " ) <<
            "Unable to canonicalise \"" << path << "\": " << error::number( errno );

    return buffer;
}

//--- make_directory -------------------------------------------------------------------------------
void make_directory( const std::string& path )
{
    if ( mkdir( path.c_str(), 0700 ) )
        throw error::io( "make_directory: " ) <<
            "Unable to make directory \"" << path << "\": " << error::number( errno );
}

OOE_NAMESPACE_END( ( ooe ) )
