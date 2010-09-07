/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/ipc/nameservice.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

void function_1( u8 )
{
}

void function_2( u16 )
{
}

void function_3( u32 )
{
}

typedef unit::group< anonymous_t, anonymous_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "upgrade" );

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
	void fixture_type::test< 0 >( anonymous_t& )
{
	std::cerr << "test nameservice upgrade\n";

	ipc::nameservice ns1;
	ns1.insert( "function_1", function_1, "doc 1" );
	ns1.insert( "function_2", function_2, "doc 2" );

	ipc::nameservice ns2;
	ns2.insert( "function_2", function_2, "doc 2" );
	ns2.insert( "function_3", function_3, "doc 3" );

	ipc::nameservice::list_type l1;
	l1.push_back( make_tuple( "function_1", typeid( function_1 ).name() ) );
	l1.push_back( make_tuple( "function_2", typeid( function_2 ).name() ) );
	l1.push_back( make_tuple( "function_3", typeid( function_3 ).name() ) );

	ns2.upgrade( ns1.list() );
	ipc::nameservice::list_type l2 = ns2.list();
	OOE_CHECK( "list size " << l2.size() << " == 3", l2.size() == 3 );

	std::vector< std::string > docs;
	docs.push_back( std::string() );
	docs.push_back( "doc 2" );
	docs.push_back( "doc 3" );

	for ( up_t i = 0, end = l2.size(), index_adjust = 5; i != end; ++i )
	{
		const ipc::nameservice::tuple_type& name = l2[ i ];
		ipc::index_t index = ns2.find( name._0, name._1 );
		std::string doc = ns2.doc( name._0, name._1 );

		OOE_CHECK( "index: " << index << " == " << i + index_adjust, index == i + index_adjust );
		OOE_CHECK( "name: " << l1[ i ] << " == " << l2[ i ], l1[ i ] == l2[ i ] );
		OOE_CHECK( "doc: " << doc << " == " << docs[ i ], doc == docs[ i ] );
	}

	const ipc::switchboard& sw = ns2;
	OOE_CHECK( "sw[ 5 ] != function_1", sw[ 5 ]._1.function != any( function_1 ).function );
	OOE_CHECK( "sw[ 6 ] == function_2", sw[ 6 ]._1.function == any( function_2 ).function );
	OOE_CHECK( "sw[ 7 ] == function_3", sw[ 7 ]._1.function == any( function_3 ).function );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
