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
        exchange( value );
    }

    type operator ++( void )
    {
        return exchange_add( 1 ) + 1;
    }

    type operator ++( int )
    {
        return exchange_add( 1 );
    }

    type operator --( void )
    {
        return exchange_add( -1 ) - 1;
    }

    type operator --( int )
    {
        return exchange_add( -1 );
    }

    type operator +=( type value )
    {
        return exchange_add( value ) + value;
    }

    type operator -=( type value )
    {
        return exchange_add( -value ) - value;
    }

    type swap( type value )
    {
        return exchange( value );
    }

    bool cas( type compare, type value )
    {
        return compare_exchange( compare, value );
    }

protected:
    type atomic;

private:
#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 1
    type exchange_add( type value )
    {
        return __sync_fetch_and_add( &atomic, value );
    }

    type exchange( type value )
    {
        return __sync_lock_test_and_set( &atomic, value );
    }

    bool compare_exchange( type compare, type value )
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
        return this->atomic;
    }

    type& operator *( void ) const
    {
        return *this->atomic;
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
