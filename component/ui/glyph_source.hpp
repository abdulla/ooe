/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_GLYPH_SOURCE_HPP
#define OOE_COMPONENT_UI_GLYPH_SOURCE_HPP

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- glyph_coord ----------------------------------------------------------------------------------
struct glyph_coord
{
    const font::metric& metric;
    const rect& rect;

    glyph_coord( const font::metric&, const rect& );
};

//--- glyph_source ---------------------------------------------------------------------------------
class glyph_source
    : public physical_source
{
public:
    glyph_source( const device_ptr&, const font::face&, const std::string& );
    virtual ~glyph_source( void );

    virtual u32 size( void ) const;
    virtual u16 page_size( void ) const;
    virtual image_format::type format( void ) const;

    u32 glyph_index( u32 ) const;
    f32 kerning( u32, u32 ) const;
    glyph_coord glyph_coord( u32, u16 ) const;

private:
    typedef fixed_cache< u32, u32 > index_type;
    typedef fixed_cache< tuple< u32, u32 >, f32 > kerning_type;
    typedef std::tr1::unordered_map< tuple< u32, u16 >, const rect* > coord_type;

    mutable ooe::mutex mutex;
    const font::face& face;
    const std::string root;

    ooe::memory memory;
    mutable index_type index_cache;
    mutable kerning_type kerning_cache;
    mutable coord_type coord_map;
    ooe::bin bin;

    u32 size_;

    virtual image read( const pyramid_index& );
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_GLYPH_SOURCE_HPP
