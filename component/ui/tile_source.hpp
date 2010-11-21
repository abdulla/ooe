/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_TILE_SOURCE_HPP
#define OOE_COMPONENT_UI_TILE_SOURCE_HPP

#include "component/ui/virtual_texture.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- tile_source ----------------------------------------------------------------------------------
class tile_source
    : public physical_source
{
public:
    tile_source( const std::string& );
    virtual ~tile_source( void );

    virtual u32 size( void ) const;
    virtual u16 page_size( void ) const;
    virtual image_format::type format( void ) const;

private:
    const std::string root;
    std::string type;
    decoder_type decoder;

    u32 size_;
    u16 page_size_;
    image_format::type format_;
    u8 level_limit;

    virtual image read( const pyramid_index& );
};

//--- make_tile ------------------------------------------------------------------------------------
void make_tile( const descriptor&, thread_pool&, const std::string&, u16 );

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_TILE_SOURCE_HPP
