/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_NEXT_BUFFER_HPP
#define OOE_FOUNDATION_OPENGL_NEXT_BUFFER_HPP

#include <map>

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

class block
	: public ooe::block
{
public:
	typedef std::map< std::string, s32 > uniform_map;
	typedef tuple< s32, u8, u8, buffer_type > buffer_tuple;
	typedef std::map< std::string, buffer_tuple > buffer_map;
	typedef tuple< s32, texture_type > texture_tuple;
	typedef std::map< std::string, texture_tuple > texture_map;

	block( u32 );
	virtual ~block( void );

	virtual void input( const std::string&, s32 );
	virtual void input( const std::string&, s32, s32, s32 );
	virtual void input( const std::string&, f32 );
	virtual void input( const std::string&, f32, f32, f32 );
	virtual void input( const std::string&, const mat3& );
	virtual void input( const std::string&, const mat4& );

	virtual void input( const std::string&, const buffer_type&, u8, u8 );
	virtual void input( const std::string&, const texture_type& );

	virtual void output( const std::string&, const target_type&, attachment_type );
	virtual void output( const std::string&, const texture_type&, attachment_type );

private:
	u32 id;
	uniform_map uniforms;
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_BUFFER_HPP
