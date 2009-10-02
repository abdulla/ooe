/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SOCKET_CLIENT_HPP
#define OOE_FOUNDATION_IPC_SOCKET_CLIENT_HPP

#include <map>

#include "foundation/ipc/socket/client_forward.hpp"
#include "foundation/ipc/socket/header.hpp"
#include "foundation/ipc/socket/write_buffer.hpp"
#include "foundation/executable/environment.hpp"
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
		: private platform::ipc::socket::client
	{
	public:
		typedef tuple< array_type, bool > map_tuple;
		typedef std::map< u32, map_tuple > map_type;
		typedef map_type::iterator iterator;
		typedef result_base< void > result_type;

		client( const address& );
		~client( void );

		array_type wait( result_type& );
		void erase( const iterator& );
		iterator insert( void );

		u8* get( void ) const;
		up_t size( void ) const;
		void write( const u8*, up_t );

	private:
		ooe::connect connect;
		mutable u8 buffer[ executable::static_page_size ];

		map_type map;
		u32 in;
		u32 out;
		u32 notify;

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
		typedef client_type::map_type::iterator iterator;

		client_type& client;
		const iterator i;
		state_type state;

		result_base( client_type& client_, const iterator& i_ )
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

		result_base( client_type& client_, const iterator& i_ )
			: result_base< void >( client_, i_ ), value()
		{
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_SOCKET_CLIENT_HPP
