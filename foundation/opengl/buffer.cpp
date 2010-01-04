/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl/buffer.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;

	u32 gl_type( buffer::type type )
	{
		switch ( type )
		{
		case buffer::point:
			return opengl::ARRAY_BUFFER;

		case buffer::index:
			return opengl::ELEMENT_ARRAY_BUFFER;

		case buffer::pixel_pack:
			return opengl::PIXEL_PACK_BUFFER;

		case buffer::pixel_unpack:
			return opengl::PIXEL_UNPACK_BUFFER;

		default:
			throw error::runtime( "opengl: " ) << "Unknown buffer type: " << type;
		}
	}
}

namespace ooe
{
//--- opengl::buffer -----------------------------------------------------------
	opengl::buffer::buffer( type value )
		: target( gl_type( value ) ), id( 0 )
	{
		GenBuffers( 1, &id );
	}

	opengl::buffer::~buffer( void )
	{
		DeleteBuffers( 1, &id );
	}

	void opengl::buffer::bind( void )
	{
		BindBuffer( target, id );
	}

	void opengl::buffer::unbind( void )
	{
		BindBuffer( target, 0 );
	}

	void* opengl::buffer::map( map_type type_ )
	{
		u32 mode;

		switch ( type_ )
		{
		case read:
			mode = READ_ONLY;
			break;

		case write:
			mode = WRITE_ONLY;
			break;

		case read_write:
			mode = READ_WRITE;
			break;

		default:
			throw error::runtime( "opengl: " ) << "Unknown map type: " << type_;
		}

		return MapBuffer( target, mode );
	}

	void opengl::buffer::unmap( void )
	{
		if ( !UnmapBuffer( target ) )
			throw error::runtime( "opengl: " ) << "Unable to unmap buffer";
	}

	void opengl::buffer::load( const void* data, up_t size, load_type type_ )
	{
		u32 usage;

		switch ( type_ )
		{
		case stream_draw:
			usage = STREAM_DRAW;
			break;

		case stream_read:
			usage = STREAM_READ;
			break;

		case stream_copy:
			usage = STREAM_COPY;
			break;

		case static_draw:
			usage = STATIC_DRAW;
			break;

		case static_read:
			usage = STATIC_READ;
			break;

		case static_copy:
			usage = STATIC_COPY;
			break;

		case dynamic_draw:
			usage = DYNAMIC_DRAW;
			break;

		case dynamic_read:
			usage = DYNAMIC_READ;
			break;

		case dynamic_copy:
			usage = DYNAMIC_COPY;
			break;

		case reload:
			return BufferSubData( target, 0, size, data );

		default:
			throw error::runtime( "opengl: " ) << "Unknown load type: " << type_;
		}

		BufferData( target, size, data, usage );
	}
}
