/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SOCKET_CLIENT_HPP
#define OOE_FOUNDATION_IPC_SOCKET_CLIENT_HPP

#include <map>

#include "foundation/io/socket.hpp"
#include "foundation/parallel/lock.hpp"
#include "foundation/parallel/thread.hpp"

#include "external/nipc/header.hpp"

namespace ooe
{
	namespace nipc
	{
		class client;

		template< typename >
			struct result_base;

		template<>
			struct result_base< void >;
	}

//--- nipc::client -------------------------------------------------------------
	class OOE_VISIBLE nipc::client
	{
	public:
		typedef tuple< array_type, bool > map_tuple;
		typedef std::map< u32, map_tuple > map_type;

		client( const address& );
		~client( void );

		array_type wait( result_base< void >& );
		void erase( const map_type::iterator& );
		map_type::iterator insert( void );

		operator socket&( void );

	private:
		ooe::connect connect;
		map_type map;
		u32 in;
		u32 out;

		ooe::mutex mutex;
		ooe::condition condition;
		ooe::thread thread;

		void* call( void* ) OOE_HIDDEN;
	};

//--- nipc::result_base --------------------------------------------------------
	template<>
		struct OOE_VISIBLE nipc::result_base< void >
		: private noncopyable
	{
		enum state_type
		{
			wait,
			done,
			error
		};

		nipc::client& client;
		nipc::client::map_type::iterator i;
		state_type state;

		result_base( nipc::client& client_, const nipc::client::map_type::iterator& i_ )
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
		struct nipc::result_base
		: public result_base< void >
	{
		type value;

		result_base( nipc::client& client_, const nipc::client::map_type::iterator& i_ )
			: result_base< void >( client_, i_ ), value()
		{
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_SOCKET_CLIENT_HPP
