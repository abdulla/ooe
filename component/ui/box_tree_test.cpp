/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "component/ui/box_tree.hpp"
#include "foundation/math/math.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

//--- check_tree -----------------------------------------------------------------------------------
void check_tree( const box_tree& tree, const box_tree::iterator& i, u16 width, u16 x )
{
    OOE_CHECK( "i != tree.end()", i != tree.end() );

    ooe::box box = i->box();
    OOE_CHECK( box.width << " == " << width, box.width == width );
    OOE_CHECK( box.height << " == " << width, box.height == width );
    OOE_CHECK( box.x << " == " << x, box.x == x );
    OOE_CHECK( box.y << " == " << x, box.y == x );
}

//--- check_size -----------------------------------------------------------------------------------
void check_size( const box_tree::layer_vector& layer, up_t size )
{
    OOE_CHECK( layer.size() << " == " << size, layer.size() == size );

    for ( box_tree::layer_vector::const_iterator i = layer.begin(), end = layer.end();
        i != end; ++i )
        OOE_CHECK( i->size() << " == 1", i->size() == 1 );
}

//--- check_data -----------------------------------------------------------------------------------
void check_data( const box_tree::data_tuple& data, f32 width, f32 x )
{
    OOE_CHECK( data._0 << " == " << width, is_equal( data._0, width ) );
    OOE_CHECK( data._1 << " == " << width, is_equal( data._1, width ) );
    OOE_CHECK( data._2 << " == " << x, is_equal( data._2, x ) );
    OOE_CHECK( data._3 << " == " << x, is_equal( data._3, x ) );
}

//--- data -----------------------------------------------------------------------------------------
class data
{
public:
    data( void )
        : colour( 255, 255, 255, 255 ), pointer( static_cast< s32* >( 0 ) )
    {
    }

protected:
    ooe::colour colour;
    opaque_ptr pointer;
};

typedef unit::group< anonymous_t, data, 3 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "box_tree" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "box_tree successive insert";

    box_tree tree( box( 1 << 10, 1 << 10, 0, 0 ), colour, pointer );
    box_tree::layer_vector layer = tree.view( 1 << 11, 1 << 11, 0, 0, 0 );
    check_size( layer, 1 );
    check_data( layer[ 0 ][ 0 ]._0, 1 << 10, 0 );
    box_tree* leaf = &tree;

    // insert successive sub-trees and ensure they've been inserted correctly
    for ( up_t i = 0; i != 5; ++i )
    {
        box_tree::iterator iterator =
            leaf->insert( box( 1 << 10, 1 << 10, 1 << 9, 1 << 9 ), colour, pointer );
        check_tree( *leaf, iterator, 1 << 10, 1 << 9 );
        leaf = &*iterator;
        layer = tree.view( 1 << 11, 1 << 11, 0, 0, 0 );
        check_size( layer, i + 2 );
        check_data( layer[ 0 ][ 0 ]._0, 1 << 10, 0 );
        u32 total = 0;

        for ( up_t j = 1; j != i + 2; ++j )
        {
            total += 1 << ( 9 - j );
            check_data( layer[ j ][ 0 ]._0, 1 << ( 10 - j ), total );
        }
    }
}

OOE_TEST void fixture_type::test< 1 >( anonymous_t& )
{
    std::cerr << "box_tree insert at offset";

    box_tree tree( box( 1 << 10, 1 << 10, 0, 0 ), colour, pointer );
    box_tree::layer_vector layer = tree.view( 1 << 11, 1 << 11, 0, 0, 0 );
    check_size( layer, 1 );
    check_data( layer[ 0 ][ 0 ]._0, 1 << 10, 0 );

    box_tree::iterator i = tree.insert( box( 1 << 10, 1 << 10, 1 << 9, 1 << 9 ), colour, pointer );
    check_tree( tree, i, 1 << 10, 1 << 9 );
    layer = tree.view( 1 << 11, 1 << 11, 0, 0, 0 );
    check_size( layer, 2 );
    check_data( layer[ 0 ][ 0 ]._0, 1 << 10, 0 );
    check_data( layer[ 1 ][ 0 ]._0, 1 << 9, 1 << 8 );

    box_tree::iterator j = i->insert( box( 1 << 10, 1 << 10, 0, 0 ), colour, pointer );
    check_tree( *i, j, 1 << 10, 0 );
    layer = tree.view( 1 << 11, 1 << 11, 0, 0, 0 );
    check_size( layer, 3 );
    check_data( layer[ 0 ][ 0 ]._0, 1 << 10, 0 );
    check_data( layer[ 1 ][ 0 ]._0, 1 << 9, 1 << 8 );
    check_data( layer[ 2 ][ 0 ]._0, 1 << 8, 1 << 8 );
}

OOE_TEST void fixture_type::test< 2 >( anonymous_t& )
{
    std::cerr << "box_tree zoom";

    box_tree tree( box( 1 << 10, 1 << 10, 0, 0 ), colour, pointer );

    for ( up_t i = 0; i != 9; ++i )
    {
        box_tree::layer_vector layer = tree.view( 1 << 11, 1 << 11, 1 << 9, 1 << 9, i );
        check_size( layer, 1 );
        check_data( layer[ 0 ][ 0 ]._0, 1 << ( 10 + i ), -( 1 << ( 9 + i ) ) );
    }
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
