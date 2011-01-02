/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_LINK_HPP
#define OOE_FOUNDATION_IPC_MEMORY_LINK_HPP

#include "foundation/io/socket.hpp"
#include "foundation/ipc/fundamental.hpp"
#include "foundation/ipc/memory/transport.hpp"
#include "foundation/parallel/thread.hpp"
#include "foundation/utility/atom.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

class server;

//--- link_listen ----------------------------------------------------------------------------------
class link_listen
{
public:
    link_listen( const std::string& );
    ~link_listen( void );

    ooe::socket accept( void ) const;

private:
    std::string path;
    ooe::listen listen;
};

//--- link_server ----------------------------------------------------------------------------------
class link_server
{
public:
    link_server( const ooe::socket&, link_t, server& );
    ~link_server( void );

    void migrate( ooe::socket& );

private:
    ooe::socket socket;
    socket_pair pair;

    const link_t link;
    atom< bool > state;
    ooe::thread thread;

    void* main( void* );
};

//--- link_client ----------------------------------------------------------------------------------
class link_client
{
public:
    link_client( const std::string&, transport& );
    ~link_client( void );

    void shutdown( void );
    operator bool( void ) const;

private:
    ooe::connect connect;
    socket_pair pair;

    atom< bool > state;
    ooe::thread thread;

    void* main( void* );
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif  // OOE_FOUNDATION_IPC_MEMORY_LINK_HPP
