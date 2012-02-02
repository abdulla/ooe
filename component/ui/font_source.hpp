/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_FONT_SOURCE_HPP
#define OOE_COMPONENT_UI_FONT_SOURCE_HPP

#include "component/ui/virtual_texture.hpp"
#include "foundation/parallel/lock.hpp"
#include "foundation/utility/fixed_cache.hpp"
#include "foundation/visual/font.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- font_source ----------------------------------------------------------------------------------
class font_source
    : public physical_source
{
public:
    typedef tuple< font::metric, u32, u32 > glyph_type;

    font_source( const font::face&, u16, const std::string& );
    virtual ~font_source( void );

    virtual u32 size( void ) const;
    virtual u16 page_size( void ) const;
    virtual image_format::type format( void ) const;

    u16 font_size( void ) const;
    u32 glyph_index( u32 ) const;
    f32 kerning( u32, u32, u8 ) const;
    glyph_type glyph( u32, u8 ) const;

private:
    typedef fixed_cache< u32, u32 > index_cache_type;
    typedef fixed_cache< tuple< u32, u32 >, f32 > kerning_cache_type;

    mutable ooe::mutex mutex;
    const font::face& face;
    const u16 face_size;
    const std::string root;

    const u32 source_size;
    const u32 glyphs;
    const u8 level_limit;

    mutable index_cache_type index_cache;
    mutable kerning_cache_type kerning_cache;
    ooe::memory memory;

    virtual image read( const pyramid_index& );
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_FONT_SOURCE_HPP
