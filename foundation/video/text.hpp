/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VIDEO_TEXT_HPP
#define OOE_FOUNDATION_VIDEO_TEXT_HPP

#include "foundation/utility/pointer.hpp"
#include "foundation/video/font.hpp"
#include "foundation/video/video.hpp"

namespace ooe
{
//--- text ---------------------------------------------------------------------
	class OOE_VISIBLE text
		: private noncopyable
	{
	public:
		typedef tuple< texture_ptr, video_data > build_type;
		typedef tuple< s32 /*x-min*/, s32 /*x-max*/, s32 /*y-min*/, s32 /*y-max*/> bound_type;
		typedef std::string::const_iterator iterator;

		text( const ooe::video&, font::face&, u8 );

		bound_type bound( const iterator&, const iterator& ) const;
		build_type build( const std::string&, u32 = -1u ) const;

	private:
		const ooe::video& video;
		texture_ptr texture;
		scoped_array< font::metric > metric;

		const u32 size;
		const u32 width;
		const u32 height;
	};
}

#endif	// OOE_FOUNDATION_VIDEO_TEXT_HPP
