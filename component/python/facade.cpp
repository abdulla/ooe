/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/python/facade.hpp"

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
	vector.insert( i, call );
}

OOE_NAMESPACE_END( ( ooe )( facade ) )

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

//--- throw_exception ------------------------------------------------------------------------------
void throw_exception( const c8* what, const c8* where )
{
	std::string string;
	string << what << "\n\nStack trace:" << where;
	PyErr_SetString( PyExc_RuntimeError, string.c_str() );
}

OOE_NAMESPACE_END( ( ooe )( python ) )
