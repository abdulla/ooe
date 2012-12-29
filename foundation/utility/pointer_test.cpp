/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

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

OOE_TEST( 0 )
{
    std::cerr << "scoped_ptr\n";

    bool did_destruct = false;
    {
        scoped_ptr< destruct_test > ptr( new destruct_test( did_destruct ) );
    }
    OOE_CHECK( did_destruct );

    up_t* value = new up_t( 7 );
    scoped_ptr< up_t > ptr( value );
    OOE_CHECK( *ptr == *value );
    OOE_CHECK( ptr == value );
    OOE_CHECK( ptr.get() == value );
    OOE_CHECK( ptr.operator ->() == value );
    OOE_CHECK( ptr.as< up_t >() == value );

    scoped_ptr< up_t > other( value );
    OOE_CHECK( other.release() == value );

    up_t* reset = new up_t( 8 );
    ptr.reset( reset );
    OOE_CHECK( ptr.get() == reset ) << "ptr.reset( reset ) == reset";

    ptr.reset();
    OOE_CHECK( !ptr.get() ) << "ptr.reset() == 0";
}

OOE_TEST( 1 )
{
    std::cerr << "shared_ptr\n";

    bool did_destruct = false;
    {
        shared_ptr< destruct_test > ptr( new destruct_test( did_destruct ) );
    }
    OOE_CHECK( did_destruct );

    up_t* value = new up_t( 7 );
    shared_ptr< up_t > ptr( value );
    OOE_CHECK( *ptr == *value );
    OOE_CHECK( ptr == value );
    OOE_CHECK( ptr.get() == value );
    OOE_CHECK( ptr.operator ->() == value );
    OOE_CHECK( ptr.as< up_t >() == value );

    {
        shared_ptr< up_t > copy( ptr );
        OOE_CHECK( copy.get() == value );
    }
    OOE_CHECK( ptr.get() == value ) << "copy: ptr.get() == value";

    up_t* reset = new up_t( 8 );
    ptr.reset( reset );
    OOE_CHECK( ptr.get() == reset ) << "ptr.reset( reset ) == reset";

    ptr.reset();
    OOE_CHECK( !ptr.get() ) << "ptr.reset() == 0";
}

OOE_TEST( 2 )
{
    std::cerr << "opaque_ptr\n";

    bool did_destruct = false;
    {
        opaque_ptr ptr( new destruct_test( did_destruct ) );
    }
    OOE_CHECK( did_destruct );

    up_t* value = new up_t( 7 );
    opaque_ptr ptr( value );
    OOE_CHECK( ptr == value );
    OOE_CHECK( ptr.get() == value );
    OOE_CHECK( ptr.as< up_t >() == value );

    {
        opaque_ptr copy( ptr );
        OOE_CHECK( copy.get() == value );
    }
    OOE_CHECK( ptr.get() == value ) << "copy: ptr.get() == value";

    up_t* reset = new up_t( 8 );
    ptr.reset( reset );
    OOE_CHECK( ptr.get() == reset ) << "ptr.reset( reset ) == reset";
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
