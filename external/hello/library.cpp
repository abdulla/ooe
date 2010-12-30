/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>
#include <set>

#include "component/javascript/facade.hpp"
#include "component/lua/facade.hpp"
#include "component/python/facade.hpp"
#include "component/registry/builder.hpp"
#include "component/registry/local.hpp"
#include "component/registry/remote.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

class print
{
public:
    print( const std::string& value_ )
        : value( value_ )
    {
    }

    ~print( void )
    {
        std::cout << value << '\n';
    }

    void say( void )
    {
        std::cout << this << " said: " << value << '\n';
    }

private:
    std::string value;
};

void hello( void )
{
    std::cout << "hello library\n";
}

tuple< bool, f32 > gauntlet( bool b, f32 f )
{
    return make_tuple( b, f );
}

bool* mismatch( void )
{
    return 0;
}

typedef std::map< std::string, up_t > map_type;
map_type stdmap( const map_type& map )
{
    for ( map_type::const_iterator i = map.begin(), end = map.end(); i != end; ++i )
        std::cout << "key: " << i->first << ", value: " << i->second << '\n';

    return map;
}

typedef std::set< std::string > set_type;
set_type stdset( const set_type& set )
{
    for ( set_type::const_iterator i = set.begin(), end = set.end(); i != end; ++i )
        std::cout << "key: " << *i << '\n';

    return set;
}

OOE_NAMESPACE_END( ( ooe ) )

//--- module_open ----------------------------------------------------------------------------------
extern "C" ooe::module OOE_VISIBLE module_open( void )
{
    module module;

    builder< facade::remote, facade::lua, facade::javascript, facade::python > builder( module );
    builder.insert( "hello", hello, "A function that prints 'hello library'." );
    builder.insert( "construct", construct< print, const std::string& >, "Constructs a 'print'." );
    builder.insert( "destruct", destruct< print >, "Destructs a 'print'." );
    builder.insert( "say", &print::say, "Calls the 'say' member function on a 'print'." );
    builder.insert( "gauntlet", gauntlet );
    builder.insert( "mismatch", mismatch );
    builder.insert( "stdmap", stdmap );
    builder.insert( "stdset", stdset );

    return module;
}
