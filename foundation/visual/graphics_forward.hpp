/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VISUAL_GRAPHICS_FORWARD_HPP
#define OOE_FOUNDATION_VISUAL_GRAPHICS_FORWARD_HPP

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- texture_generic ------------------------------------------------------------------------------
struct texture_generic
{
	typedef std::vector< shared_free< void > > vector_type;

	const u32 width;
	const u32 height;
	const image::type format;
	const texture::type filter;
	bool generate_mipmap;
	vector_type vector;

	texture_generic( u32, u32, image::type, texture::type );
};

template< typename type >
	void texture_verify( const type& texture, const image& image, u8 level )
{
	u32 width = texture.width >> level;
	u32 height = texture.height >> level;

	if ( !width || !height )
		throw error::runtime( "texture: " ) << "Invalid mipmap level: " << level;
	else if ( image.width != width || image.height != width )
		throw error::runtime( "texture: " ) <<
			"Image size does not match texture size for mipmap level " << level << ": expected " <<
			width << 'x' << height << ", got " << image.width << 'x' << image.height;
	else if ( image.format != texture.format )
		throw error::runtime( "texture: " ) << "Image format does not match texture format: " <<
			image.format << " != " << texture.format;
}

//--- variable_generic -----------------------------------------------------------------------------
struct variable_generic
{
	typedef u8 data_type[ sizeof( f32 ) * 4 * 4 ];
	typedef tuple< std::string, variable::type, data_type > tuple_type;
	typedef std::vector< tuple_type > vector_type;

	vector_type vector;
};

//--- shader_generic -------------------------------------------------------------------------------
struct shader_generic
{
	shader::type type;
	std::string text;

	shader_generic( shader::type, const std::string& );
};

//--- buffer_generic -------------------------------------------------------------------------------
struct buffer_generic
{
	buffer::type type;
	up_t size;
	scoped_array< u8 > data;

	buffer_generic( buffer::type, const void*, up_t );
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_FOUNDATION_VISUAL_GRAPHICS_FORWARD_HPP
