/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <algorithm>

#include "component/registry/module.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- module ---------------------------------------------------------------------------------------
const interface::vector_type& module::names( void ) const
{
    return face.get();
}

const module::vector_type& module::docs( void ) const
{
    return vector;
}

const c8* module::doc( const std::string& name, const std::string& type ) const
{
    const interface::vector_type& faces = face.get();
    interface::vector_tuple tuple( name, type );
    interface::vector_type::const_iterator end = faces.end();
    interface::vector_type::const_iterator i = std::lower_bound( faces.begin(), end, tuple );

    if ( i == end )
        throw error::runtime( "module: " ) <<
            "Unable to find documentation for \"" << name << "\" of type \"" << type << '\"';

    vector_type::const_iterator j = vector.begin();
    std::advance( j, std::distance( faces.begin(), i ) );
    return *j;
}

up_t module::insert( const std::string& name, const std::string& type, const c8* doc_ )
{
    up_t index = face.insert( name, type );
    vector_type::iterator i = vector.begin();
    std::advance( i, index );
    vector.insert( i, doc_ );
    return index;
}

const void* module::find( const std::string& name ) const
{
    map_type::const_iterator i = map.find( name );

    if ( i == map.end() || i->first != name )
        throw error::runtime( "module: " ) << "Unable to find \"" << name << "\", does not exist";

    return i->second;
}

void module::insert( const std::string& name, const opaque_ptr& pointer )
{
    if ( !map.insert( map_type::value_type( name, pointer ) ).second )
        throw error::runtime( "module: " ) << "Unable to insert \"" << name << "\", facade exists";
}

OOE_NAMESPACE_END( ( ooe ) )
