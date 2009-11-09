/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/utility/error.hpp"
#include "foundation/utility/pointer.hpp"

#include "external/graph/resource.hpp"

namespace
{
	using namespace ooe;

	std::string extension( const std::string& string )
	{
		up_t i = string.rfind( '.' );

		if ( i != std::string::npos )
			return string.substr( i + 1 );

		throw error::runtime( "resource< image_decode_t >: " ) << "Image name has no extension";
	}
}

namespace ooe
{
//--- resource< image_decode_t > -----------------------------
	void resource< image_decode_t >::insert( const std::string& name, image_decode_t decoder )
	{
		map.insert( map_type::value_type( name, decoder ) );
	}

	image_decode_t resource< image_decode_t >::operator []( const std::string& string ) const
	{
		map_type::const_iterator i = map.find( extension( string ) );

		if ( i != map.end() )
			return i->second;

		throw error::runtime( "resource< image_decode_t >: " ) << "Unknown image type";
	}

//--- cache_DEPRECATED< texture > ----------------------------------------------
	cache_DEPRECATED< texture >::cache_DEPRECATED( const ooe::vfs& vfs_, const ooe::video& video_,
		const ooe::resource< image_decode_t >& resource_ )
		: vfs( vfs_ ), video( video_ ), resource( resource_ ), map()
	{
	}

	texture& cache_DEPRECATED< texture >::operator []( const std::string& name )
	{
		map_type::const_iterator i = map.find( name );

		if ( i != map.end() )
			return *i->second;

		image image = resource[ name ]( vfs[ name ] );

		texture_ptr pointer = video.texture();
		pointer->bind();
		pointer->load( image.get(), image.width, image.height, image.format, texture::reverse );

		map.insert( map_type::value_type( name, pointer ) );
		return *pointer;
	}

	void cache_DEPRECATED< texture >::clear( void )
	{
		map.clear();
	}
}
