/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

    #ifndef OOE_FOUNDATION_IPC_SOCKET_RPC_HPP
    #define OOE_FOUNDATION_IPC_SOCKET_RPC_HPP

#include <vector>

#include "foundation/ipc/socket/rpc_forward.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( socket ) )

template< typename >
    struct call;

//--- find_result ----------------------------------------------------------------------------------
class find_result
{
public:
    typedef socket::result< index_t > result_type;

    find_result( result_type result_, const std::string& name_, const std::string& type_ )
        : result( result_ ), name( name_ ), type( type_ )
    {
    }

    index_t operator ()( void ) const
    {
        index_t i = result();

        if ( i != ~index_t( 0 ) )
            return i;

        throw error::runtime( "ipc::socket::find: " ) <<
            "Unable to find \"" << name << "\" of type \"" << type << '\"';
    }

private:
    result_type result;
    const std::string name;
    const std::string type;
};

//--- find -----------------------------------------------------------------------------------------
struct find
    : private rpc< index_t ( const std::string&, const std::string& ) >
{
    typedef rpc< index_t ( const std::string&, const std::string& ) > base_type;

    find( socket::client& client_ )
        : base_type( client_, 1 )
    {
    }

    find_result operator ()( const std::string& name, const std::string& type ) const
    {
        return find_result( base_type::operator ()( name, type ), name, type );
    }
};

//--- list -----------------------------------------------------------------------------------------
struct list
    : public rpc< std::vector< tuple< std::string, std::string > > ( void ) >
{
    list( socket::client& client_ )
        : rpc< std::vector< tuple< std::string, std::string > > ( void ) >( client_, 2 )
    {
    }
};

//--- find_all_result ------------------------------------------------------------------------------
class find_all_result
{
public:
    typedef socket::result< std::vector< index_t > > result_type;
    typedef std::vector< tuple< std::string, std::string > > parameter_type;
    typedef result_type::return_type return_type;

    find_all_result( result_type result_ )
        : result( result_ ), state( false )
    {
    }

    return_type& operator ()( void ) const
    {
        if ( state )
            return result();

        return_type& value = result();

        for ( up_t i = 0, end = value.size(); i != end; ++i )
        {
            if ( value[ i ] != ~index_t( 0 ) )
                continue;

            throw error::runtime( "ipc::socket::find_all: " ) <<
                "Unable to find function at index: " << i;
        }

        state = true;
        return value;
    }

private:
    result_type result;
    mutable bool state;
};

//--- find_all -------------------------------------------------------------------------------------
struct find_all
    : private
    rpc< std::vector< index_t > ( const std::vector< tuple< std::string, std::string > >& ) >
{
    typedef find_all_result result_type;
    typedef std::vector< tuple< std::string, std::string > > parameter_type;
    typedef rpc< std::vector< index_t > ( const parameter_type& ) > base_type;

    find_all( socket::client& client_ )
        : base_type( client_, 3 )
    {
    }

    result_type operator ()( const parameter_type& parameter ) const
    {
        return base_type::operator ()( parameter );
    }
};

//--- doc ------------------------------------------------------------------------------------------
struct doc
    : public rpc< std::string ( const std::string&, const std::string& ) >
{
    doc( socket::client& client_ )
        : rpc< std::string ( const std::string&, const std::string& ) >( client_, 4 )
    {
    }
};

OOE_NAMESPACE_END( ( ooe )( ipc )( socket ) )

    #define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
    #define BOOST_PP_FILENAME_1 "foundation/ipc/socket/rpc.hpp"
    #include BOOST_PP_ITERATE()
    #undef BOOST_PP_FILENAME_1
    #undef BOOST_PP_ITERATION_LIMITS

    #endif  // OOE_FOUNDATION_IPC_SOCKET_RPC_HPP

#else   // BOOST_PP_IS_ITERATING

    #define LIMIT BOOST_PP_ITERATION()

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( socket ) )

//--- call -----------------------------------------------------------------------------------------
template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    struct call< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
    : public rpc< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
    typedef r signature_type( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
    typedef rpc< signature_type > base_type;

    call( socket::client& client_, const std::string& name )
        : base_type( client_, find( client_ )( name, typeid( signature_type ).name() )() )
    {
    }
};

OOE_NAMESPACE_END( ( ooe )( ipc )( socket ) )

    #undef LIMIT

#endif  // BOOST_PP_IS_ITERATING
