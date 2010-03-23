/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl_next/block.hpp"
#include "foundation/opengl_next/buffer.hpp"
#include "foundation/opengl_next/context.hpp"
#include "foundation/opengl_next/frame.hpp"
#include "foundation/opengl_next/program.hpp"
#include "foundation/opengl_next/shader.hpp"
#include "foundation/opengl_next/symbol.hpp"
#include "foundation/opengl_next/target.hpp"
#include "foundation/opengl_next/texture.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe )( opengl ) )

void set_attributes( opengl::block::in_buffer_map::const_iterator begin,
	opengl::block::in_buffer_map::const_iterator end )
{
	opengl::buffer& buffer = dynamic_cast< opengl::buffer& >( *begin->first );
	BindBuffer( buffer.target, buffer.id );
	u8 size = 0;

	for ( opengl::block::in_buffer_map::const_iterator i = begin; i != end; ++i )
		size += i->second._1;

	u8 offset = 0;

	for ( opengl::block::in_buffer_map::const_iterator i = begin; i != end; ++i )
	{
		offset += i->second._1;
		VertexAttribPointer( i->second._0, i->second._1, FLOAT, false, size, ( void* )offset );
	}
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

	virtual texture_type texture( const image_pyramid&, texture::type, bool ) const;
	virtual buffer_type buffer( up_t, buffer::type, buffer::usage_type ) const;
	virtual target_type target( u32, u32, u8 ) const;
	virtual shader_type shader( const std::string&, shader::type ) const;
	virtual program_type program( const shader_vector& ) const;
	virtual frame_type frame( void ) const;

private:
	const view_data& view;
	platform::context_type context;
};

device::device( const ooe::view_data& view_ )
try
	: view( view_ ), context( context_construct( view ) )
{
	context_current( view, context );
	context_sync( view, context, true );
	load_symbols();

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

void device::draw( const block_type& generic_block, const frame_type& )
{
	opengl::block& block = dynamic_cast< opengl::block& >( *generic_block );
	UseProgram( block.id );
	u8 j = 0;

	for ( block::in_texture_vector::const_iterator i = block.in_textures.begin(),
		end = block.in_textures.end(); i != end; ++i, ++j )
	{
		Uniform1i( i->_0, j );
		ActiveTexture( TEXTURE0 + j );
		BindTexture( TEXTURE_2D, dynamic_cast< opengl::texture& >( *i->_1 ).id );
	}

	block::in_buffer_map& map = block.in_buffers;

	if ( map.empty() )
		throw error::runtime( "opengl::device: " ) << "Block does not contain any buffers";

	typedef block::in_buffer_map::const_iterator buffer_iterator;
	typedef std::pair< buffer_iterator, buffer_iterator > buffer_pair;
	buffer_iterator end = map.end();

	for ( buffer_pair pair = map.equal_range( map.begin()->first ); pair.first != end;
		pair = map.equal_range( pair.second->first ) )
		set_attributes( pair.first, pair.second );

	opengl::buffer& index = dynamic_cast< opengl::buffer& >( *block.index );
	BindBuffer( index.target, index.id );
	DrawElements( TRIANGLES, index.size / sizeof( u16 ), UNSIGNED_SHORT, 0 );
}

void device::swap( void )
{
	context_swap( view, context );
	Clear( DEPTH_BUFFER_BIT | COLOR_BUFFER_BIT );
}

texture_type device::
	texture( const image_pyramid& pyramid, texture::type filter, bool generate_mipmap ) const
{
	if ( is_compressed( pyramid.format ) )
		return new compressed_texture( pyramid, filter, generate_mipmap );
	else
		return new uncompressed_texture( pyramid, filter, generate_mipmap );
}

buffer_type device::buffer( up_t size, buffer::type format, buffer::usage_type usage ) const
{
	return new opengl::buffer( size, format, usage );
}

target_type device::target( u32 width, u32 height, u8 format ) const
{
	return new opengl::target( width, height, format );
}

shader_type device::shader( const std::string& source, shader::type type ) const
{
	return new opengl::shader( source, type );
}

program_type device::program( const shader_vector& vector ) const
{
	return new opengl::program( vector );
}

frame_type device::frame( void ) const
{
	return new opengl::frame;
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )

extern "C" device_type device_open( const view_data& view )
{
	return new opengl::device( view );
}
