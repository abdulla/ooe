/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_ATOM_HPP
#define OOE_FOUNDATION_UTILITY_ATOM_HPP

#include <tr1/memory>

#include "foundation/utility/namespace.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

inline void memory_barrier( void )
{
#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 1
    __sync_synchronize();
#endif
}

//--- atom_base ------------------------------------------------------------------------------------
template< typename type >
    class atom_base
{
public:
    explicit atom_base( type value = 0 )
        : atomic( value )
    {
    }

    template< typename to >
        to as( void ) const
    {
        return static_cast< to >( atomic );
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
    : public std::tr1::shared_ptr< type >
{
    atom_ptr( type* value = 0 )
        : std::tr1::shared_ptr< type >( value )
    {
    }
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_UTILITY_ATOM_HPP
