/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "component/registry/registry.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef registry::info_vector ( find_type )( const interface::vector_type& );
typedef void ( insert_type )( registry::type, const std::string& );
typedef std::string ( surrogate_type )( const std::string&, bool );
typedef registry::info_vector ( list_all_type )( void );

ipc::memory::find_all::result_type registry_find_all( ipc::memory::client& client )
{
    ipc::memory::find_all::parameter_type parameter;
    parameter.reserve( 4 );
    parameter.push_back( make_tuple( "find", typeid( find_type ).name() ) );
    parameter.push_back( make_tuple( "insert", typeid( insert_type ).name() ) );
    parameter.push_back( make_tuple( "surrogate", typeid( surrogate_type ).name() ) );
    parameter.push_back( make_tuple( "list_all", typeid( list_all_type ).name() ) );
    return ipc::memory::find_all( client )( parameter );
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

registry::registry( void )
    : client( ipc::local_name( "ooe.registry" ) ), result( registry_find_all( client ) )
{
}

registry::info_vector registry::find( const interface& interface )
{
    ipc::memory::rpc< find_type > registry_find( client, result[ 0 ] );
    return registry_find( interface.get() );
}

void registry::insert( type flag, const std::string& path )
{
    ipc::memory::rpc< insert_type > registry_insert( client, result[ 1 ] );
    registry_insert( flag, path );
}

std::string registry::surrogate( const std::string& path, bool public_server )
{
    ipc::memory::rpc< surrogate_type > registry_surrogate( client, result[ 2 ] );
    return registry_surrogate( path, public_server );
}

registry::info_vector registry::list_all( void )
{
    ipc::memory::rpc< list_all_type > registry_list_all( client, result[ 3 ] );
    return registry_list_all();
}

OOE_NAMESPACE_END( ( ooe ) )
