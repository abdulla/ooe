/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_NEXT_TEXTURE_HPP
#define OOE_FOUNDATION_OPENGL_NEXT_TEXTURE_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

class texture
	: public ooe::texture
{
public:
	const u32 id;

	texture( const image_pyramid& );
	virtual ~texture( void );

	void verify( const image&, u32, u32, u8 ) const;

protected:
	const u32 width;
	const u32 height;
	const image::type format;
	const u32 levels;
};

class compressed_texture
	: public texture
{
public:
	compressed_texture( const image_pyramid&, texture::type, bool );

	virtual void load( const image&, u32, u32, u8 );

private:
	const u32 internal;
	const u32 size;
};

class uncompressed_texture
	: public texture
{
public:
	uncompressed_texture( const image_pyramid&, texture::type, bool );

	virtual void load( const image&, u32, u32, u8 );

private:
	const u32 external;
	const u32 type;
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_TEXTURE_HPP
