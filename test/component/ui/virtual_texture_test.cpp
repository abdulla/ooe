/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cmath>

#include "component/ui/virtual_texture.hpp"
#include "foundation/parallel/thread_pool.hpp"
#include "foundation/utility/error.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

//--- test_texture ---------------------------------------------------------------------------------
class test_texture
	: public texture
{
public:
	test_texture( u32 id_ )
		: id( id_ )
	{
	}

	virtual void write( const image& image, u32 x, u32 y, u8 level )
	{
		std::cout << "writing texture " << id << ": " << image.width << 'x' << image.height <<
			' ' << image.format << " at " << x << ", " << y << ", " << u32( level ) << std::endl;
	}

private:
	u32 id;
};

//--- test_texture_array ---------------------------------------------------------------------------
class test_texture_array
	: public texture_array
{
public:
	test_texture_array( u32 id_ )
		: id( id_ )
	{
	}

	virtual void write( const image& image, u32 x, u32 y, u32 index )
	{
		std::cout << "writing texture array " << id << ": " << image.width << 'x' << image.height <<
			' ' << image.format << " at " << x << ", " << y << ", " << index << std::endl;
	}

private:
	u32 id;
};

//--- test_device ----------------------------------------------------------------------------------
class test_device
	: public device
{
public:
	test_device( void )
		: texture_id( 0 ), texture_array_id( 0 )
	{
	}

	virtual void draw( const block_type&, const frame_type& )
	{
		throw error::runtime( "test_device: " ) << "Not implemented";
	}

	virtual void swap( void )
	{
		throw error::runtime( "test_device: " ) << "Not implemented";
	}

	virtual void set( set_type, bool )
	{
		throw error::runtime( "test_device: " ) << "Not implemented";
	}

	virtual u32 limit( limit_type flag ) const
	{
		switch ( flag )
		{
		case draw_buffers:
			return 1 << 4;

		case texture_size:
			return 1 << 13;

		case texture_units:
			return 1 << 4;

		case array_size:
			return 1 << 9;

		default:
			throw error::runtime( "test_device: " ) << "Unknown limit type: " << flag;
		}
	}

	virtual texture_type texture( const image_pyramid& pyramid, texture::type, bool ) const
	{
		std::cout << "created texture " << texture_id << ": " <<
			pyramid.width << 'x' << pyramid.height << ' ' << pyramid.format << std::endl;
		return new test_texture( texture_id++ );
	}

	virtual texture_array_type texture_array
		( u32 width, u32 height, u32 depth, image::type format ) const
	{
		std::cout << "created texture_array " << texture_array_id << ": " <<
			width << 'x' << height << 'x' << depth << ' ' << format << std::endl;
		return new test_texture_array( texture_array_id++ );
	}

	virtual buffer_type buffer( up_t, buffer::type, buffer::usage_type ) const
	{
		throw error::runtime( "test_device: " ) << "Not implemented";
	}

	virtual target_type target( u32, u32, image::type ) const
	{
		throw error::runtime( "test_device: " ) << "Not implemented";
	}

	virtual shader_type shader( const std::string&, const descriptor&, shader::type ) const
	{
		throw error::runtime( "test_device: " ) << "Not implemented";
	}

	virtual program_type program( const shader_vector& ) const
	{
		throw error::runtime( "test_device: " ) << "Not implemented";
	}

	virtual frame_type default_frame( u32, u32 ) const
	{
		throw error::runtime( "test_device: " ) << "Not implemented";
	}

private:
	mutable u32 texture_id;
	mutable u32 texture_array_id;
};

//--- test_source ----------------------------------------------------------------------------------
struct test_source
	: physical_source
{
	virtual u32 size( void ) const
	{
		return 1 << 16;
	}

	virtual image::type format( void ) const
	{
		return image::a_u8;
	}

	virtual u16 page_size( void ) const
	{
		return 1 << 8;
	}

	virtual image read( const pyramid_index& )
	{
		return uncompressed_image( page_size(), page_size(), format() );
	}
};

typedef unit::group< anonymous_t, anonymous_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "virtual_texture" );

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
	void fixture_type::test< 0 >( anonymous_t& )
{
	std::cerr << "test virtual texture\n";

	device_type device = new test_device;
	thread_pool pool;
	test_source source;

	page_cache cache( device, pool, source.format(), source.page_size() );
	virtual_texture vt( device, cache, source );
	u32 size = source.size();
	u16 page_size = source.page_size();

	for ( u8 i = 6, end = log2( size / page_size ) + 1; i != end; ++i )
	{
		vt.load( 0, 0, size, size, i );

		while ( cache.pending() )
			cache.write();
	}
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
