/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VIDEO_VIDEO_NG_HPP
#define OOE_FOUNDATION_VIDEO_VIDEO_NG_HPP

#include "foundation/image/image.hpp"
#include "foundation/utility/string.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- texture --------------------------------------------------------------------------------------
class texture
{
public:
	typedef void ( * function_type )( opaque_ptr&, const image&, u8 );

	texture( u32, u32, image::type );
	void load( const image&, u8 );

private:
	opaque_ptr pointer;
	function_type function;
};

//--- variable -------------------------------------------------------------------------------------
class variable
{
public:
	enum type
	{
		float_1,	// float
		float_2,	// vec2
		float_3,	// vec3
		float_4,	// vec4

		int_1,		// int
		int_2,		// ivec2
		int_3,		// ivec3
		int_4,		// ivec4

		mat_2,		// mat2
		mat_3,		// mat3
		mat_4		// mat4
	};

	typedef void ( * function_type )( opaque_ptr&, const std::string&, type, const void*, u8 );

	variable( void );
	void insert( const std::string&, type, const void*, u8 );

private:
	opaque_ptr pointer;
	function_type function;
};

//--- shader ---------------------------------------------------------------------------------------
class shader
{
public:
	enum type
	{
		vertex,
		fragment
	};

	shader( type, const std::string& );

private:
	opaque_ptr pointer;
};

//--- buffer ---------------------------------------------------------------------------------------
class buffer
{
public:
	enum type
	{
		point,
		index,
		pixel_pack,
		pixel_unpack,
		frame
	};

	buffer( type, const void*, up_t );

private:
	opaque_ptr pointer;
};

//--- batch ----------------------------------------------------------------------------------------
class batch
{
public:

private:
	typedef std::vector< shader > shader_vector;
	typedef std::vector< buffer > buffer_vector;

	shader_vector shaders;
	buffer_vector buffers;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_FOUNDATION_VIDEO_VIDEO_NG_HPP
