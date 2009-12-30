/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_CLIENT_HPP
#define OOE_FOUNDATION_IPC_MEMORY_CLIENT_HPP

#include "foundation/ipc/memory/link.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- client ---------------------------------------------------------------------------------------
class OOE_VISIBLE client
{
public:
	client( const std::string& );
	~client( void );

	operator memory::transport&( void );

private:
	typedef scoped_ptr< memory::transport > transport_ptr;
	typedef scoped_ptr< memory::link_client > link_ptr;

	transport_ptr transport;
	link_ptr link_client;
};

//--- client_data ----------------------------------------------------------------------------------
struct OOE_PACKED client_data
{
	link_t link;
	c8 name[ transport::private_size - sizeof( link_t ) ];
};

//--- disconnect -----------------------------------------------------------------------------------
void disconnect( const std::string&, link_t );

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif	// OOE_FOUNDATION_IPC_MEMORY_CLIENT_HPP
