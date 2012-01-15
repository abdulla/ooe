/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/pool.hpp"
#include "foundation/utility/convert.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

//--- pool -----------------------------------------------------------------------------------------
pool::~pool( void )
{
    for ( map_type::const_iterator i = map.begin(), end = map.end(); i != end; ++i )
        i->second( i->first );
}

void pool::insert( const void* p, function_type f )
{
    map.insert( std::make_pair( p, f ) );
}

void pool::erase( const void* p )
{
    map_type::iterator i = map.find( p );

    if ( i == map.end() )
        throw error::runtime( "pool: " ) << "Unknown pointer: " << ptr( p );

    map.erase( i );
}

bool pool::find( const void* p ) const
{
    return map.find( p ) != map.end();
}

OOE_NAMESPACE_END( ( ooe )( ipc ) )
