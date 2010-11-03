/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "test/unit/group.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

//--- group_base -----------------------------------------------------------------------------------
group_base::const_iterator group_base::begin( void ) const
{
    return vector.begin();
}

group_base::const_iterator group_base::end( void ) const
{
    return vector.end();
}

void group_base::insert( function_type function )
{
    vector.push_back( function );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
