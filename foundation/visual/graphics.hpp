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
	virtual void load( const image&, u32, u32, u8 = 0 ) = 0;
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
};

typedef shared_ptr< shader > shader_type;

//--- slang_type -----------------------------------------------------------------------------------
template< typename, typename, u8 >
	struct slang_type;

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
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

//--- variable -------------------------------------------------------------------------------------
struct variable
{
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

	virtual ~variable( void ) {};

	virtual void insert( const std::string&, f32 ) = 0;
	virtual void insert( const std::string&, s32 ) = 0;
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
		pixel_unpack
	};

	buffer( type, const void*, up_t );

protected:
	opaque_ptr pointer;
};

//--- frame ----------------------------------------------------------------------------------------
class frame
{
public:
	frame( void );

protected:
	opaque_ptr pointer;
};

//--- batch ----------------------------------------------------------------------------------------
class batch
{
public:
	void insert( const texture& );
	void insert( const shader& );
	void insert( const buffer& );

protected:
	typedef std::vector< texture > texture_vector;
	typedef std::vector< shader > shader_vector;
	typedef std::vector< buffer > buffer_vector;

	texture_vector textures;
	shader_vector shaders;
	buffer_vector buffers;
};

//--- driver ---------------------------------------------------------------------------------------
struct driver
{
	virtual ~driver( void ) {}
	virtual void draw( frame&, const batch& ) = 0;
	virtual void swap( void ) = 0;

	virtual texture_type
		texture( const image_pyramid&, texture::type = texture::linear, bool = true ) = 0;
	virtual shader_type shader( const std::string&, shader::type );
};

typedef shared_ptr< driver > driver_type;

OOE_NAMESPACE_END( ( ooe ) )

	#endif	// OOE_FOUNDATION_VISUAL_GRAPHICS_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()
	#define ASSIGN_PARAMETER( z, n, _ ) data[ n ] = a ## n;
	#define ASSIGN_ARRAY( z, n, _ ) data[ n ] = array[ n ];

#if LIMIT

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

#endif

	#undef ASSIGN_ARRAY
	#undef ASSIGN_PARAMETER
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
