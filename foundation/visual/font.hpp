/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

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
    f32 left;
    f32 top;
    f32 advance;
    u16 width;
    u16 height;

    metric( s32, s32, s32, u16, u16 );
};

//--- bitmap ---------------------------------------------------------------------------------------
struct bitmap
{
    font::metric metric;
    s32 pitch;
    const u8* data;

    bitmap( const font::metric&, s32, const u8* );
};

//--- face -----------------------------------------------------------------------------------------
class OOE_VISIBLE face
    : private noncopyable
{
public:
    enum string_type
    {
        family,
        style
    };

    enum number_type
    {
        glyphs,
        strikes,
        first
    };

    face( const library&, const descriptor& );
    ~face( void );

    std::string string( string_type ) const;
    u32 number( number_type ) const;

    f32 kerning( u32, u32, u32 ) const;
    font::bitmap bitmap( u32, u32 ) const;

private:
    ooe::memory memory;
    FT_Face face_;
};

OOE_NAMESPACE_END( ( ooe )( font ) )

#endif  // OOE_FOUNDATION_VISUAL_FONT_HPP
