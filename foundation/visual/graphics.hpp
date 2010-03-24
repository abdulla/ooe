/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VISUAL_GRAPHICS_HPP
#define OOE_FOUNDATION_VISUAL_GRAPHICS_HPP

#include <vector>

#include "foundation/image/image.hpp"
#include "foundation/maths/maths.hpp"
#include "foundation/utility/string.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- image_pyramid --------------------------------------------------------------------------------
class image_pyramid
{
public:
	typedef std::vector< shared_free< void > > vector_type;

	const u32 width;
	const u32 height;
	const image::type format;

	image_pyramid( const image& );
	image_pyramid( u32, u32, image::type );

	void push_back( const image& );
	const vector_type& get( void ) const;

private:
	vector_type vector;
};

//--- texture --------------------------------------------------------------------------------------
struct texture
{
	enum type
	{
		nearest,
		linear
	};

	virtual ~texture( void ) {}
	virtual void write( const image&, u32, u32, u8 = 0 ) = 0;
};

typedef shared_ptr< texture > texture_type;

//--- map ------------------------------------------------------------------------------------------
struct map
{
	void* const data;
	const up_t size;

	map( void*, up_t );
	virtual ~map( void ) {}
};

typedef shared_ptr< map > map_type;

//--- buffer ---------------------------------------------------------------------------------------
struct buffer
{
	enum type
	{
		point,
		index,
		pixel
	};

	enum usage_type
	{
		static_read,
		static_write,
		stream_read,
		stream_write
	};

	enum access_type
	{
		read,
		write,
		read_write
	};

	virtual ~buffer( void ) {}
	virtual map_type map( access_type ) const = 0;
};

typedef shared_ptr< buffer > buffer_type;

//--- target ---------------------------------------------------------------------------------------
struct target
{
	enum type
	{
		depth_u24 = image::compressed,
		depth_f32
	};

	virtual ~target( void ) {}
};

typedef shared_ptr< target > target_type;

//--- shader ---------------------------------------------------------------------------------------
struct shader
{
	enum type
	{
		vertex,
		fragment
	};

	virtual ~shader( void ) {}
};

typedef shared_ptr< shader > shader_type;
typedef std::vector< shader_type > shader_vector;

//--- block ----------------------------------------------------------------------------------------
struct block
{
	virtual ~block( void ) {}
	virtual void input( const std::string&, s32 ) = 0;
	virtual void input( const std::string&, s32, s32, s32 ) = 0;
	virtual void input( const std::string&, f32 ) = 0;
	virtual void input( const std::string&, f32, f32, f32 ) = 0;
	virtual void input( const std::string&, const mat3& ) = 0;
	virtual void input( const std::string&, const mat4& ) = 0;

	virtual void input( const std::string&, const texture_type& ) = 0;
	virtual void input( const std::string&, u8, const buffer_type& ) = 0;
};

typedef shared_ptr< block > block_type;

//--- frame ----------------------------------------------------------------------------------------
struct frame
{
	enum attachment_type
	{
		colour,
		depth
	};

	virtual ~frame( void ) {}
	virtual void output( const std::string&, attachment_type, const texture_type& ) = 0;
	virtual void output( const std::string&, attachment_type, const target_type& ) = 0;
};

typedef shared_ptr< frame > frame_type;

//--- program --------------------------------------------------------------------------------------
struct program
{
	virtual ~program( void ) {}
	virtual block_type block( const buffer_type& ) const = 0;
	virtual frame_type frame( void ) const = 0;
};

typedef shared_ptr< program > program_type;

//--- device ---------------------------------------------------------------------------------------
struct device
{
	virtual ~device( void ) {}
	virtual void draw( const block_type&, const frame_type& ) = 0;
	virtual void swap( void ) = 0;

	virtual texture_type texture
		( const image_pyramid&, texture::type = texture::linear, bool = true ) const = 0;
	virtual buffer_type buffer
		( up_t, buffer::type, buffer::usage_type = buffer::static_write ) const = 0;
	virtual target_type target( u32, u32, u8 ) const = 0;
	virtual shader_type shader( const std::string&, shader::type ) const = 0;
	virtual program_type program( const shader_vector& ) const = 0;
};

typedef shared_ptr< device > device_type;

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_FOUNDATION_VISUAL_GRAPHICS_HPP
