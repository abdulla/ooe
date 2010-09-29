/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_TEST_UNIT_RUNNER_HPP
#define OOE_TEST_UNIT_RUNNER_HPP

#include <map>

#include "foundation/utility/macro.hpp"
#include "foundation/utility/string.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

class group_base;

//--- runner ---------------------------------------------------------------------------------------
class runner
{
public:
    typedef std::map< std::string, group_base* > map_type;
    typedef map_type::const_iterator iterator_type;

    runner( void );

    iterator_type begin( void ) const;
    iterator_type end( void ) const;
    void insert( const std::string&, group_base& ) OOE_VISIBLE;
    bool run( time_t = 60, bool = true ) const;
    bool run( const std::string&, time_t = 60, bool = true ) const;

private:
    map_type map;
};

extern runner global_runner OOE_VISIBLE;

//--- fail -----------------------------------------------------------------------------------------
void fail( void ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe )( unit ) )

#endif  // OOE_TEST_UNIT_RUNNER_HPP
