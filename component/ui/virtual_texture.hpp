/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP
#define OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP

#include <bitset>
#include <list>
#include <map>

#include "foundation/utility/tuple.hpp"
#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

class virtual_texture;

//--- physical_source ------------------------------------------------------------------------------
struct physical_source
{
	virtual ~physical_source( void ) {}

	virtual u32 size( void ) const = 0;
	virtual image::type format( void ) const = 0;
	virtual u16 page_size( void ) const = 0;

	virtual image read( u32, u32, u8 ) = 0;
};

//--- virtual_texture ------------------------------------------------------------------------------
class virtual_texture
{
public:
	typedef tuple< u32, u32, u8 > key_type;

	virtual_texture( const device_type&, physical_source& );

	void input( const std::string&, block_type& ) const;
	void load( u32, u32, u32, u32, u8, bool = false );
	void unlock( u32, u32, u32, u32, u8 );
	void write( void );

private:
	typedef tuple< u32, u32, key_type, bool > value_type;
	typedef std::list< value_type > cache_list;
	typedef std::map< key_type, cache_list::iterator > cache_map;
	typedef std::bitset< sizeof( up_t ) > cache_bitset;

	physical_source& source;
	const u32 table_size;
	const u32 cache_size;

	image_pyramid pyramid;
	texture_type table;
	texture_type cache;

	cache_list list;
	cache_map map;
	cache_bitset bitset;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP
