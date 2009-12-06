/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "external/scene/cache.hpp"
#include "foundation/image/dds.hpp"
#include "foundation/image/exr.hpp"
#include "foundation/image/jpeg.hpp"
#include "foundation/image/jpeg2000.hpp"
#include "foundation/image/png.hpp"
#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;

	std::string extension( const std::string& name )
	{
		up_t i = name.rfind( '.' );

		if ( i != std::string::npos )
			return name.substr( i + 1 );
		else
			throw error::runtime( "cache: " ) << "Image name has no extension";
	}
}

namespace ooe
{
//--- cache --------------------------------------------------------------------
	cache::cache( const ooe::vfs& vfs_, ooe::video& video_ )
		: vfs( vfs_ ), video( video_ ), textures(), programs()
	{
		insert( "dds", compressed_decode< dds::decode > );
		insert( "exr", uncompressed_decode< exr::decode > );
		insert( "jpg", uncompressed_decode< jpeg::decode > );
		insert( "jp2", uncompressed_decode< jpeg2000::decode > );
		insert( "png", uncompressed_decode< png::decode > );
	}

	texture& cache::texture( const std::string& name )
	{
		texture_map::const_iterator i = textures.find( name );

		if ( i != textures.end() )
			return *i->second;

		std::string image_type = extension( name );
		decoder_map::const_iterator j = decoders.find( image_type );

		if ( j == decoders.end() )
			throw error::runtime( "cache: " ) << "Unknown image type \"" << image_type << '\"';

		image image = j->second( vfs[ name ] );
		texture_ptr pointer = video.texture();
		pointer->bind();
		pointer->load( image.get(), image.width, image.height, image.format );

		textures.insert( texture_map::value_type( name, pointer ) );
		return *pointer;
	}

	program& cache::program( const std::string& name )
	{
		program_map::const_iterator i = programs.find( name );

		if ( i != programs.end() )
			return i->second.get();

		program_map::value_type value( name, effect( vfs[ name ], vfs, video ) );
		return programs.insert( value ).first->second.get();
	}

	void cache::insert( const std::string& name, image_decode_t decoder )
	{
		decoders.insert( decoder_map::value_type( name, decoder ) );
	}
}
