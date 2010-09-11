/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_TEXTURE_HPP
#define OOE_FOUNDATION_OPENGL_TEXTURE_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- texture_id -----------------------------------------------------------------------------------
class texture_id
{
public:
	const u32 id;

protected:
	texture_id( void );
	virtual ~texture_id( void );
};

//--- compressed_id --------------------------------------------------------------------------------
class compressed_id
{
protected:
	const u32 internal;
	const u32 size;

	compressed_id( u32, u32, image::type );
};

//--- uncompressed_id ------------------------------------------------------------------------------
class uncompressed_id
{
protected:
	const u32 external;
	const u32 data_type;

	uncompressed_id( image::type );
};

//--- texture --------------------------------------------------------------------------------------
class texture
	: public ooe::texture, public texture_id
{
public:
	const u32 width;
	const u32 height;

protected:
	const image::type format;
	const u8 levels;
	const bool generate_mipmap;

	texture( const image_pyramid&, texture::type, bool );
	void verify( const image&, u32, u32, u8 ) const;
};

struct compressed_texture
	: public texture, private compressed_id
{
	compressed_texture( const image_pyramid&, texture::type, bool );
	virtual void write( const image&, u32, u32, u8 );
};

struct uncompressed_texture
	: public texture, private uncompressed_id
{
	uncompressed_texture( const image_pyramid&, texture::type, bool );
	virtual void write( const image&, u32, u32, u8 );
};

//--- texture_array --------------------------------------------------------------------------------
class texture_array
	: public ooe::texture_array, public texture_id
{
protected:
	const u32 width;
	const u32 height;
	const u32 depth;
	const image::type format;

	texture_array( u32, u32, u32, image::type );
	void verify( const image&, u32, u32, u32 ) const;
};

struct compressed_texture_array
	: public texture_array, private compressed_id
{
	compressed_texture_array( u32, u32, u32, image::type );
	virtual void write( const image&, u32, u32, u32 );
};

struct uncompressed_texture_array
	: public texture_array, private uncompressed_id
{
	uncompressed_texture_array( u32, u32, u32, image::type );
	virtual void write( const image&, u32, u32, u32 );
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_TEXTURE_HPP
