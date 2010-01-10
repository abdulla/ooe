/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP
#define OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP

#include "foundation/ipc/switchboard.hpp"
#include "foundation/ipc/memory/link.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- servlet --------------------------------------------------------------------------------------
class servlet
{
public:
	servlet( const std::string&, link_t, const ipc::switchboard&, server& );
	servlet( ooe::socket&, link_t, const ipc::switchboard&, server& );

	void join( void );
	void migrate( ooe::socket&, semaphore&, server& );

private:
	memory::transport transport;
	const link_t link;
	const ipc::switchboard& switchboard;

	scoped_ptr< const memory::link_listen > link_listen;
	scoped_ptr< memory::link_server > link_server;
	atom< bool > state;
	ooe::thread thread;

	void* call( void* );
};

//--- server ---------------------------------------------------------------------------------------
class server
{
public:
	server( const std::string&, const switchboard& ) OOE_VISIBLE;
	~server( void ) OOE_VISIBLE;

	bool decode( void ) OOE_VISIBLE;
	std::string name( void ) const;

	link_t link( pid_t, time_t );
	void unlink( link_t, bool );
	atom_ptr< servlet > find( link_t ) const;

	void relink( ooe::socket& ) OOE_VISIBLE;
	void migrate( ooe::socket& ) OOE_VISIBLE;

private:
	typedef std::map< link_t, atom_ptr< servlet > > servlet_map;

	mutable ipc::semaphore semaphore;
	memory::transport transport;
	const switchboard& external;
	switchboard internal;

	link_t seed;
	servlet_map map;
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

#endif	// OOE_FOUNDATION_IPC_MEMORY_SERVER_HPP
