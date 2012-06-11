/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VISUAL_FONT_HPP
#define OOE_FOUNDATION_VISUAL_FONT_HPP

#include "foundation/image/image.hpp"
#include "foundation/io/memory.hpp"

typedef struct FT_FaceRec_* FT_Face;
typedef struct FT_LibraryRec_* FT_Library;

OOE_NAMESPACE_BEGIN( ( ooe )( font ) )

//--- library --------------------------------------------------------------------------------------
struct OOE_VISIBLE library
    : private noncopyable
{
public:
    library( void );
    ~library( void );

private:
    FT_Library freetype;

    friend class face;
};

//--- metric ---------------------------------------------------------------------------------------
struct metric
{
    f32 width;
    f32 height;
    f32 left;
    f32 top;
    f32 advance;

    metric( f32, f32, f32, f32, f32 );
};

//--- bitmap ---------------------------------------------------------------------------------------
struct bitmap
{
    u16 width;
    u16 height;
    s32 pitch;
    const u8* data;

    bitmap( u16, u16, s32, const u8* );
};

//--- face -----------------------------------------------------------------------------------------
class OOE_VISIBLE face
    : private noncopyable
{
public:
    enum bitmap_type
    {
        red,
        green,
        blue
    };

    face( const library&, const descriptor& );
    ~face( void );

    std::string family( void ) const;
    std::string style( void ) const;
    up_t size( void ) const;

    u32 glyph_index( u32 ) const;
    f32 kerning( u32, u32 ) const;
    font::metric metric( u32 ) const;
    font::bitmap bitmap( u32, u32, bitmap_type = red ) const;

private:
    ooe::memory memory;
    FT_Face face_;
};

OOE_NAMESPACE_END( ( ooe )( font ) )

#endif  // OOE_FOUNDATION_VISUAL_FONT_HPP
