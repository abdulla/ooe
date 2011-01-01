/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_TRAITS_FORWARD_HPP
#define OOE_FOUNDATION_IPC_TRAITS_FORWARD_HPP

#include <cstring>

#include "foundation/utility/macro.hpp"
#include "foundation/utility/string.hpp"
#include "foundation/utility/traits.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

template< typename, typename = void >
    struct size;

template< typename, typename = void >
    struct read;

template< typename, typename = void >
    struct write;

//--- is_pod ---------------------------------------------------------------------------------------
template< typename t >
    struct is_pod
{
    static const bool value =
        !is_cstring< t >::value &&
        ( ooe::is_pod< typename no_ref< t >::type >::value ||
        has_trivial_copy< typename no_ref< t >::type >::value );
};

//--- traits: default ------------------------------------------------------------------------------
template< typename NO_SPECIALISATION_DEFINED, typename >
    struct size
{
    static up_t call( NO_SPECIALISATION_DEFINED ) OOE_CONST
    {
        OOE_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
        return 0;
    }
};

template< typename NO_SPECIALISATION_DEFINED, typename >
    struct read
{
    static up_t call( const u8*, NO_SPECIALISATION_DEFINED ) OOE_CONST
    {
        OOE_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
        return 0;
    }
};

template< typename NO_SPECIALISATION_DEFINED, typename >
    struct write
{
    static up_t call( u8*, NO_SPECIALISATION_DEFINED ) OOE_CONST
    {
        OOE_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
        return 0;
    }
};

//--- traits: empty --------------------------------------------------------------------------------
template< typename t >
    struct size< t, typename enable_if< is_empty< t > >::type >
{
    static up_t call( t ) OOE_CONST
    {
        return 0;
    }
};

template< typename t >
    struct read< t, typename enable_if< is_empty< t > >::type >
{
    static up_t call( const u8*, t ) OOE_CONST
    {
        return 0;
    }
};

template< typename t >
    struct write< t, typename enable_if< is_empty< t > >::type >
{
    static up_t call( u8*, t ) OOE_CONST
    {
        return 0;
    }
};

//--- traits: pod ----------------------------------------------------------------------------------
template< typename t >
    struct size< t, typename enable_if< is_pod< t > >::type >
{
    static up_t call( t ) OOE_PURE
    {
        return sizeof( t );
    }
};

template< typename t >
    struct read< t, typename enable_if< is_pod< t > >::type >
{
    static up_t call( const u8* buffer, typename call_traits< t >::reference value )
    {
        value = *reinterpret_cast< const t* >( buffer );
        return size< t >::call( value );
    }
};

template< typename t >
    struct write< t, typename enable_if< is_pod< t > >::type >
{
    static up_t call( u8* buffer, t value )
    {
        *reinterpret_cast< t* >( buffer ) = value;
        return size< t >::call( value );
    }
};

//--- traits: string -------------------------------------------------------------------------------
template< typename t >
    struct size< t, typename enable_if< is_string< t > >::type >
{
    static up_t call( typename call_traits< t >::param_type value ) OOE_PURE
    {
        return string_size( value ) + 1;
    }
};

template< typename t >
    struct read< t, typename enable_if< is_string< t > >::type >
{
    static up_t call( const u8* buffer, typename call_traits< t >::reference value )
    {
        const c8* data = reinterpret_cast< const c8* >( buffer );
        up_t size = string_size( data );
        value = string_make< typename no_ref< t >::type >( data, size );
        return size + 1;
    }
};

template< typename t >
    struct write< t, typename enable_if< is_string< t > >::type >
{
    static up_t call( u8* buffer, typename call_traits< t >::param_type value )
    {
        const c8* string = string_data( value );
        up_t size = ipc::size< t >::call( value );
        std::memcpy( buffer, string, size );
        return size;
    }
};

//--- traits: array --------------------------------------------------------------------------------
template< typename t >
    struct size< t, typename enable_if< is_array< t > >::type >
{
    static up_t call( typename call_traits< t >::param_type value ) OOE_PURE
    {
        typedef typename no_ref< t >::type value_type;
        typedef typename remove_extent< value_type >::type type;

        up_t sum = 0;

        for ( up_t i = 0; i != extent< value_type >::value; ++i )
            sum += size< type >::call( value[ i ] );

        return sum;
    }
};

template< typename t >
    struct read< t, typename enable_if< is_array< t > >::type >
{
    static up_t call( const u8* buffer, t value )
    {
        typedef typename no_ref< t >::type value_type;
        typedef typename remove_extent< value_type >::type type;
        const u8* pointer = buffer;

        for ( up_t i = 0; i != extent< value_type >::value; ++i )
            pointer += read< type >::call( pointer, value[ i ] );

        return pointer - buffer;
    }
};

template< typename t >
    struct write< t, typename enable_if< is_array< t > >::type >
{
    static up_t call( u8* buffer, typename call_traits< t >::param_type value, up_t )
    {
        typedef typename no_ref< t >::type value_type;
        typedef typename remove_extent< value_type >::type type;
        u8* pointer = buffer;

        for ( up_t i = 0; i != extent< value_type >::value; ++i )
            pointer += write< type >::call( pointer, value[ i ] );

        return pointer - buffer;
    }
};

OOE_NAMESPACE_END( ( ooe )( ipc ) )

#endif  // OOE_FOUNDATION_IPC_TRAITS_FORWARD_HPP
