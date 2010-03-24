/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_NEXT_BLOCK_HPP
#define OOE_FOUNDATION_OPENGL_NEXT_BLOCK_HPP

#include <map>
#include <set>

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

class block
	: public ooe::block
{
public:
	typedef u8 uniform_data[ sizeof( mat4 ) ];
	typedef void ( * uniform_function )( s32, const uniform_data );
	typedef tuple< s32, uniform_data, uniform_function > uniform_tuple;
	typedef std::vector< uniform_tuple > uniform_vector;
	typedef tuple< s32, texture_type > texture_tuple;
	typedef std::vector< texture_tuple > texture_vector;
	typedef tuple< s32, u8 > buffer_tuple;
	typedef std::multimap< buffer_type, buffer_tuple > buffer_map;
	typedef std::set< std::string > name_set;

	const u32 id;
	const buffer_type index;

	uniform_vector uniforms;
	texture_vector textures;
	buffer_map buffers;

	block( u32, const buffer_type& );
	virtual ~block( void );

	virtual void input( const std::string&, s32 );
	virtual void input( const std::string&, s32, s32, s32 );
	virtual void input( const std::string&, f32 );
	virtual void input( const std::string&, f32, f32, f32 );
	virtual void input( const std::string&, const mat3& );
	virtual void input( const std::string&, const mat4& );

	virtual void input( const std::string&, const texture_type& );
	virtual void input( const std::string&, u8, const buffer_type& );

private:
	name_set names;
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_BLOCK_HPP
