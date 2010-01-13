/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VIDEO_FONT_HPP
#define OOE_FOUNDATION_VIDEO_FONT_HPP

#include "foundation/io/memory.hpp"

struct FT_FaceRec_;
typedef FT_FaceRec_* FT_Face;
struct FT_LibraryRec_;
typedef FT_LibraryRec_* FT_Library;

namespace ooe
{
	namespace font
	{
		class library;
		class face;
		struct metric;
		struct bitmap;
	}

	struct OOE_VISIBLE font::library
		: private noncopyable
	{
	public:
		library( void );
		~library( void );

	private:
		FT_Library freetype;

		friend class face;
	};

	class OOE_VISIBLE font::face
		: private noncopyable
	{
	public:
		enum
		{
			family,
			style
		};

		enum
		{
			glyphs,
			strikes
		};

		face( const library&, const descriptor& );
		~face( void );

		bool character( bitmap&, u32 );
		bool size( u32 );
		std::string name( u8 );
		u32 number( u8 );

	private:
		ooe::memory memory;
		FT_Face face_;
	};

	struct font::metric
	{
		u32 width;
		u32 height;
		s32 left;
		s32 top;
		u32 x;
		u32 y;
	};

	struct font::bitmap
		: public metric
	{
		u8* pointer;
	};
}

#endif	// OOE_FOUNDATION_VIDEO_FONT_HPP
