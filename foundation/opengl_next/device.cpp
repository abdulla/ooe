/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <algorithm>
#include <iterator>
#include <set>

#include "foundation/opengl_next/block.hpp"
#include "foundation/opengl_next/buffer.hpp"
#include "foundation/opengl_next/context.hpp"
#include "foundation/opengl_next/frame.hpp"
#include "foundation/opengl_next/program.hpp"
#include "foundation/opengl_next/shader.hpp"
#include "foundation/opengl_next/symbol.hpp"
#include "foundation/opengl_next/target.hpp"
#include "foundation/opengl_next/texture.hpp"
#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

typedef std::set< s32 > attribute_set;

void set_attributes( attribute_set& set, const opengl::block::buffer_map::const_iterator begin,
	const opengl::block::buffer_map::const_iterator end )
{
	opengl::buffer& buffer = dynamic_cast< opengl::buffer& >( *begin->first );
	BindBuffer( buffer.target, buffer.id );
	s32 size = 0;

	for ( opengl::block::buffer_map::const_iterator i = begin; i != end; ++i )
		size += i->second._1;

	size *= sizeof( f32 );
	u8* offset = 0;

	for ( opengl::block::buffer_map::const_iterator i = begin; i != end; ++i )
	{
		set.insert( i->second._0 );
		VertexAttribPointer( i->second._0, i->second._1, FLOAT, false, size, offset );
		offset += i->second._1 * sizeof( f32 );
	}
}

void enable_attributes( attribute_set& x, attribute_set& y )
{
	typedef std::vector< s32 > attributes_vector;
	typedef std::insert_iterator< attributes_vector > inserter;
	attributes_vector v;
	std::set_difference( x.begin(), x.end(), y.begin(), y.end(), inserter( v, v.begin() ) );

	for ( attributes_vector::const_iterator i = v.begin(), end = v.end(); i != end; ++i )
		DisableVertexAttribArray( *i );

	v.clear();
	std::set_difference( y.begin(), y.end(), x.begin(), x.end(), inserter( v, v.begin() ) );

	for ( attributes_vector::const_iterator i = v.begin(), end = v.end(); i != end; ++i )
		EnableVertexAttribArray( *i );

	x.swap( y );
}

void enable_frame( const frame_type& generic_frame, s32 draw_buffers_limit )
{
	if ( dynamic_cast< opengl::default_frame* >( generic_frame.get() ) )
	{
		BindFramebuffer( DRAW_FRAMEBUFFER, 0 );
		return;
	}

	opengl::frame& frame = dynamic_cast< opengl::frame& >( *generic_frame );
	s32 size = frame.colours.size();

	if ( size > draw_buffers_limit )
		throw error::runtime( "opengl::device: " ) <<
			"Frame has " << size << " colour attachments, device supports " << draw_buffers_limit;

	BindFramebuffer( DRAW_FRAMEBUFFER, frame.id );
	DrawBuffers( size, &frame.colours[ 0 ] );

	if ( !frame.check )
		return;

	s32 status = CheckFramebufferStatus( DRAW_FRAMEBUFFER );

	if ( status != FRAMEBUFFER_COMPLETE )
		throw error::runtime( "opengl::device: " ) << "Frame is incomplete: 0x" << hex( status );

	frame.check = false;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- device ---------------------------------------------------------------------------------------
class device
	: public ooe::device
{
public:
	device( const ooe::view_data& );
	virtual ~device( void );

	virtual void draw( const block_type&, const frame_type& );
	virtual void swap( void );

	virtual void set( set_type, bool );
	virtual u32 limit( limit_type ) const;

	virtual texture_type texture( const image_pyramid&, texture::type, bool ) const;
	virtual buffer_type buffer( up_t, buffer::type, buffer::usage_type ) const;
	virtual target_type target( u32, u32, image::type ) const;
	virtual shader_type shader( const std::string&, const descriptor&, shader::type ) const;
	virtual program_type program( const shader_vector& ) const;
	virtual frame_type default_frame( u32, u32 ) const;

private:
	const view_data& view;
	platform::context_type context;
	attribute_set attributes;

	s32 draw_buffers_limit;
	s32 texture_size_limit;
	s32 texture_units_limit;
};

device::device( const ooe::view_data& view_ )
try
	: view( view_ ), context( context_construct( view ) ), attributes(), draw_buffers_limit(),
	texture_size_limit(), texture_units_limit()
{
	context_current( view, context );
	context_sync( view, context, true );
	load_symbols();

	GetIntegerv( MAX_DRAW_BUFFERS, &draw_buffers_limit );
	GetIntegerv( MAX_TEXTURE_SIZE, &texture_size_limit );
	GetIntegerv( MAX_TEXTURE_IMAGE_UNITS, &texture_units_limit );

	BlendFunc( SRC_ALPHA, ONE_MINUS_SRC_ALPHA );
	PixelStorei( PACK_ALIGNMENT, 1 );
	PixelStorei( UNPACK_ALIGNMENT, 1 );
}
catch ( ... )
{
	this->~device();
}

device::~device( void )
{
	context_current( view, 0 );
	context_destruct( view, context );
}

void device::draw( const block_type& generic_block, const frame_type& frame )
{
	opengl::block& block = dynamic_cast< opengl::block& >( *generic_block );
	UseProgram( block.id );

	for ( block::uniform_map::const_iterator i = block.uniforms.begin(),
		end = block.uniforms.end(); i != end; ++i )
		i->second._1( i->first, i->second._0 );

	s32 j = 0;
	s32 size = block.textures.size();

	if ( size > texture_units_limit )
		throw error::runtime( "opengl::device: " ) <<
			"Block has " << size << " textures, device supports " << texture_units_limit;

	for ( block::texture_map::const_iterator i = block.textures.begin(),
		end = block.textures.end(); i != end; ++i, ++j )
	{
		Uniform1iv( i->first, 1, &j );
		ActiveTexture( TEXTURE0 + j );
		BindTexture( TEXTURE_2D, dynamic_cast< opengl::texture& >( *i->second ).id );
	}

	block::buffer_map& map = block.buffers;

	if ( map.empty() )
		throw error::runtime( "opengl::device: " ) << "Block does not contain any point buffers";

	typedef block::buffer_map::const_iterator buffer_iterator;
	typedef std::pair< buffer_iterator, buffer_iterator > buffer_pair;
	buffer_iterator end = map.end();
	attribute_set enable_set;

	for ( buffer_pair pair = map.equal_range( map.begin()->first ); ;
		pair = map.equal_range( pair.second->first ) )
	{
		set_attributes( enable_set, pair.first, pair.second );

		if ( pair.second == end )
			break;
	}

	enable_attributes( attributes, enable_set );
	enable_frame( frame, draw_buffers_limit );

	opengl::buffer& index = dynamic_cast< opengl::buffer& >( *block.index );
	BindBuffer( index.target, index.id );
	DrawElements( TRIANGLES, index.size / sizeof( u16 ), UNSIGNED_SHORT, 0 );
}

void device::swap( void )
{
	context_swap( view, context );
}

void device::set( set_type type, bool enable )
{
	u32 value;

	switch ( type )
	{
	case depth_test:
		value = DEPTH_TEST;
		break;

	case blend:
		value = BLEND;
		break;

	default:
		throw error::runtime( "opengl::device: " ) << "Unknown set type: " << type;
	}

	if ( enable )
		Enable( value );
	else
		Disable( value );
}

u32 device::limit( limit_type type ) const
{
	switch ( type )
	{
	case draw_buffers:
		return draw_buffers_limit;

	case texture_size:
		return texture_size_limit;

	case texture_units:
		return texture_units_limit;

	default:
		throw error::runtime( "opengl::device: " ) << "Unknown limit type: " << type;
	}
}

texture_type device::
	texture( const image_pyramid& pyramid, texture::type filter, bool generate_mipmap ) const
{
	if ( pyramid.width > u32( texture_size_limit ) || pyramid.height > u32( texture_size_limit ) )
		throw error::runtime( "opengl::device: " ) << "Pyramid size " << pyramid.width << 'x' <<
			pyramid.height << " > device texture size " << texture_size_limit;

	if ( is_compressed( pyramid.format ) )
		return new compressed_texture( pyramid, filter, generate_mipmap );
	else
		return new uncompressed_texture( pyramid, filter, generate_mipmap );
}

buffer_type device::buffer( up_t size, buffer::type format, buffer::usage_type usage ) const
{
	return new opengl::buffer( size, format, usage );
}

target_type device::target( u32 width, u32 height, image::type format ) const
{
	return new opengl::target( width, height, format );
}

shader_type device::
	shader( const std::string& source, const descriptor& desc, shader::type type ) const
{
	return new opengl::shader( source, desc, type );
}

program_type device::program( const shader_vector& vector ) const
{
	return new opengl::program( vector );
}

frame_type device::default_frame( u32 width, u32 height ) const
{
	return new opengl::default_frame( width, height );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )

extern "C" device_type OOE_VISIBLE device_open( const view_data& view )
{
	return new opengl::device( view );
}
