/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_PYTHON_TRAITS_FORWARD_HPP
#define OOE_COMPONENT_PYTHON_TRAITS_FORWARD_HPP

#include <Python.h>

#include "foundation/utility/hint.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

//--- as -------------------------------------------------------------------------------------------
template< typename, typename = void >
	struct as;

template< typename t >
	struct as< t, typename enable_if< is_empty< t > >::type >;

template< typename t >
	struct as< t, typename enable_if< is_boolean< t > >::type >;

template< typename t >
	struct as< t, typename enable_if< is_string< t > >::type >;

template< typename t >
	struct as< t, typename enable_if< is_floating_point< t > >::type >;

template< typename t >
	struct as< t, typename enable_if< is_integral< t > >::type >;

template< typename t >
	struct as< t, typename enable_if< is_pointer< t > >::type >;

//--- from -----------------------------------------------------------------------------------------
template< typename, typename = void >
	struct from;

template< typename t >
	struct from< t, typename enable_if< is_empty< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< is_boolean< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< is_string< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< is_floating_point< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< is_integral< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< is_pointer< t > >::type >;

//--- traits: default ------------------------------------------------------------------------------
template< typename NO_SPECIALISATION_DEFINED, typename >
	struct as
{
	static void call( PyObject*, NO_SPECIALISATION_DEFINED ) OOE_CONST
	{
		BOOST_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
	}
};

template< typename NO_SPECIALISATION_DEFINED, typename >
	struct from
{
	static PyObject* call( NO_SPECIALISATION_DEFINED ) OOE_CONST
	{
		BOOST_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
		return 0;
	}
};

//--- traits: empty --------------------------------------------------------------------------------
template< typename t >
	struct as< t, typename enable_if< is_empty< t > >::type >
{
	static void call( PyObject* object, typename call_traits< t >::reference )
	{
		if ( object != Py_None )
			throw error::python() << "Object is not none";
	}
};

template< typename t >
	struct from< t, typename enable_if< is_empty< t > >::type >
{
	static PyObject* call( typename call_traits< t >::param_type )
	{
		Py_RETURN_NONE;
	}
};

//--- traits: boolean ------------------------------------------------------------------------------
template< typename t >
	struct as< t, typename enable_if< is_boolean< t > >::type >
{
	static void call( PyObject* object, bool& boolean )
	{
		if ( !PyBool_Check( object ) )
			throw error::python() << "Object is not a boolean";

		boolean = object == Py_True;
	}
};

template< typename t >
	struct from< t, typename enable_if< is_boolean< t > >::type >
{
	static PyObject* call( bool boolean )
	{
		if ( boolean )
			Py_RETURN_TRUE;
		else
			Py_RETURN_FALSE;
	}
};

//--- traits: string -------------------------------------------------------------------------------
template< typename t >
	struct as< t, typename enable_if< is_string< t > >::type >
{
	static void call( PyObject* object, typename call_traits< t >::reference string )
	{
		if ( !PyBytes_Check( object ) )
			throw error::python() << "Object is not a string";

		string = string_make< typename no_ref< t >::type >
			( PyBytes_AS_STRING( object ), PyBytes_GET_SIZE( object ) );
	}
};

template< typename t >
	struct from< t, typename enable_if< is_string< t > >::type >
{
	static PyObject* call( typename call_traits< t >::param_type string )
	{
		return PyBytes_FromStringAndSize( string_data( string ), string_size( string ) );
	}
};

//--- traits: floating-point -----------------------------------------------------------------------
template< typename t >
	struct as< t, typename enable_if< is_string< t > >::type >
{
	static void call( PyObject* object, typename call_traits< t >::reference floating_point )
	{
		if ( !PyFloat_Check( object ) )
			throw error::python() << "Object is not a floating-point number";

		floating_point = PyFloat_AS_DOUBLE( object );
	}
};

template< typename t >
	struct from< t, typename enable_if< is_string< t > >::type >
{
	static PyObject* call( typename call_traits< t >::param_type floating_point )
	{
		return PyFloat_FromDouble( floating_point );
	}
};

//--- traits: integral -----------------------------------------------------------------------------
template< typename t >
	struct as< t, typename enable_if< is_integral< t > >::type >
{
	static void call( PyObject* object, typename call_traits< t >::reference integral )
	{
		if ( !PyLong_Check( object ) )
			throw error::python() << "Object is not a integral number";

		convert( object, integral );
	}

	static void convert( PyObject* object, long& integral )
	{
		integral = PyLong_AsLong( object );
	}

	static void convert( PyObject* object, unsigned long& integral )
	{
		integral = PyLong_AsUnsignedLong( object );
	}

	static void convert( PyObject* object, long long& integral )
	{
		integral = PyLong_AsLongLong( object );
	}

	static void convert( PyObject* object, unsigned long long& integral )
	{
		integral = PyLong_UnsignedLongLong( object );
	}

	static void convert( PyObject* object, ssize_t& integral )
	{
		integral = PyLong_AsSsize_t( object );
	}

	static void convert( PyObject* object, size_t& integral )
	{
		integral = PyLong_AsSize_t( object );
	}
};

template< typename t >
	struct from< t, typename enable_if< is_integral< t > >::type >
{
	static PyObject* call( typename call_traits< t >::param_type integral )
	{
		return convert( integral );
	}

	static PyObject* convert( long integral )
	{
		return PyLong_FromLong( integral );
	}

	static PyObject* convert( unsigned long integral )
	{
		return PyLong_FromUnsignedLong( integral );
	}

	static PyObject* convert( long long integral )
	{
		return PyLong_FromLongLong( integral );
	}

	static PyObject* convert( unsigned long long integral )
	{
		return PyLong_FromUnsignedLongLong( integral );
	}

	static PyObject* convert( ssize_t integral )
	{
		return PyLong_FromSsize_t( integral );
	}

	static PyObject* convert( size_t integral )
	{
		return PyLong_FromSize_t( integral );
	}
};

//--- traits: pointer ------------------------------------------------------------------------------
template< typename t >
	struct as< t, typename enable_if< is_pointer< t > >::type >
{
	static void call( PyObject* object, typename call_traits< t >::reference pointer )
	{
		if ( !PyCapsule_CheckExact( object ) )
			throw error::python() << "Object is not a capsule";

		pointer = ptr_cast< typename no_ref< t >::type >( PyCapsule_GetPointer( object, 0 ) );
	}
};

template< typename t >
	struct from< t, typename enable_if< is_pointer< t > >::type >
{
	static PyObject* call( typename call_traits< t >::param_type pointer )
	{
		return PyCapsule_New( pointer, 0, 0 );
	}
};

OOE_NAMESPACE_END( ( ooe )( python ) )

#endif	// OOE_COMPONENT_PYTHON_TRAITS_FORWARD_HPP
