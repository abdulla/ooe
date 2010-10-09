/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_EXECUTABLE_LIBRARY_HPP
#define OOE_FOUNDATION_EXECUTABLE_LIBRARY_HPP

#include "foundation/utility/macro.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/noncopyable.hpp"
#include "foundation/utility/string.hpp"

namespace ooe
{
//--- library ------------------------------------------------------------------
    class OOE_VISIBLE library
        : private noncopyable
    {
    public:
        enum type
        {
            local_lazy,
            local_now,
            global_lazy,
            global_now
        };

        enum find_type
        {
            all,
            next
        };

        library( const std::string&, type = local_lazy );
        ~library( void );

        void* find( const std::string& ) const;
        static void* find( const std::string&, find_type );

        template< typename t >
            t* find( const std::string& name,
            typename enable_if< is_function< t > >::type* = 0 ) const
        {
            return ptr_cast< t* >( find( name ) );
        }

        template< typename t >
            t* find( const std::string& name,
            typename disable_if< is_function< t > >::type* = 0 ) const
        {
            return static_cast< t* >( find( name ) );
        }

        template< typename t >
            static t* find( const std::string& name, find_type flag,
            typename enable_if< is_function< t > >::type* = 0 )
        {
            return ptr_cast< t* >( find( name, flag ) );
        }

        template< typename t >
            static t* find( const std::string& name, find_type flag,
            typename disable_if< is_function< t > >::type* = 0 )
        {
            return static_cast< t* >( find( name, flag ) );
        }

    private:
        void* id;
    };
}

#endif  // OOE_FOUNDATION_EXECUTABLE_LIBRARY_HPP
