/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_TEST_UNIT_GROUP_HPP
#define OOE_TEST_UNIT_GROUP_HPP

#include <iostream>
#include <vector>

#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/pointer.hpp"
#include "foundation/utility/tuple.hpp"
#include "test/unit/runner.hpp"

#define OOE_TEST template<> template<>

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template< typename, up_t >
    struct insert_test;

//--- fixture --------------------------------------------------------------------------------------
template< typename setup, typename data >
    struct fixture
    : private data
{
    template< up_t i >
        void test( setup& )
    {
        std::cerr << "Fix test group: Test " << i << " is empty\n";
        fail();
    }
};

//--- group_base -----------------------------------------------------------------------------------
class group_base
{
public:
    typedef void ( * function_type )( void* );
    typedef std::vector< function_type > vector_type;
    typedef vector_type::const_iterator const_iterator;

    virtual ~group_base( void ) {}
    virtual opaque_ptr create_setup( void ) = 0;

    const_iterator begin( void ) const;
    const_iterator end( void ) const;
    void insert( function_type ) OOE_VISIBLE;

private:
    vector_type vector;
};

//--- group ----------------------------------------------------------------------------------------
template< typename setup, typename data, up_t size >
    class group
    : public group_base
{
public:
    typedef setup setup_type;
    typedef fixture< setup_type, data > fixture_type;

    group( const std::string& name, runner& runner = global_runner )
        : group_base()
    {
        insert_test< group, size >::call( *this );
        runner.insert( name, *this );
    }

private:
    virtual opaque_ptr create_setup( void )
    {
        return opaque_ptr( new setup_type );
    }
};

//--- invoke_test ----------------------------------------------------------------------------------
template< typename group, void ( group::fixture_type::* member )( typename group::setup_type& ) >
    void invoke_test( void* pointer )
{
    typename group::fixture_type instance;
    ( instance.*member )( *static_cast< typename group::setup_type* >( pointer ) );
}

//--- insert_test ----------------------------------------------------------------------------------
template< typename group, up_t i >
    struct insert_test
{
    static void call( group_base& base )
    {
        insert_test< group, i - 1 >::call( base );
        base.insert( invoke_test< group, &group::fixture_type::template test< i - 1 > > );
    }
};

template< typename group >
    struct insert_test< group, 0 >
{
    static void call( group_base& )
    {
    }
};

OOE_NAMESPACE_END( ( ooe )( unit ) )

#endif  // OOE_TEST_UNIT_GROUP_HPP
