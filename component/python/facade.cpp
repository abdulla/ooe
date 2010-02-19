/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/python/facade.hpp"
#include "component/registry/local.hpp"
#include "component/registry/registry.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( python ) )

typedef std::vector< std::string > find_vector;

template< PyCFunction function >
	struct embed
{
	struct defer
	{
		static PyObject* call( PyObject* self, PyObject* arguments )
		{
			return function( self, arguments );
		}
	};

	static PyObject* call( PyObject* self, PyObject* arguments )
	{
		return invoke< defer >::call( self, arguments );
	}
};

PyObject* find( PyObject*, PyObject* list )
{
	find_vector input;
	as< find_vector >::call( list, input );
	interface interface;

	for ( find_vector::const_iterator i = input.begin(), end = input.end(); i != end; ++i )
	{
		up_t j = i->find( '/' );

		if ( j == ~up_t( 0 ) )
			throw error::runtime( "python::find: " ) << "Invalid function \"" << *i << '\"';

		interface.insert( i->substr( 0, j ), i->substr( j + 1 ) );
	}

	registry registry;
	registry::info_vector v = registry.find( interface );
	find_vector output;

	for ( registry::info_vector::const_iterator i = v.begin(), end = v.end(); i != end; ++i )
	{
		if ( i->_0 == registry::library )
			output.push_back( i->_1 );
	}

	return from< find_vector >::call( output );
}

PyObject* load( PyObject*, PyObject* string )
{
	std::string path;
	as< std::string >::call( string, path );

	typedef shared_ptr< ooe::source > source_ptr;
	source_ptr source = new ooe::source( path );
	const module& module = source->get();
	const interface::vector_type& names = module.names();
	const module::vector_type& docs = module.docs();
	const facade::local::vector_type& local = static_cast< const facade::local* >
		( module.find( typeid( facade::local ).name() ) )->get();
	const facade::python::vector_type& python = static_cast< const facade::python* >
		( module.find( typeid( facade::python ).name() ) )->get();

	object dictionary = valid( PyDict_New() );

	for ( up_t i = 0, end = names.size(); i != end; ++i )
	{
		object data;

		if ( names[ i ]._1[ 0 ] == 'F' )
			data = from< void ( * )( void ) >::call( local[ i ].function );
		else if ( names[ i ]._1[ 0 ] == 'M' )
			data = from< void ( any::* )( void ) >::call( local[ i ].member );
		else
			throw error::runtime( "python::load: " ) << "Function \"" << names[ i ]._0 <<
				"\" of type \"" << names[ i ]._1 << "\" has unknown pointer";

		PyMethodDef* method = const_cast< PyMethodDef* >( &python[ i ] );
		method->ml_name = names[ i ]._0.c_str();
		method->ml_doc = docs[ i ];

		std::string key = names[ i ]._0 + '/' + names[ i ]._1;
		object value = valid( PyCFunction_New( method, data ) );
		PyDict_SetItemString( dictionary, key.c_str(), value );
	}

	PyDict_SetItemString( dictionary, "", from< source_ptr >::call( source ) );
	return dictionary.release();
}

PyMethodDef methods[] =
{
	{ "find", embed< find >::call, METH_O, 0 },
	{ "load", embed< load >::call, METH_O, 0 }
};

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( python ) )

OOE_NAMESPACE_BEGIN( ( ooe )( facade ) )

//--- python ---------------------------------------------------------------------------------------
const python::vector_type& python::get( void ) const
{
	return vector;
}

void python::insert( up_t index, PyCFunction call )
{
	vector_type::iterator i = vector.begin();
	std::advance( i, index );

	PyMethodDef method = { "", call, METH_VARARGS, 0 };
	vector.insert( i, method );
}

OOE_NAMESPACE_END( ( ooe )( facade ) )

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

//--- component_setup ------------------------------------------------------------------------------
void component_setup( PyObject* globals )
{
	object ooe = valid( PyModule_New( "ooe" ) );
	Py_INCREF( Py_None );
	PyModule_AddObject( ooe, "__file__", Py_None );

	//--- registry -------------------------------------------------------------
	object registry = valid( PyModule_New( "registry" ) );
	Py_INCREF( Py_None );
	PyModule_AddObject( registry, "__file__", Py_None );

	PyModule_AddObject( registry, "find", valid( PyCFunction_New( methods + 0, 0 ) ) );
	PyModule_AddObject( registry, "load", valid( PyCFunction_New( methods + 1, 0 ) ) );

	PyModule_AddObject( ooe, "registry", registry.release() );

	//--------------------------------------------------------------------------

	PyDict_SetItemString( globals, "ooe", ooe );
}

//--- throw_exception ------------------------------------------------------------------------------
void throw_exception( const c8* what, const c8* where )
{
	std::string string;
	string << what << "\n\nStack trace:" << where;
	PyErr_SetString( PyExc_RuntimeError, string.c_str() );
}

OOE_NAMESPACE_END( ( ooe )( python ) )
