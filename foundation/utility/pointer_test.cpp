/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

class data
{
protected:
    class destruct_test;
};

class data::destruct_test
{
public:
    destruct_test( bool& state_ )
        : state( state_ )
    {
    }

    ~destruct_test( void )
    {
        state = true;
    }

private:
    bool& state;
};

typedef unit::group< anonymous_t, data, 3 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "pointer" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "scoped_ptr\n";

    bool did_destruct = false;
    {
        scoped_ptr< destruct_test > ptr( new destruct_test( did_destruct ) );
    }
    OOE_CHECK( "destruct_test::~destruct_test()", did_destruct );

    up_t* value = new up_t( 7 );
    scoped_ptr< up_t > ptr( value );
    OOE_CHECK( "*ptr == *value", *ptr == *value );
    OOE_CHECK( "ptr == value", ptr == value );
    OOE_CHECK( "ptr.get() == value", ptr.get() == value );
    OOE_CHECK( "ptr.operator ->() == value", ptr.operator ->() == value );
    OOE_CHECK( "ptr.as< up_t >() == value", ptr.as< up_t >() == value );

    scoped_ptr< up_t > other( value );
    OOE_CHECK( "other.release() == value", other.release() == value );

    ptr.swap( other );
    OOE_CHECK( "ptr.get() == 0", ptr.get() == 0 );
    OOE_CHECK( "other.get() == value", other.get() == value );

    other.swap( ptr );
    OOE_CHECK( "ptr.get() == value", ptr.get() == value );
    OOE_CHECK( "other.get() == 0", other.get() == 0 );
}

OOE_TEST void fixture_type::test< 1 >( anonymous_t& )
{
    std::cerr << "shared_ptr\n";

    bool did_destruct = false;
    {
        shared_ptr< destruct_test > ptr( new destruct_test( did_destruct ) );
    }
    OOE_CHECK( "destruct_test::~destruct_test()", did_destruct );

    up_t* value = new up_t( 7 );
    shared_ptr< up_t > ptr( value );
    OOE_CHECK( "*ptr == *value", *ptr == *value );
    OOE_CHECK( "ptr == value", ptr == value );
    OOE_CHECK( "ptr.get() == value", ptr.get() == value );
    OOE_CHECK( "ptr.operator ->() == value", ptr.operator ->() == value );
    OOE_CHECK( "ptr.as< up_t >() == value", ptr.as< up_t >() == value );

    shared_ptr< up_t > other;
    ptr.swap( other );
    OOE_CHECK( "ptr.get() == 0", ptr.get() == 0 );
    OOE_CHECK( "other.get() == value", other.get() == value );

    other.swap( ptr );
    OOE_CHECK( "ptr.get() == value", ptr.get() == value );
    OOE_CHECK( "other.get() == 0", other.get() == 0 );

    {
        shared_ptr< up_t > copy( ptr );
        OOE_CHECK( "copy.get() == value", copy.get() == value );
    }
    OOE_CHECK( "copy: ptr.get() == value", ptr.get() == value );
}

OOE_TEST void fixture_type::test< 2 >( anonymous_t& )
{
    std::cerr << "opaque_ptr\n";

    bool did_destruct = false;
    {
        opaque_ptr ptr( new destruct_test( did_destruct ) );
    }
    OOE_CHECK( "destruct_test::~destruct_test()", did_destruct );

    up_t* value = new up_t( 7 );
    opaque_ptr ptr( value );
    OOE_CHECK( "ptr == value", ptr == value );
    OOE_CHECK( "ptr.get() == value", ptr.get() == value );
    OOE_CHECK( "ptr.as< up_t >() == value", ptr.as< up_t >() == value );

    opaque_ptr other( static_cast< up_t* >( 0 ) );
    ptr.swap( other );
    OOE_CHECK( "ptr.get() == 0", ptr.get() == 0 );
    OOE_CHECK( "other.get() == value", other.get() == value );

    other.swap( ptr );
    OOE_CHECK( "ptr.get() == value", ptr.get() == value );
    OOE_CHECK( "other.get() == 0", other.get() == 0 );

    {
        opaque_ptr copy( ptr );
        OOE_CHECK( "copy.get() == value", copy.get() == value );
    }
    OOE_CHECK( "copy: ptr.get() == value", ptr.get() == value );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )