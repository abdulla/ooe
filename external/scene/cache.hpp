/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_EXTERNAL_SCENE_CACHE_HPP
#define OOE_EXTERNAL_SCENE_CACHE_HPP

#include <map>

#include "external/scene/effect.hpp"
#include "foundation/image/image.hpp"

namespace ooe
{
	typedef image ( * image_decode_t )( const descriptor& );

//--- cache --------------------------------------------------------------------
	class OOE_VISIBLE cache
	{
	public:
		const ooe::vfs& vfs;
		ooe::video& video;

		cache( const ooe::vfs&, ooe::video& );

		ooe::texture& texture( const std::string& );
		ooe::program& program( const std::string& );

		void insert( const std::string&, image_decode_t );

	private:
		typedef std::map< const std::string, const texture_ptr > texture_map;
		typedef std::map< const std::string, const effect > program_map;
		typedef std::map< const std::string, const image_decode_t > decoder_map;

		texture_map textures;
		program_map programs;
		decoder_map decoders;
	};
}

#endif	// OOE_EXTERNAL_SCENE_CACHE_HPP
