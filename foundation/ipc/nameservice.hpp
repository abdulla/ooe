/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_NAMESERVICE_HPP
#define OOE_FOUNDATION_IPC_NAMESERVICE_HPP

#include "foundation/ipc/switchboard.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

//--- nameservice ----------------------------------------------------------------------------------
class nameservice
	: private noncopyable
{
public:
	typedef tuple< std::string, std::string > map_tuple;
	typedef std::vector< map_tuple > list_type;

	nameservice( void ) OOE_VISIBLE;

	operator const ipc::switchboard&( void ) const OOE_VISIBLE;
	index_t find( const std::string&, const std::string& ) const;
	list_type list( void ) const;

	void insert_direct( const std::string&, const std::string&, index_t ) OOE_VISIBLE;
	void insert_direct( const std::string&, const std::string&, switchboard::call_type, any )
		OOE_VISIBLE;

	template< typename t >
		void insert( const std::string& name, t function,
		typename enable_if< is_function_pointer< t > >::type* = 0 )
	{
		typedef typename remove_pointer< t >::type function_type;
		insert_direct( name, typeid( function_type ).name(), switchboard.insert( function ) );
	}

	template< typename t >
		void insert( const std::string& name, t member,
		typename enable_if< is_member_function_pointer< t > >::type* = 0 )
	{
		typedef typename function_of< t >::type member_type;
		insert_direct( name, typeid( member_type ).name(), switchboard.insert( member ) );
	}

private:
	typedef std::map< map_tuple, index_t > map_type;

	ipc::switchboard switchboard;
	map_type map;
};

OOE_NAMESPACE_END( ( ooe )( ipc ) )

#endif	// OOE_FOUNDATION_IPC_NAMESERVICE_HPP