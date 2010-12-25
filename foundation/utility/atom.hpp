/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_ATOM_HPP
#define OOE_FOUNDATION_UTILITY_ATOM_HPP

#include "foundation/utility/pointer.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 1
inline void memory_barrier( void )
{
    __sync_synchronize();
}
#endif

//--- atom_base ------------------------------------------------------------------------------------
template< typename type >
    class atom_base
{
public:
    explicit atom_base( type value = 0 )
        : atomic( value )
    {
    }

    operator type( void ) const
    {
        return atomic;
    }

    void operator =( type value )
    {
        test_and_set( value );
    }

    type operator ++( void )
    {
        return add_and_fetch( 1 );
    }

    type operator ++( int )
    {
        return fetch_and_add( 1 );
    }

    type operator --( void )
    {
        return add_and_fetch( -1 );
    }

    type operator --( int )
    {
        return fetch_and_add( -1 );
    }

    type operator +=( type value )
    {
        return add_and_fetch( value );
    }

    type operator -=( type value )
    {
        return add_and_fetch( -value );
    }

    bool cas( type compare, type value )
    {
        return compare_and_swap( compare, value );
    }

private:
    type atomic;

#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 1
    type fetch_and_add( type value )
    {
        return __sync_fetch_and_add( &atomic, value );
    }

    type add_and_fetch( type value )
    {
        return __sync_add_and_fetch( &atomic, value );
    }

    type test_and_set( type value )
    {
        return __sync_lock_test_and_set( &atomic, value );
    }

    bool compare_and_swap( type compare, type value )
    {
        return __sync_bool_compare_and_swap( &atomic, compare, value );
    }
#endif
};

//--- atom -----------------------------------------------------------------------------------------
template< typename type >
    struct atom
    : public atom_base< type >
{
    using atom_base< type >::operator =;

    explicit atom( type value_ = 0 )
        : atom_base< type >( value_ )
    {
    }
};

template< typename type >
    struct atom< type* >
    : public atom_base< type* >
{
    using atom_base< type* >::operator =;

    explicit atom( type* value_ = 0 )
        : atom_base< type* >( value_ )
    {
    }

    type* operator ->( void ) const
    {
        return *this;
    }

    type& operator *( void ) const
    {
        return **this;
    }
};

//--- atom_ptr -------------------------------------------------------------------------------------
template< typename type >
    struct atom_ptr
    : public shared_dereference< type, deallocate_ptr< type >, atom< unsigned > >
{
    atom_ptr( type* value = 0 )
        : shared_dereference< type, deallocate_ptr< type >, atom< unsigned > >( value )
    {
    }
};

//--- atom_array -----------------------------------------------------------------------------------
template< typename type >
    struct atom_array
    : public shared_dereference< type, deallocate_array< type >, atom< unsigned > >
{
    atom_array( type* value = 0 )
        : shared_dereference< type, deallocate_array< type >, atom< unsigned > >( value )
    {
    }
};

//--- atom_free ------------------------------------------------------------------------------------
template< typename type >
    struct atom_free
    : public shared_dereference< type, deallocate_free< type >, atom< unsigned > >
{
    atom_free( type* value = 0 )
        : shared_dereference< type, deallocate_free< type >, atom< unsigned > >( value )
    {
    }
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_UTILITY_ATOM_HPP
