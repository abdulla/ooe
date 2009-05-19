/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SOCKET_CLIENT_HPP
#define OOE_FOUNDATION_IPC_SOCKET_CLIENT_HPP

#include <map>

#include "foundation/io/socket.hpp"
#include "foundation/ipc/socket/header.hpp"
#include "foundation/parallel/lock.hpp"
#include "foundation/parallel/thread.hpp"

namespace ooe
{
	namespace ipc
	{
		namespace socket
		{
			class client;

			template< typename >
				struct result_base;

			template<>
				struct result_base< void >;
		}
	}

//--- ipc::socket::client ------------------------------------------------------
	class OOE_VISIBLE ipc::socket::client
	{
	public:
		typedef tuple< array_type, bool > map_tuple;
		typedef std::map< u32, map_tuple > map_type;
		typedef map_type::iterator iterator_type;
		typedef result_base< void > result_type;

		client( const address& );
		~client( void );

		array_type wait( result_type& );
		void erase( const iterator_type& );
		iterator_type insert( void );

		buffer_tuple get( void ) const;
		void write( const void*, up_t );

	private:
		ooe::connect connect;
		u8 buffer[ executable::static_page_size ];

		map_type map;
		u32 in;
		u32 out;

		ooe::mutex mutex;
		ooe::condition condition;
		ooe::thread thread;

		void* call( void* ) OOE_HIDDEN;
	};

//--- ipc::socket::result_base -------------------------------------------------
	template<>
		struct OOE_VISIBLE ipc::socket::result_base< void >
		: private noncopyable
	{
		enum state_type
		{
			wait,
			done,
			error
		};

		typedef socket::client client_type;
		typedef client_type::map_type::iterator iterator_type;

		client_type& client;
		const iterator_type i;
		state_type state;

		result_base( client_type& client_, const iterator_type& i_ )
			: client( client_ ), i( i_ ), state( wait )
		{
		}

		~result_base( void )
		{
			if ( done == wait )
				client.erase( i );
		}
	};

	template< typename type >
		struct ipc::socket::result_base
		: public result_base< void >
	{
		type value;

		result_base( client_type& client_, const iterator_type& i_ )
			: result_base< void >( client_, i_ ), value()
		{
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_SOCKET_CLIENT_HPP
