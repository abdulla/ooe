/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_POOL_HPP
#define OOE_FOUNDATION_IPC_POOL_HPP

#include <map>

#include "foundation/ipc/error.hpp"
#include "foundation/ipc/traits.hpp"
#include "foundation/utility/hint.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/noncopyable.hpp"

namespace ooe
{
    namespace ipc
    {
        class pool;

//--- *_ptr --------------------------------------------------------------------
        template< typename >
            struct nullable_ptr;

        template< typename >
            struct unchecked_ptr;

//--- is_* ---------------------------------------------------------------------
        template< typename >
            struct is_pointer;

        template< typename >
            struct is_nullable;

        template< typename >
            struct is_unchecked;

        template< typename >
            struct is_proxy;

//--- ipc::traits: proxy -------------------------------------------------------
        template< typename t >
            struct size< t, typename enable_if< is_proxy< t > >::type >;

        template< typename t >
            struct read< t, typename enable_if< is_proxy< t > >::type >;

        template< typename t >
            struct write< t, typename enable_if< is_proxy< t > >::type >;

//--- verify -------------------------------------------------------------------
        template< typename, typename = void >
            struct verify;

        template< typename t >
            struct verify< t, typename enable_if< is_construct< t > >::type >;

        template< typename t >
            struct verify< t, typename enable_if< is_destruct< t > >::type >;

        template< typename t >
            struct verify< t, typename enable_if< is_pointer< t > >::type >;

        template< typename t >
            struct verify< t, typename enable_if< is_nullable< t > >::type >;
    }

//--- ipc::pool ----------------------------------------------------------------
    class ipc::pool
        : private noncopyable
    {
    public:
        typedef void ( * function_type )( const void* );

        ~pool( void );

        void insert( const void*, function_type ) OOE_VISIBLE;
        void erase( const void* ) OOE_VISIBLE;
        bool find( const void* ) const OOE_VISIBLE;

    private:
        typedef std::map< const void*, function_type > map_type;

        map_type map;
    };

//--- ipc::nullable_ptr --------------------------------------------------------
    template< typename t >
        struct ipc::nullable_ptr
        : public proxy_ptr< t >
    {
        nullable_ptr( t* value_ = 0 )
            : proxy_ptr< t >( value_ )
        {
        }
    };

//--- ipc::unchecked_ptr -------------------------------------------------------
    template< typename t >
        struct ipc::unchecked_ptr
        : public proxy_ptr< t >
    {
        unchecked_ptr( t* value_ = 0 )
            : proxy_ptr< t >( value_ )
        {
        }
    };

//--- ipc::is_pointer ----------------------------------------------------------
    template< typename t >
        struct ipc::is_pointer
    {
        static const bool value = !is_cstring< t >::value && ooe::is_pointer< t >::value;
    };


//--- ipc::is_nullable ---------------------------------------------------------
    template< typename t >
        struct ipc::is_nullable
        : public is_template1< t, nullable_ptr >
    {
    };

//--- ipc::is_unchecked --------------------------------------------------------
    template< typename t >
        struct ipc::is_unchecked
        : public is_template1< t, unchecked_ptr >
    {
    };

//--- ipc::is_proxy ------------------------------------------------------------
    template< typename t >
        struct ipc::is_proxy
    {
        static const bool value = is_construct< t >::value || is_destruct< t >::value ||
            is_nullable< t >::value || is_unchecked< t >::value;
    };

//--- ipc::traits: proxy -------------------------------------------------------
    template< typename t >
        struct ipc::size< t, typename enable_if< ipc::is_proxy< t > >::type >
    {
        static up_t call( t )
        {
            return sizeof( t );
        }
    };

    template< typename t >
        struct ipc::read< t, typename enable_if< ipc::is_proxy< t > >::type >
    {
        static up_t call( const u8* buffer, typename call_traits< t >::reference value )
        {
            value = *reinterpret_cast< const t* >( buffer );
            return size< t >::call( value );
        }
    };

    template< typename t >
        struct ipc::write< t, typename enable_if< ipc::is_proxy< t > >::type >
    {
        static up_t call( u8* buffer, t value )
        {
            *reinterpret_cast< t* >( buffer ) = value;
            return size< t >::call( value );
        }
    };

//--- ipc::verify --------------------------------------------------------------
    template< typename t, typename >
        struct ipc::verify
    {
        static void call( pool&, typename call_traits< t >::param_type, u8 )
        {
        }
    };

    template< typename t >
        struct ipc::verify< t, typename enable_if< is_construct< t > >::type >
    {
        static void call( pool& pool, t pointer, u8 )
        {
            typedef typename remove_pointer< typename t::pointer >::type value_type;
            pool.insert( pointer, destroy< value_type > );
        }
    };

    template< typename t >
        struct ipc::verify< t, typename enable_if< is_destruct< t > >::type >
    {
        static void call( pool& pool, t pointer, u8 )
        {
            pool.erase( pointer );
        }
    };

    template< typename t >
        struct ipc::verify< t, typename enable_if< ipc::is_pointer< t > >::type >
    {
        static void call( pool& pool, t pointer, u8 index )
        {
            // could speed up further by checking against known out-of-bounds ranges/values
            if ( !pool.find( pointer ) )
                throw error::verification( pointer, index );
        }
    };

    template< typename t >
        struct ipc::verify< t, typename enable_if< ipc::is_nullable< t > >::type >
    {
        static void call( pool& pool, t pointer, u8 index )
        {
            if ( pointer && !pool.find( pointer ) )
                throw error::verification( pointer, index );
        }
    };
}

#endif  // OOE_FOUNDATION_IPC_POOL_HPP
