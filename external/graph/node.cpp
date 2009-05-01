/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/general/sight.hpp"
#include "external/graph/node.hpp"

namespace ooe
{
//--- scene::data --------------------------------------------------------------
	scene::data::data( bool blend_, const video_data& vdata_ )
		: node( blend_ ), vdata( vdata_ )
	{
	}

	scene::data* scene::data::copy( void ) const
	{
		return new data( *this );
	}

	void scene::data::draw( video& video ) const
	{
		video.draw( vdata );
	}

//--- scene::text --------------------------------------------------------------
	scene::text::text( const texture_type& texture_, const video_data& data_ )
		: node( true ), texture( texture_ ), data( data_ )
	{
	}

	scene::text* scene::text::copy( void ) const
	{
		return new text( *this );
	}

	void scene::text::draw( video& video ) const
	{
		texture->bind();
		video.draw( data );
	}

//--- scene::mesh --------------------------------------------------------------
	scene::mesh::mesh( ooe::texture& texture_, const video_data& data_ )
		: node(), texture( texture_ ), data( data_ )
	{
	}

	scene::mesh* scene::mesh::copy( void ) const
	{
		return new mesh( *this );
	}

	void scene::mesh::draw( video& video ) const
	{
		texture.bind();
		video.draw( data );
	}

//--- scene::sight -------------------------------------------------------------
	scene::sight::sight( const video& video, const video_data& data_, u16 width_, u16 height_ )
		: node(), buffer( video.buffer( buffer::pixel_unpack ) ), texture( video.texture() ),
		data( data_ ), width( width_ ), height( height_ )
	{
		buffer->bind();
		buffer->load( 0, width * height * 3, buffer::stream_draw );
		buffer->unbind();

		texture->bind();
		texture->load( 0, width, height, ooe::sight::format() );
	}

	scene::sight* scene::sight::copy( void ) const
	{
		return new sight( *this );
	}

	void scene::sight::draw( video& video ) const
	{
		texture->bind();
		video.draw( data );
	}

	void scene::sight::update( const u8* in )
	{
		buffer->bind();
		buffer->load( in, width * height * 3, buffer::reload );

		texture->bind();
		texture->load( 0, width, height, ooe::sight::format(), texture::reload );

		buffer->unbind();
	}
}
