/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

    #ifndef OOE_FOUNDATION_IPC_MEMORY_RPC_HPP
    #define OOE_FOUNDATION_IPC_MEMORY_RPC_HPP

#include <vector>

#include "foundation/ipc/memory/rpc_forward.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

template< typename >
    struct call;

//--- find -----------------------------------------------------------------------------------------
struct find
    : private rpc< index_t ( const std::string&, const std::string& ) >
{
    typedef rpc< index_t ( const std::string&, const std::string& ) > base_type;
    using base_type::result_type;

    find( memory::transport& transport_ )
        : base_type( transport_, 1 )
    {
    }

    result_type operator ()( const std::string& name, const std::string& type ) const
    {
        index_t i = base_type::operator ()( name, type );

        if ( i != ~index_t( 0 ) )
            return i;

        throw error::runtime( "ipc::memory::find: " ) <<
            "Unable to find \"" << name << "\" of type \"" << type << '\"';
    }
};

//--- list -----------------------------------------------------------------------------------------
struct list
    : public rpc< std::vector< tuple< std::string, std::string > > ( void ) >
{
    list( memory::transport& transport_ )
        : rpc< std::vector< tuple< std::string, std::string > > ( void ) >( transport_, 2 )
    {
    }
};

//--- find_all -------------------------------------------------------------------------------------
struct find_all
    : private
    rpc< std::vector< index_t > ( const std::vector< tuple< std::string, std::string > >& ) >
{
    typedef std::vector< tuple< std::string, std::string > > parameter_type;
    typedef rpc< result_type ( const parameter_type& ) > base_type;
    using base_type::result_type;

    find_all( memory::transport& transport_ )
        : base_type( transport_, 3 )
    {
    }

    result_type operator ()( const parameter_type& parameter ) const
    {
        result_type result = base_type::operator ()( parameter );

        for ( up_t i = 0, end = result.size(); i != end; ++i )
        {
            if ( result[ i ] != ~index_t( 0 ) )
                continue;

            throw error::runtime( "ipc::memory::find_all: " ) << "Unable to find \"" <<
                parameter[ i ]._0 << "\" of type \"" << parameter[ i ]._1 << '\"';
        }

        return result;
    }
};

//--- doc ------------------------------------------------------------------------------------------
struct doc
    : public rpc< std::string ( const std::string&, const std::string& ) >
{
    doc( memory::transport& transport_ )
        : rpc< std::string ( const std::string&, const std::string& ) >( transport_, 4 )
    {
    }
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

    #define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
    #define BOOST_PP_FILENAME_1 "foundation/ipc/memory/rpc.hpp"
    #include BOOST_PP_ITERATE()
    #undef BOOST_PP_FILENAME_1
    #undef BOOST_PP_ITERATION_LIMITS

    #endif  // OOE_FOUNDATION_IPC_MEMORY_RPC_HPP

#else   // BOOST_PP_IS_ITERATING

    #define LIMIT BOOST_PP_ITERATION()

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- call -----------------------------------------------------------------------------------------
template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
    struct call< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
    : public rpc< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
{
    typedef r signature_type( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
    typedef rpc< signature_type > base_type;

    call( memory::transport& transport_, const std::string& name )
        : base_type( transport_, find( transport_ )( name, typeid( signature_type ).name() ) )
    {
    }
};

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )

    #undef LIMIT

#endif  // BOOST_PP_IS_ITERATING
