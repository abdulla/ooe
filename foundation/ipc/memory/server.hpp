/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP
#define OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP

#include "foundation/ipc/semaphore.hpp"
#include "foundation/ipc/switchboard.hpp"
#include "foundation/ipc/memory/link.hpp"
#include "foundation/parallel/lock.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- servlet --------------------------------------------------------------------------------------
class servlet
{
public:
    servlet( const std::string&, link_t, const ipc::switchboard&, server& );
    servlet( ooe::socket&, link_t, const ipc::switchboard&, server& );

    void join( void );
    void migrate( ooe::socket&, server& );
    void check( const void* );

private:
    typedef scoped_ptr< const memory::link_listen > link_listen_ptr;
    typedef scoped_ptr< memory::link_server > link_server_ptr;

    memory::transport transport;
    const link_t link;
    const ipc::switchboard& switchboard;

    link_listen_ptr link_listen;
    link_server_ptr link_server;

    shared_allocator allocator;
    io_buffer buffer;

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
    ~server( void ) OOE_VISIBLE;

    bool decode( void ) OOE_VISIBLE;
    std::string name( void ) const;

    link_t link( pid_t, time_t );
    void unlink( link_t );
    servlet_ptr find( link_t ) const;

    void migrate( ooe::socket& ) OOE_VISIBLE;
    void relink( ooe::socket& ) OOE_VISIBLE;

private:
    typedef std::map< link_t, servlet_ptr > servlet_map;

    ipc::semaphore semaphore;
    memory::transport transport;
    const switchboard& external;
    switchboard internal;

    atom< link_t > seed;
    mutable ooe::read_write read_write;
    servlet_map map;
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif  // OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP
