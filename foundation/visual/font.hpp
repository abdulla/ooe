/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VISUAL_FONT_HPP
#define OOE_FOUNDATION_VISUAL_FONT_HPP

#include "foundation/image/image.hpp"
#include "foundation/io/memory.hpp"

struct FT_FaceRec_;
typedef FT_FaceRec_* FT_Face;
struct FT_LibraryRec_;
typedef FT_LibraryRec_* FT_Library;

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

//--- bitmap ---------------------------------------------------------------------------------------
struct OOE_VISIBLE bitmap
{
	const s32 left;
	const s32 top;
	const u32 x;
	const u32 y;
	const ooe::uncompressed_image image;

	bitmap( s32, s32, u32, u32, const uncompressed_image& );
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

	face( const library&, const descriptor&, u32 );
	~face( void );

	bitmap character( up_t );
	void size( u32 );

	std::string string( string_type );
	u32 number( number_type );

private:
	ooe::memory memory;
	FT_Face face_;
};

OOE_NAMESPACE_END( ( ooe )( font ) )

#endif	// OOE_FOUNDATION_VISUAL_FONT_HPP
