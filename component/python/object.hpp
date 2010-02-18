/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_PYTHON_OBJECT_HPP
#define OOE_COMPONENT_PYTHON_OBJECT_HPP

#include "component/python/error.hpp"
#include "component/python/header.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

inline PyObject* valid( PyObject* object )
{
	if ( !object )
		throw error::python() << "Invalid python object";

	return object;
}

class object
{
public:
	object( void )
		: py_object( 0 )
	{
	}

	object( PyObject* py_object_ )
		: py_object( py_object_ )
	{
	}

	object( const object& other )
		: py_object( other.py_object )
	{
		Py_XINCREF( py_object );
	}

	~object( void )
	{
		Py_XDECREF( py_object );
	}

	object& operator =( const object& other )
	{
		Py_XINCREF( other.py_object );
		Py_XDECREF( py_object );
		py_object = other.py_object;
		return *this;
	}

	object str( void ) const
	{
		return valid( PyObject_Str( py_object ) );
	}

	PyObject** operator &( void )
	{
		return &py_object;
	}

	operator PyObject*( void )
	{
		return py_object;
	}

private:
	PyObject* py_object;
};

OOE_NAMESPACE_END( ( ooe )( python ) )

#endif	// OOE_COMPONENT_PYTHON_OBJECT_HPP
