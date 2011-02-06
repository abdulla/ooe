/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/io/directory.hpp"
#include "foundation/io/error.hpp"
#include "foundation/io/vfs.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

// tuple_type: _0 -> external "physical" path, _1 -> internal "virtual" path

std::string crimp_slashes( const std::string& string )
{
    up_t i = string.find_first_not_of( "/" );

    if ( i == std::string::npos )
        return std::string();

    up_t j = string.find_last_not_of( "/" ) + 1;
    return std::string( string, i, j - i );
}

bool lru( const std::string& path, vfs::list_type& list, vfs::list_type::iterator& i )
{
    if ( !exists( path.c_str() ) )
        return false;

    list.splice( list.begin(), list, i );
    return true;
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- vfs ------------------------------------------------------------------------------------------
vfs::vfs( void )
    : list()
{
}

void vfs::insert( const std::string& external, const std::string& internal )
{
    const std::string path_external( external, 0, external.find_last_not_of( "/" ) + 1 );
    const std::string path_internal( crimp_slashes( internal ) );
    list.push_front( tuple_type( path_external, path_internal ) );
}

bool vfs::empty( void ) const
{
    return list.empty();
}

descriptor vfs::operator []( const std::string& in ) const
{
    std::string path( crimp_slashes( in ) );

    for ( list_type::iterator i = list.begin(), end = list.end(); i != end; ++i )
    {
        const std::string& external = i->_0;
        const std::string& internal = i->_1;
        u32 size = internal.size();

        if ( size && !path.compare( 0, size, internal ) )
            path = path.substr( size );

        std::string full = external + '/' + path;

        if ( !lru( full, list, i ) )
            continue;

        return descriptor( full );
    }

    throw error::io( "vfs: " ) << "Unable to find \"" << in << '\"';
}

OOE_NAMESPACE_END( ( ooe ) )
