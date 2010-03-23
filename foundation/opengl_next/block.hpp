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
	typedef tuple< s32, texture_type > in_texture_tuple;
	typedef std::vector< in_texture_tuple > in_texture_vector;
	typedef tuple< s32, u8 > in_buffer_tuple;
	typedef std::multimap< buffer_type, in_buffer_tuple > in_buffer_map;
	typedef tuple< s32, attachment_type, texture_type > out_texture_tuple;
	typedef std::vector< out_texture_tuple > out_texture_vector;
	typedef tuple< s32, attachment_type, target_type > out_target_tuple;
	typedef std::vector< out_target_tuple > out_target_vector;
	typedef std::set< std::string > name_set;

	const u32 id;
	const buffer_type index;
	in_texture_vector in_textures;
	in_buffer_map in_buffers;
	out_texture_vector out_textures;
	out_target_vector out_targets;

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

	virtual void output( const std::string&, attachment_type, const texture_type& );
	virtual void output( const std::string&, attachment_type, const target_type& );

private:
	name_set names;
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_BLOCK_HPP
