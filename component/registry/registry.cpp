/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "component/registry/registry.hpp"
#include "foundation/ipc/memory/rpc.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

registry::registry( void )
    : client( ipc::local_name( "ooe.registry" ) )
{
}

registry::info_vector registry::find( const interface& interface )
{
    ipc::memory::rpc< info_vector ( const interface::vector_type& ) > registry_find( client, 1 );
    return registry_find( interface.get() );
}

void registry::insert( type flag, const std::string& path )
{
    ipc::memory::rpc< void ( type, const std::string& ) > registry_insert( client, 2 );
    registry_insert( flag, path );
}

std::string registry::surrogate( const std::string& path, bool public_server )
{
    ipc::memory::rpc< std::string ( const std::string&, bool ) > registry_surrogate( client, 3 );
    return registry_surrogate( path, public_server );
}

registry::info_vector registry::list_all( void )
{
    ipc::memory::rpc< info_vector ( void ) > registry_list_all( client, 4 );
    return registry_list_all();
}

OOE_NAMESPACE_END( ( ooe ) )
