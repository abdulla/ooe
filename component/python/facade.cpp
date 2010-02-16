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
