/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_ALIGN_HPP
#define OOE_FOUNDATION_UTILITY_ALIGN_HPP

#include <cerrno>
#include <cstdlib>

#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/error.hpp"

#ifdef __GNUC__
    #define OOE_ALIGN( size ) __attribute__( ( __aligned__( size ) ) )
#else
    #define OOE_ALIGN( size )
#endif

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- aligned --------------------------------------------------------------------------------------
template< up_t a >
    class aligned
{
public:
    aligned( up_t size )
        : pointer()
    {
        size = round_up< a >( size );

        if ( posix_memalign( &pointer, a, size ) )
            throw error::runtime( "aligned: " ) << "Unable to allocate " << size <<
                " bytes, aligned to " << a << " bytes: " << error::number( errno );
    }

    void deallocate( void )
    {
        std::free( pointer );
    }

    void* get( void ) const
    {
        return pointer;
    }

    void* release( void )
    {
        void* pass_back = pointer;
        pointer = 0;
        return pass_back;
    }

private:
    void* pointer;
};

//--- aligned_ptr ----------------------------------------------------------------------------------
template< up_t a >
    class aligned_ptr
{
public:
    aligned_ptr( aligned< a > value_ )
        : value( value_ )
    {
    }

    ~aligned_ptr( void )
    {
        value.deallocate();
    }

    operator aligned< a >( void ) const
    {
        return value;
    }

    template< typename to >
        to* as( void ) const
    {
        return static_cast< to* >( value.get() );
    }

    template< typename to >
        to* release( void )
    {
        return reinterpret_cast< to* >( value.release() );
    }

    void swap( aligned_ptr& exchange )
    {
        aligned< a > save = value;
        value = exchange.value;
        exchange.value = save;
    }

private:
    aligned< a > value;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_UTILITY_ALIGN_HPP
