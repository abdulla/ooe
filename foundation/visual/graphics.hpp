/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_VISUAL_GRAPHICS_HPP
	#define OOE_FOUNDATION_VISUAL_GRAPHICS_HPP

#include <vector>

#include "foundation/image/image.hpp"
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

	void insert( const image& );
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

//--- slang_type -----------------------------------------------------------------------------------
template< typename, typename, u8 >
	struct slang_type;

	#define BOOST_PP_ITERATION_LIMITS ( 1, 16 )
	#define BOOST_PP_FILENAME_1 "foundation/visual/graphics.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

struct vec_tag;
struct ivec_tag;
struct mat_tag;

typedef slang_type< vec_tag, f32, 2 > slang_vec2;
typedef slang_type< vec_tag, f32, 3 > slang_vec3;
typedef slang_type< vec_tag, f32, 4 > slang_vec4;
typedef slang_type< ivec_tag, s32, 2 > slang_ivec2;
typedef slang_type< ivec_tag, s32, 3 > slang_ivec3;
typedef slang_type< ivec_tag, s32, 4 > slang_ivec4;
typedef slang_type< mat_tag, f32, 4 > slang_mat2;
typedef slang_type< mat_tag, f32, 9 > slang_mat3;
typedef slang_type< mat_tag, f32, 16 > slang_mat4;

//--- constant -------------------------------------------------------------------------------------
struct constant
{
	virtual ~constant( void ) {}
	virtual void insert( const std::string&, f32 ) = 0;
	virtual void insert( const std::string&, const slang_vec2& ) = 0;
	virtual void insert( const std::string&, const slang_vec3& ) = 0;
	virtual void insert( const std::string&, const slang_vec4& ) = 0;
	virtual void insert( const std::string&, s32 ) = 0;
	virtual void insert( const std::string&, const slang_ivec2& ) = 0;
	virtual void insert( const std::string&, const slang_ivec3& ) = 0;
	virtual void insert( const std::string&, const slang_ivec4& ) = 0;
	virtual void insert( const std::string&, const slang_mat2& ) = 0;
	virtual void insert( const std::string&, const slang_mat3& ) = 0;
	virtual void insert( const std::string&, const slang_mat4& ) = 0;
};

typedef shared_ptr< constant > constant_type;

//--- program --------------------------------------------------------------------------------------
struct program
{
	virtual ~program( void ) {}
	virtual constant_type constant( void ) const = 0;
};

typedef shared_ptr< program > program_type;

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

//--- batch ----------------------------------------------------------------------------------------
class batch
{
public:
	void insert( const texture& );
	void insert( const buffer& );

protected:
	typedef std::vector< texture > texture_vector;
	typedef std::vector< buffer > buffer_vector;

	texture_vector textures;
	buffer_vector buffers;
};

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

//--- frame ----------------------------------------------------------------------------------------
typedef shared_ptr< struct frame > frame_type;

struct frame
{
	enum type
	{
		create,
		system
	};

	virtual ~frame( void ) {}
	virtual void draw( const frame_type& ) = 0;
	virtual void draw( const batch& ) = 0;
};

//--- driver ---------------------------------------------------------------------------------------
struct driver
{
	virtual ~driver( void ) {}
	virtual void swap( void ) = 0;

	virtual texture_type texture
		( const image_pyramid&, texture::type = texture::linear, bool = true ) const = 0;
	virtual shader_type shader( const std::string&, shader::type ) const = 0;
	virtual program_type program( const shader_vector& ) const = 0;
	virtual buffer_type buffer
		( up_t, buffer::type, buffer::usage_type = buffer::static_write ) const = 0;
	virtual target_type target( u32, u32, u8 ) const = 0;
	virtual frame_type frame( frame::type = frame::create ) const = 0;
};

typedef shared_ptr< driver > driver_type;

OOE_NAMESPACE_END( ( ooe ) )

	#endif	// OOE_FOUNDATION_VISUAL_GRAPHICS_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()
	#define ASSIGN_PARAMETER( z, n, _ ) data[ n ] = a ## n;
	#define ASSIGN_ARRAY( z, n, _ ) data[ n ] = array[ n ];

//--- slang_type -----------------------------------------------------------------------------------
template< typename tag, typename type >
	struct slang_type< tag, type, LIMIT >
{
	type data[ LIMIT ];

	slang_type( BOOST_PP_ENUM_PARAMS( LIMIT, type a ) )
		: data()
	{
		BOOST_PP_REPEAT( LIMIT, ASSIGN_PARAMETER, ~ )
	}

	template< typename t >
		slang_type( const t& array )
		: data()
	{
		BOOST_PP_REPEAT( LIMIT, ASSIGN_ARRAY, ~ )
	}
};

	#undef ASSIGN_ARRAY
	#undef ASSIGN_PARAMETER
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
