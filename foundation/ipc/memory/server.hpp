/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP
#define OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP

#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/switchboard.hpp"
#include "foundation/ipc/memory/link.hpp"
#include "foundation/parallel/lock.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

class server;

//--- servlet --------------------------------------------------------------------------------------
class servlet
{
public:
    servlet( link_t, const ipc::switchboard&, const ooe::socket&, server& );
    ~servlet( void );

private:
    const link_t link;
    const ipc::switchboard& switchboard;
    ooe::socket socket;

    atom< transport* > transport_ptr;
    atom< bool > state;
    ooe::thread thread;

    void* main( void* );
};

typedef atom_ptr< servlet > servlet_ptr;

//--- server ---------------------------------------------------------------------------------------
class server
{
public:
    server( const std::string&, const switchboard& ) OOE_VISIBLE;

    bool decode( void ) OOE_VISIBLE;
    std::string name( void ) const;

    link_t link( const std::string& );
    void unlink( link_t );

private:
    typedef std::map< link_t, servlet_ptr > servlet_map;

    ooe::listen listen;
    memory::transport transport;
    const switchboard& external;
    switchboard internal;

    atom< link_t > seed;
    mutable ooe::read_write read_write;
    servlet_map map;
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif  // OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP
