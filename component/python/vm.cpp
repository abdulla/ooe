/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/python/object.hpp"
#include "component/python/traits.hpp"
#include "component/python/vm.hpp"
#include "foundation/io/memory.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

//--- vm -------------------------------------------------------------------------------------------
vm::vm( void )
	: globals()
{
	Py_InitializeEx( 0 );
	PyObject* module = PyImport_AddModule( "__main__" );

	if ( !module )
	{
		Py_Finalize();
		throw error::python() << "Unable to import module \"__main__\"";
	}

	globals = PyModule_GetDict( module );
}

vm::~vm( void )
{
	collect();
	Py_Finalize();
}

void vm::load( const std::string& name, const descriptor& desc )
{
	memory memory( desc );
	object result = PyRun_String( memory.as< c8 >(), Py_file_input, globals, globals );

	if ( result )
		return;

	object py_type;
	object py_value;
	object py_backtrace;
	PyErr_Fetch( &py_type, &py_value, &py_backtrace );
	PyErr_NormalizeException( &py_type, &py_value, &py_backtrace );

	object str_value = py_value.str();
	object str_backtrace = py_value.str();

	std::string value;
	std::string backtrace;
	as< std::string >::call( str_value, value );
	as< std::string >::call( str_backtrace, backtrace );

	throw error::python() <<
		"Unable to load \"" << name << "\": " << value << "\nPython backtrace:\n" << backtrace;
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
