/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/python/traits.hpp"
#include "component/python/vm.hpp"
#include "foundation/io/memory.hpp"
#include "foundation/utility/scoped.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

//--- vm -------------------------------------------------------------------------------------------
vm::vm( void )
	: globals()
{
	Py_InitializeEx( 0 );
	scoped< void ( void ) > scoped( Py_Finalize );
	PyObject* module = valid( PyImport_AddModule( "__main__" ) );

	if ( !module )
		throw error::python() << "Unable to import module \"__main__\"";

	globals = PyModule_GetDict( module );
	scoped.clear();
}

vm::~vm( void )
{
	collect();
	Py_Finalize();
}

void vm::load( const std::string& name, const descriptor& desc )
{
	std::string script;

	{
		memory memory( desc );
		script = std::string( memory.as< c8 >(), memory.size() );
	}

	object result = PyRun_String( script.c_str(), Py_file_input, globals, globals );

	if ( result )
		return;

	exception_tuple tuple = get_exception();
	throw error::python() <<
		"Unable to load \"" << name << "\": " << tuple._0 << "\nPython backtrace:\n" << tuple._1;
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
