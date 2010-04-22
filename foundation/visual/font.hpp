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
	const s32 left;
	const s32 top;
	const u32 x;
	const u32 y;
	const u32 width;
	const u32 height;

	metric( s32, s32, u32, u32, u32, u32 );
};

//--- bitmap ---------------------------------------------------------------------------------------
struct bitmap
{
	font::metric metric;
	const u8* data;

	bitmap( const font::metric&, const u8* );
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
		strikes
	};

	face( const library&, const descriptor& );
	~face( void );

	std::string string( string_type ) const;
	u32 number( number_type ) const;
	bitmap character( up_t, u32 );

private:
	ooe::memory memory;
	FT_Face face_;
};

OOE_NAMESPACE_END( ( ooe )( font ) )

#endif	// OOE_FOUNDATION_VISUAL_FONT_HPP
