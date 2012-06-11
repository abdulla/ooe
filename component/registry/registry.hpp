/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_REGISTRY_REGISTRY_HPP
#define OOE_COMPONENT_REGISTRY_REGISTRY_HPP

#include "component/registry/interface.hpp"
#include "foundation/ipc/memory/client.hpp"
#include "foundation/ipc/memory/rpc.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

class OOE_VISIBLE registry
{
public:
    enum type
    {
        server,
        library
    };

    typedef tuple< type, std::string > info_tuple;
    typedef std::vector< info_tuple > info_vector;

    registry( void );

    info_vector find( const interface& );
    void insert( type, const std::string& );
    std::string surrogate( const std::string&, bool = false );
    info_vector list_all( void );

private:
    ipc::memory::client client;
    ipc::memory::find_all::result_type result;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_REGISTRY_REGISTRY_HPP
