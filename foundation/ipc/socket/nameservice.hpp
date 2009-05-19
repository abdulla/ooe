/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SOCKET_NAMESERVICE_HPP
#define OOE_FOUNDATION_IPC_SOCKET_NAMESERVICE_HPP

#include <typeinfo>

#include "foundation/ipc/socket/switchboard.hpp"

namespace ooe
{
	namespace ipc
	{
		namespace socket
		{
			class nameservice;
		}
	}

//--- ipc::socket::nameservice -------------------------------------------------
	class ipc::socket::nameservice
		: private noncopyable
	{
	public:
		nameservice( void ) OOE_VISIBLE;

		operator const socket::switchboard&( void ) const OOE_VISIBLE;
		u32 find( const std::string&, const std::string& ) const;
		void insert_direct( const std::string&, const std::string&, u32 ) OOE_VISIBLE;

		template< typename type >
			void insert( const std::string& name, type function,
			typename enable_if< is_function_pointer< type > >::type* = 0 )
		{
			typedef typename remove_pointer< type >::type function_type;
			insert_direct( name, typeid( function_type ).name(), switchboard.insert( function ) );
		}

		template< typename type >
			void insert( const std::string& name, type member,
			typename enable_if< is_member_function_pointer< type > >::type* = 0 )
		{
			typedef typename function_of< type >::type signature_type;
			insert_direct( name, typeid( signature_type ).name(), switchboard.insert( member ) );
		}

	private:
		typedef tuple< std::string, std::string > map_tuple;
		typedef std::map< map_tuple, u32 > map_type;

		socket::switchboard switchboard;
		map_type map;
	};
}

#endif	// OOE_FOUNDATION_IPC_SOCKET_NAMESERVICE_HPP
