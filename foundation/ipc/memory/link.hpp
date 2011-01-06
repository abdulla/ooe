/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_LINK_HPP
#define OOE_FOUNDATION_IPC_MEMORY_LINK_HPP

#include <list>

#include "foundation/ipc/fundamental.hpp"
#include "foundation/ipc/memory/transport.hpp"
#include "foundation/parallel/thread.hpp"
#include "foundation/utility/atom.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

class servlet;
class server;

typedef shared_ptr< servlet > servlet_ptr;
typedef std::list< servlet_ptr > servlet_list;
typedef std::list< servlet_ptr >::iterator servlet_iterator;

//--- link_server ----------------------------------------------------------------------------------
class link_server
{
public:
    link_server( const ooe::socket&, servlet_iterator, server&, atom< bool >&, transport& );

private:
    ooe::socket socket;

    const servlet_iterator iterator;
    memory::server& server;

    atom< bool >& state;
    ooe::thread thread;

    void* main( void* );
};

//--- link_client ----------------------------------------------------------------------------------
class link_client
{
public:
    link_client( const ooe::socket&, transport& );
    ~link_client( void );

    operator bool( void ) const;

private:
    ooe::socket socket;

    atom< bool > state;
    ooe::thread thread;

    void* main( void* );
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif  // OOE_FOUNDATION_IPC_MEMORY_LINK_HPP
