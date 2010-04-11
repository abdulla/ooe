/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_NULL_SOURCE_HPP
#define OOE_COMPONENT_UI_NULL_SOURCE_HPP

#include "component/ui/virtual_texture.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- null_source ----------------------------------------------------------------------------------
class null_source
	: public physical_source
{
public:
	null_source( u32 ) OOE_VISIBLE;
	virtual ~null_source( void );

	virtual u32 size( void ) const;
	virtual image::type format( void ) const;
	virtual u16 page_size( void ) const;

	virtual image read( u32, u32, u8 );

private:
	const u32 size_;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_COMPONENT_UI_NULL_SOURCE_HPP
