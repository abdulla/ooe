/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP
#define OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP

#include <list>

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- physical_source ------------------------------------------------------------------------------
struct physical_source
{
	virtual ~physical_source( void ) {}

	virtual u32 size( void ) const = 0;
	virtual image::type format( void ) const = 0;
	virtual u16 page_size( void ) const = 0;

	virtual image read( u32, u32, u8 ) = 0;
};

//--- physical_cache -------------------------------------------------------------------------------
class physical_cache
{
public:
	typedef tuple< f32, f32 > write_tuple;

	physical_cache( const device_type&, image::type, u16 );

	image::type format( void ) const;
	u16 page_size( void ) const;
	texture_type texture( void ) const;

	write_tuple write( const image&, bool );

private:
	typedef tuple< u32 /* x */, u32 /* y */, bool /* locked */ > cache_tuple;
	typedef std::list< cache_tuple > cache_type;

	const image::type format_;
	const u16 page_size_;
	const u32 cache_size;

	texture_type cache;
	cache_type free_list;
	cache_type used_list;
};

//--- virtual_texture ------------------------------------------------------------------------------
class virtual_texture
{
public:
	virtual_texture( const device_type&, physical_cache&, physical_source& );

	shader_type shader( void ) const;
	texture_type texture( void ) const;

	void load( u32, u32, u32, u32, u8 );

private:
	physical_cache& cache;
	physical_source& source;
	const u32 table_size;
	const u8 level_limit;

	shader_type shader_;
	texture_type table;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP
