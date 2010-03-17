/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_NEXT_BUFFER_HPP
#define OOE_FOUNDATION_OPENGL_NEXT_BUFFER_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

class map
	: public ooe::map
{
public:
	map( void*, up_t, u32 );
	virtual ~map( void );

private:
	const u32 target;
};

class buffer
	: public ooe::buffer
{
public:
	const u32 id;

	buffer( up_t, type, usage_type );
	virtual ~buffer( void );

	virtual map_type map( access_type ) const;

private:
	const u32 target;
	const up_t size;
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_BUFFER_HPP
