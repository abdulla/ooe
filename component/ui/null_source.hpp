/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_NULL_SOURCE_HPP
#define OOE_COMPONENT_UI_NULL_SOURCE_HPP

#include "component/ui/virtual_texture.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- null_source ----------------------------------------------------------------------------------
class null_source
    : public physical_source
{
public:
    null_source( u32 );
    virtual ~null_source( void );

    virtual u32 size( void ) const;
    virtual u16 page_size( void ) const;
    virtual image_format::type format( void ) const;

private:
    const u32 size_;

    virtual image read( const pyramid_index& );
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_NULL_SOURCE_HPP
