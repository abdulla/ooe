/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <algorithm>

#include "component/registry/interface.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- interface ------------------------------------------------------------------------------------
const interface::vector_type& interface::get( void ) const
{
    return vector;
}

up_t interface::insert( const std::string& name, const std::string& type )
{
    vector_tuple tuple( name, type );
    vector_type::iterator i = std::lower_bound( vector.begin(), vector.end(), tuple );

    if ( i != vector.end() && *i == tuple )
        throw error::runtime( "interface: " ) <<
            "Unable to insert \"" << name << "\" of type \"" << type << "\", function exists";

    up_t index = std::distance( vector.begin(), i );
    vector.insert( i, tuple );
    return index;
}

OOE_NAMESPACE_END( ( ooe ) )
