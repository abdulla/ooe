/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP
#define OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP

#include "foundation/ipc/switchboard.hpp"
#include "foundation/ipc/memory/link.hpp"
#include "foundation/parallel/lock.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- servlet --------------------------------------------------------------------------------------
class servlet
{
public:
    servlet( const ooe::socket&, const ipc::switchboard&, servlet_iterator, server& );
    ~servlet( void );

    void detach( void );

private:
    ooe::socket socket;
    const ipc::switchboard& switchboard;
    const servlet_iterator iterator;

    ooe::thread thread;

    void main( void* );
};

//--- server ---------------------------------------------------------------------------------------
class OOE_VISIBLE server
{
public:
    server( const ipc::switchboard& );
    ~server( void );

    void insert( const socket& );
    void erase( servlet_iterator ) OOE_HIDDEN;

private:
    const ipc::switchboard& switchboard;

    ooe::mutex mutex;
    servlet_list list;
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif  // OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP
