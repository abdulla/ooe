/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_ERROR_HPP
#define OOE_FOUNDATION_UTILITY_ERROR_HPP

#include <exception>

#include <cstring>

#include <cxxabi.h>
#include <dlfcn.h>
#include <execinfo.h>

// must be forward declared for clang
#include "foundation/utility/string.hpp"

#include "foundation/utility/convert.hpp"
#include "foundation/utility/macro.hpp"
#include "foundation/utility/pointer.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- demangle -------------------------------------------------------------------------------------
inline std::string demangle( const std::string& mangled )
{
    s32 status;
    scoped_free< c8 > demangled( abi::__cxa_demangle( mangled.c_str(), 0, 0, &status ) );
    return status ? mangled : std::string( demangled );
}

//--- stack_trace ----------------------------------------------------------------------------------
template< u32 begin >
    std::string stack_trace( void )
{
    const u32 size = 32;
    void* address[ size ];
    s32 used = backtrace( address, size );
    Dl_info info;
    std::string string;

    for ( s32 i = begin; i < used; ++i )
    {
        string << '\n' << i - begin << ": " << ptr( address[ i ] );

        if ( dladdr( address[ i ], &info ) && info.dli_sname )
            string << ' ' << demangle( info.dli_sname );
    }

    return string;
}

OOE_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( error ) )

//--- runtime --------------------------------------------------------------------------------------
class OOE_VISIBLE runtime
    : virtual public std::exception
{
public:
    runtime( const std::string& what_string_ )
        : exception(), what_string( what_string_ ), where_string( stack_trace< 2 >() )
    {
    }

    virtual ~runtime( void ) throw()
    {
    }

    runtime( const runtime& copy )
        : exception(), what_string( copy.what_string ), where_string( copy.where_string )
    {
    }

    runtime& operator =( const runtime& copy )
    {
        what_string = copy.what_string;
        where_string = copy.where_string;
        return *this;
    }

    virtual const c8* what( void ) const throw()
    {
        return what_string.c_str();
    }

    const c8* where( void ) const throw()
    {
        return where_string.c_str();
    }

    runtime& operator <<( const std::string& value )
    {
        what_string += value;
        return *this;
    }

    runtime& operator <<( const c8* value )
    {
        what_string += value;
        return *this;
    }

    runtime& operator <<( c8 value )
    {
        what_string += value;
        return *this;
    }

private:
    std::string what_string;
    std::string where_string;
};

//--- number ---------------------------------------------------------------------------------------
class number
{
public:
    number( s32 id_ )
        : id( id_ )
    {
    }

private:
    s32 id;

    template< typename type >
        friend type& operator <<( type& out, const number& number )
    {
        return out << std::strerror( number.id );
    }
};

OOE_NAMESPACE_END( ( ooe )( error ) )

#endif  // OOE_FOUNDATION_UTILITY_ERROR_HPP
