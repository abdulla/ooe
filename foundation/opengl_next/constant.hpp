/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_NEXT_CONSTANT_HPP
#define OOE_FOUNDATION_OPENGL_NEXT_CONSTANT_HPP

#include <map>

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

class constant
	: public ooe::constant
{
public:
	typedef std::map< std::string, s32 > map_type;

	constant( u32 );
	virtual void insert( const std::string&, f32 );
	virtual void insert( const std::string&, const slang_vec2& );
	virtual void insert( const std::string&, const slang_vec3& );
	virtual void insert( const std::string&, const slang_vec4& );
	virtual void insert( const std::string&, s32 );
	virtual void insert( const std::string&, const slang_ivec2& );
	virtual void insert( const std::string&, const slang_ivec3& );
	virtual void insert( const std::string&, const slang_ivec4& );
	virtual void insert( const std::string&, const slang_mat2& );
	virtual void insert( const std::string&, const slang_mat3& );
	virtual void insert( const std::string&, const slang_mat4& );

private:
	const u32 id;
	map_type map;
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif	// OOE_FOUNDATION_OPENGL_NEXT_CONSTANT_HPP
