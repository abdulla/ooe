/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "component/lua/facade.hpp"
#include "component/python/facade.hpp"
#include "component/registry/builder.hpp"
#include "component/registry/remote.hpp"
#include "component/storage/digest.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

class tree
{
public:
    sha256 digest( void );
    void insert( const sha256& );
    void replace( const sha256&, const sha256& );

private:
};

class blob
{
public:
    blob( tree* );

private:
};

OOE_NAMESPACE_END( ( ooe ) )

//--- module_open ----------------------------------------------------------------------------------
extern "C" ooe::module OOE_VISIBLE module_open( void )
{
    module module;

    builder< facade::remote, facade::lua, facade::python > builder( module );
    builder.insert( "", , "" );

    return module;
}