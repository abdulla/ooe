/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_NAMESERVICE_HPP
#define OOE_FOUNDATION_IPC_NAMESERVICE_HPP

#include "foundation/ipc/switchboard.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

//--- nameservice ----------------------------------------------------------------------------------
class OOE_VISIBLE nameservice
    : private noncopyable
{
public:
    typedef tuple< std::string, std::string > tuple_type;
    typedef std::vector< tuple_type > list_type;

    nameservice( void );

    void upgrade( const list_type& );
    operator const ipc::switchboard&( void ) const;

    index_t find( const std::string&, const std::string& ) const;
    list_type list( void ) const;
    std::string doc( const std::string&, const std::string& ) const;

    void insert_direct( const std::string&, const std::string&, const c8*, index_t );
    void insert_direct( const std::string&, const std::string&, const c8*, switchboard::call_type,
        any );

    template< typename t >
        void insert( const std::string& name, t function, const c8* doc_ = 0,
        typename enable_if< is_function_pointer< t > >::type* = 0 )
    {
        typedef typename remove_pointer< t >::type function_type;
        insert_direct( name, typeid( function_type ).name(), doc_, switchboard.insert( function ) );
    }

    template< typename t >
        void insert( const std::string& name, t member, const c8* doc_ = 0,
        typename enable_if< is_member_function_pointer< t > >::type* = 0 )
    {
        typedef typename function_of< t >::type member_type;
        insert_direct( name, typeid( member_type ).name(), doc_, switchboard.insert( member ) );
    }

private:
    typedef std::vector< const c8* > vector_type;
    typedef std::map< tuple_type, index_t > map_type;

    ipc::switchboard switchboard;
    vector_type vector;
    map_type map;
};

OOE_NAMESPACE_END( ( ooe )( ipc ) )

#endif  // OOE_FOUNDATION_IPC_NAMESERVICE_HPP
