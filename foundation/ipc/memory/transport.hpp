/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_HPP
#define OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_HPP

#include "foundation/io/socket.hpp"
#include "foundation/ipc/shared_memory.hpp"
#include "foundation/ipc/memory/transport_forward.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- transport ------------------------------------------------------------------------------------
class OOE_VISIBLE transport
    : private platform::ipc::memory::transport
{
public:
    typedef void ( * wait_type )( const void* );

    transport( const std::string& );
    transport( ooe::socket& );
    ~transport( void );

    void wait( wait_type, const void* );
    void notify( void );
    void wake_wait( void );
    void wake_notify( void );

    u8* get( void ) const;
    up_t size( void ) const;
    std::string name( void ) const;
    bool in_canary( const void* ) const;

    void unlink( void );
    void send( ooe::socket& );

private:
    shared_memory memory;
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif  // OOE_FOUNDATION_IPC_MEMORY_TRANSPORT_HPP
