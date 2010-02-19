/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_PYTHON_TRAITS_FORWARD_HPP
#define OOE_COMPONENT_PYTHON_TRAITS_FORWARD_HPP

#include "component/python/error.hpp"
#include "component/python/header.hpp"
#include "component/python/object.hpp"
#include "component/registry/traits.hpp"
#include "foundation/utility/miscellany.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

//--- as -------------------------------------------------------------------------------------------
template< typename, typename = void >
	struct as;

template< typename t >
	struct as< t, typename enable_if< is_empty< t > >::type >;

template< typename t >
	struct as< t, typename enable_if< component::is_boolean< t > >::type >;

template< typename t >
	struct as< t, typename enable_if< is_string< t > >::type >;

template< typename t >
	struct as< t, typename enable_if< component::is_integral< t > >::type >;

template< typename t >
	struct as< t, typename enable_if< component::is_floating_point< t > >::type >;

template< typename t >
	struct as< t, typename enable_if< component::is_pointer< t > >::type >;

template< typename t >
	struct as< t, typename enable_if< component::is_class< t > >::type >;

template< typename t >
	struct as< t, typename enable_if< is_construct< t > >::type >;

template< typename t >
	struct as< t, typename enable_if< is_destruct< t > >::type >;

template< typename t >
	struct as< t, typename enable_if< is_array< t > >::type >;

//--- from -----------------------------------------------------------------------------------------
template< typename, typename = void >
	struct from;

template< typename t >
	struct from< t, typename enable_if< is_empty< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< component::is_boolean< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< is_string< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< component::is_integral< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< component::is_floating_point< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< component::is_pointer< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< component::is_class< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< is_construct< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< is_destruct< t > >::type >;

template< typename t >
	struct from< t, typename enable_if< is_array< t > >::type >;

//--- destroy --------------------------------------------------------------------------------------
template< typename t >
	void destroy( PyObject* object )
{
	delete ptr_cast< t* >( PyCapsule_GetPointer( object, 0 ) );
}

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
	struct as< t, typename enable_if< component::is_boolean< t > >::type >
{
	static void call( PyObject* object, bool& boolean )
	{
		if ( !PyBool_Check( object ) )
			throw error::python() << "Object is not a boolean";

		boolean = object == Py_True;
	}
};

template< typename t >
	struct from< t, typename enable_if< component::is_boolean< t > >::type >
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
		if ( !PyUnicode_Check( object ) )
			throw error::python() << "Object is not a string";

		sp_t size;
		const c8* data = _PyUnicode_AsStringAndSize( object, &size );
		string = string_make< typename no_ref< t >::type >( data, size );
	}
};

template< typename t >
	struct from< t, typename enable_if< is_string< t > >::type >
{
	static PyObject* call( typename call_traits< t >::param_type string )
	{
		return valid( PyUnicode_FromStringAndSize( string_data( string ), string_size( string ) ) );
	}
};

//--- traits: integral -----------------------------------------------------------------------------
template< typename t >
	struct as< t, typename enable_if< component::is_integral< t > >::type >
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
		integral = PyLong_AsUnsignedLongLong( object );
	}
};

template< typename t >
	struct from< t, typename enable_if< component::is_integral< t > >::type >
{
	static PyObject* call( typename call_traits< t >::param_type integral )
	{
		return valid( convert( integral ) );
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
};

//--- traits: floating-point -----------------------------------------------------------------------
template< typename t >
	struct as< t, typename enable_if< component::is_floating_point< t > >::type >
{
	static void call( PyObject* object, typename call_traits< t >::reference floating_point )
	{
		if ( !PyFloat_Check( object ) )
			throw error::python() << "Object is not a floating-point number";

		floating_point = PyFloat_AS_DOUBLE( object );
	}
};

template< typename t >
	struct from< t, typename enable_if< component::is_floating_point< t > >::type >
{
	static PyObject* call( typename call_traits< t >::param_type floating_point )
	{
		return valid( PyFloat_FromDouble( floating_point ) );
	}
};

//--- traits: pointer ------------------------------------------------------------------------------
template< typename t >
	struct as< t, typename enable_if< component::is_pointer< t > >::type >
{
	static void call( PyObject* object, typename call_traits< t >::reference pointer )
	{
		if ( !PyCapsule_CheckExact( object ) )
			throw error::python() << "Object is not a capsule";

		pointer = ptr_cast< typename no_ref< t >::type >( PyCapsule_GetPointer( object, 0 ) );
	}
};

template< typename t >
	struct from< t, typename enable_if< component::is_pointer< t > >::type >
{
	static PyObject* call( typename call_traits< t >::param_type pointer )
	{
		return valid( PyCapsule_New( ptr_cast( pointer ), 0, 0 ) );
	}
};

//--- traits: class --------------------------------------------------------------------------------
template< typename t >
	struct as< t, typename enable_if< component::is_class< t > >::type >
{
	static void call( PyObject* object, typename call_traits< t >::reference class_ )
	{
		if ( !PyCapsule_CheckExact( object ) )
			throw error::python() << "Object is not a capsule";

		typedef typename no_ref< t >::type* pointer;
		class_ = *ptr_cast< pointer >( PyCapsule_GetPointer( object, 0 ) );
	}
};

template< typename t >
	struct from< t, typename enable_if< component::is_class< t > >::type >
{
	static PyObject* call( typename call_traits< t >::param_type class_ )
	{
		typedef typename no_ref< t >::type type;
		return valid( PyCapsule_New( new type( class_ ), 0, destroy< type > ) );
	}
};

//--- traits: construct ----------------------------------------------------------------------------
template< typename INVALID_USAGE >
	struct as< INVALID_USAGE, typename enable_if< is_construct< INVALID_USAGE > >::type >
{
	static void call( PyObject* object, INVALID_USAGE )
	{
		BOOST_STATIC_ASSERT( !sizeof( INVALID_USAGE ) );
	}
};

template< typename t >
	struct from< t, typename enable_if< is_construct< t > >::type >
{
	static PyObject* call( typename call_traits< t >::param_type construct )
	{
		return valid( PyCapsule_New( construct, 0, destroy< typename t::value_type > ) );
	}
};

//--- traits: destruct -----------------------------------------------------------------------------
template< typename t >
	struct as< t, typename enable_if< is_destruct< t > >::type >
{
	static void call( PyObject* object, typename call_traits< t >::reference destruct )
	{
		typedef typename t::pointer pointer;
		pointer p;
		as< pointer >::call( object, p );
		destruct = p;

		PyCapsule_SetDestructor( object, 0 );
	}
};

template< typename INVALID_USAGE >
	struct from< INVALID_USAGE, typename enable_if< is_destruct< INVALID_USAGE > >::type >
{
	static PyObject* call( INVALID_USAGE )
	{
		BOOST_STATIC_ASSERT( !sizeof( INVALID_USAGE ) );
		return 0;
	}
};

//--- traits: array --------------------------------------------------------------------------------
template< typename t >
	struct as< t, typename enable_if< is_array< t > >::type >
{
	static void call( PyObject* object, typename call_traits< t >::reference array )
	{
		if ( !PyList_Check( object ) )
			throw error::python() << "Object is not a list";

		typedef typename no_ref< t >::type type;
		up_t py_size = PyList_GET_SIZE( object );
		up_t array_size = extent< type >::value;

		if ( py_size != array_size )
			throw error::python() << "Array in python is of size " << py_size <<
				", array is of size " << array_size;

		for ( up_t i = 0; i != array_size; ++i )
			as< typename remove_extent< type >::type >::
				call( PyList_GET_ITEM( object, i ), array[ i ] );
	}
};

template< typename t >
	struct from< t, typename enable_if< is_array< t > >::type >
{
	static PyObject* call( typename call_traits< t >::param_type array )
	{
		typedef typename no_ref< t >::type type;
		up_t array_size = extent< type >::value;
		python::object object = valid( PyList_New( array_size ) );

		for ( up_t i = 0; i != array_size; ++i )
			PyList_SET_ITEM( object, i,
				from< typename remove_extent< type >::type >::call( array[ i ] ) );

		return object.release();
	}
};

OOE_NAMESPACE_END( ( ooe )( python ) )

#endif	// OOE_COMPONENT_PYTHON_TRAITS_FORWARD_HPP
