/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <set>

#include "foundation/ipc/nameservice.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( ipc ) )

up_t ipc_find( const any& any, io_buffer& buffer, pool& pool )
{
    std::string name;
    std::string type;
    stream_read< std::string, std::string >::call( buffer.get(), name, type );

    index_t index = static_cast< nameservice* >( any.pointer )->find( name, type );
    return return_write< index_t >( buffer, pool, index );
}

up_t ipc_list( const any& any, io_buffer& buffer, pool& pool )
{
    nameservice::list_type list = static_cast< nameservice* >( any.pointer )->list();
    return return_write< nameservice::list_type >( buffer, pool, list );
}

up_t ipc_find_all( const any& any, io_buffer& buffer, pool& pool )
{
    typedef std::vector< tuple< std::string, std::string > > in_type;
    in_type in;
    stream_read< in_type >::call( buffer.get(), in );

    typedef std::vector< index_t > out_type;
    out_type out;
    out.reserve( in.size() );

    ipc::nameservice& nameservice = *static_cast< ipc::nameservice* >( any.pointer );

    for ( in_type::const_iterator i = in.begin(), end = in.end(); i != end; ++i )
        out.push_back( nameservice.find( i->_0, i->_1 ) );

    return return_write< out_type >( buffer, pool, out );
}

up_t ipc_doc( const any& any, io_buffer& buffer, pool& pool )
{
    std::string name;
    std::string type;
    stream_read< std::string, std::string >::call( buffer.get(), name, type );

    std::string doc = static_cast< nameservice* >( any.pointer )->doc( name, type );
    return return_write< std::string >( buffer, pool, doc );
}

up_t ipc_upgrade( const any&, io_buffer&, pool& )
{
    throw error::runtime( "ipc::nameservice: " ) << "Server upgraded, function missing";
}

void load_switchboard( switchboard& switchboard, any any )
{
    if ( switchboard.insert_direct( ipc_find, any ) != 1 )
        throw error::runtime( "ipc::nameservice: " ) << "\"find\" not at index 1";
    else if ( switchboard.insert_direct( ipc_list, any ) != 2 )
        throw error::runtime( "ipc::nameservice: " ) << "\"list\" not at index 2";
    else if ( switchboard.insert_direct( ipc_find_all, any ) != 3 )
        throw error::runtime( "ipc::nameservice: " ) << "\"find_all\" not at index 3";
    else if ( switchboard.insert_direct( ipc_doc, any ) != 4 )
        throw error::runtime( "ipc::nameservice: " ) << "\"doc\" not at index 4";
}

const up_t index_adjust = 5;

OOE_ANONYMOUS_END( ( ooe )( ipc ) )

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

//--- nameservice ----------------------------------------------------------------------------------
nameservice::nameservice( void )
    : switchboard(), vector(), map()
{
    load_switchboard( switchboard, this );
}

void nameservice::upgrade( const list_type& names )
{
    typedef std::set< tuple_type > set_type;
    set_type set;
    vector_type upgrade_vector;
    ipc::switchboard upgrade_switchboard;
    load_switchboard( upgrade_switchboard, this );

    for ( up_t i = 0, end = names.size(); i != end; ++i )
    {
        const tuple_type& name = names[ i ];
        index_t index = i + index_adjust;
        set.insert( name );

        std::pair< map_type::iterator, bool > pair =
            map.insert( map_type::value_type( names[ i ], index ) );
        switchboard::tuple_type tuple( 0, 0 );

        if ( pair.second )
        {
            upgrade_vector.push_back( 0 );
            tuple = switchboard::tuple_type( ipc_upgrade, 0 );
        }
        else
        {
            upgrade_vector.push_back( vector[ pair.first->second - index_adjust ] );
            tuple = switchboard[ pair.first->second ];
            pair.first->second = index;
        }

        if ( upgrade_switchboard.insert_direct( tuple._0, tuple._1 ) != index )
            throw error::runtime( "ipc::nameservice: " ) <<
                '\"' << name._0 << "\" of type \"" << name._1 << "\" not at index " << index;
    }

    set_type::const_iterator set_end = set.end();

    for ( map_type::iterator i = map.begin(), end = map.end(); i != end; ++i )
    {
        if ( set.find( i->first ) != set_end )
            continue;

        upgrade_vector.push_back( vector[ i->second - index_adjust ] );
        switchboard::tuple_type tuple = switchboard[ i->second ];
        i->second = upgrade_switchboard.insert_direct( tuple._0, tuple._1 );
    }

    vector.swap( upgrade_vector );
    switchboard.swap( upgrade_switchboard );
}

nameservice::operator const ipc::switchboard&( void ) const
{
    return switchboard;
}

index_t nameservice::find( const std::string& name, const std::string& type ) const
{
    map_type::const_iterator i = map.find( tuple_type( name, type ) );
    return i == map.end() ? ~index_t( 0 ) : i->second;
}

nameservice::list_type nameservice::list( void ) const
{
    list_type value;
    value.resize( map.size() );

    for ( map_type::const_iterator i = map.begin(), end = map.end(); i != end; ++i )
        value[ i->second - index_adjust ] = i->first;

    return value;
}

std::string nameservice::doc( const std::string& name, const std::string& type ) const
{
    map_type::const_iterator i = map.find( tuple_type( name, type ) );

    if ( i == map.end() )
        throw error::runtime( "ipc::nameservice: " ) <<
            "Unable to find documentation for \"" << name << "\" of type \"" << type << '\"';

    up_t index = i->second - index_adjust;
    return vector[ index ] ? vector[ index ] : std::string();
}

void nameservice::insert_direct( const std::string& name, const std::string& type, const c8* doc_,
    index_t index )
{
    vector_type::iterator i = vector.begin();
    std::advance( i, index - index_adjust );
    vector.insert( i, doc_ );
    map.insert( map_type::value_type( tuple_type( name, type ), index ) );
}

void nameservice::insert_direct( const std::string& name, const std::string& type, const c8* doc_,
    switchboard::call_type call, any any )
{
    insert_direct( name, type, doc_, switchboard.insert_direct( call, any ) );
}

OOE_NAMESPACE_END( ( ooe )( ipc ) )
