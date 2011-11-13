/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <python3.2m/Python.h>

#include "component/python/traits.hpp"
#include "component/python/vm.hpp"
#include "foundation/io/memory.hpp"
#include "foundation/utility/scoped.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

std::string load_script( const descriptor& desc )
{
    memory memory( desc );
    return std::string( memory.as< c8 >(), memory.size() );
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

//--- vm -------------------------------------------------------------------------------------------
vm::vm( void )
    : globals()
{
    Py_InitializeEx( 0 );
    scoped< void ( void ) > scoped( Py_Finalize );

    module module( "__main__", module::import );
    globals = module.get();
    scoped.clear();
}

vm::~vm( void )
{
    collect();
    Py_Finalize();
}

void vm::load( const std::string& name, const descriptor& desc )
{
    std::string script = load_script( desc );
    object result = PyRun_String( script.c_str(), Py_file_input, globals, globals );

    if ( result )
        return;

    exception_tuple tuple = get_exception();
    throw error::python() <<
        "Unable to load \"" << name << "\":\n" << tuple._0 << "\nPython backtrace:\n" << tuple._1;
}

void vm::collect( void )
{
    PyGC_Collect();
}

std::string vm::version( void ) const
{
    return Py_GetVersion();
}

void vm::setup( function_type function )
{
    function( globals );
}

OOE_NAMESPACE_END( ( ooe )( python ) )
