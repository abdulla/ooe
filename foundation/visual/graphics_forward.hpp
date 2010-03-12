/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VISUAL_GRAPHICS_FORWARD_HPP
#define OOE_FOUNDATION_VISUAL_GRAPHICS_FORWARD_HPP

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- texture_handle -------------------------------------------------------------------------------
struct texture_handle
{
	typedef std::vector< shared_free< void > > vector_type;

	const u32 width;
	const u32 height;
	const image::type format;
	vector_type vector;

	texture_handle( u32, u32, image::type );
};

//--- variable_handle ------------------------------------------------------------------------------
struct variable_handle
{
	typedef u8 data_type[ sizeof( f32 ) * 4 * 4 ];
	typedef tuple< std::string, variable::type, data_type > tuple_type;
	typedef std::vector< tuple_type > vector_type;

	vector_type vector;
};

//--- shader_handle --------------------------------------------------------------------------------
struct shader_handle
{
	shader::type type;
	std::string text;

	shader_handle( shader::type, const std::string& );
};

//--- buffer_handle --------------------------------------------------------------------------------
struct buffer_handle
{
	buffer::type type;
	up_t size;
	scoped_array< u8 > data;

	buffer_handle( buffer::type, const void*, up_t );
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_FOUNDATION_VISUAL_GRAPHICS_FORWARD_HPP
