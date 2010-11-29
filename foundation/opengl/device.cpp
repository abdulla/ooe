/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <algorithm>
#include <iterator>
#include <set>

#include "foundation/opengl/block.hpp"
#include "foundation/opengl/buffer.hpp"
#include "foundation/opengl/context.hpp"
#include "foundation/opengl/frame.hpp"
#include "foundation/opengl/program.hpp"
#include "foundation/opengl/shader.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/opengl/target.hpp"
#include "foundation/opengl/texture.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( opengl ) )

typedef std::set< s32 > attribute_set;

u8 block_indices( ooe::block::type type )
{
    switch ( type )
    {
    case ooe::block::u8_1:
    case ooe::block::u8_2:
    case ooe::block::u8_3:
    case ooe::block::u8_4:
    case ooe::block::f32_1:
    case ooe::block::f32_2:
    case ooe::block::f32_3:
    case ooe::block::f32_4:
        return 1;

    case ooe::block::f32_mat3:
        return 3;

    case ooe::block::f32_mat4:
        return 4;

    default:
        throw error::runtime( "opengl::device: " ) << "Unknown block type: " << type;
    }
}

u8 block_stride( ooe::block::type type )
{
    switch ( type )
    {
    case ooe::block::u8_1:
        return 1 * sizeof( u8 );

    case ooe::block::u8_2:
        return 2 * sizeof( u8 );

    case ooe::block::u8_3:
        return 3 * sizeof( u8 );

    case ooe::block::u8_4:
        return 4 * sizeof( u8 );

    case ooe::block::f32_1:
        return 1 * sizeof( f32 );

    case ooe::block::f32_2:
        return 2 * sizeof( f32 );

    case ooe::block::f32_mat3:
    case ooe::block::f32_3:
        return 3 * sizeof( f32 );

    case ooe::block::f32_mat4:
    case ooe::block::f32_4:
        return 4 * sizeof( f32 );

    default:
        throw error::runtime( "opengl::device: " ) << "Unknown block type: " << type;
    }
}

u8 block_size( ooe::block::type type )
{
    switch ( type )
    {
    case ooe::block::u8_1:
        return 1;

    case ooe::block::u8_2:
        return 2;

    case ooe::block::u8_3:
        return 3;

    case ooe::block::u8_4:
        return 4;

    case ooe::block::f32_1:
        return 1;

    case ooe::block::f32_2:
        return 2;

    case ooe::block::f32_mat3:
    case ooe::block::f32_3:
        return 3;

    case ooe::block::f32_mat4:
    case ooe::block::f32_4:
        return 4;

    default:
        throw error::runtime( "opengl::device: " ) << "Unknown block type: " << type;
    }
}

u32 block_type( ooe::block::type type )
{
    switch ( type )
    {
    case ooe::block::u8_1:
    case ooe::block::u8_2:
    case ooe::block::u8_3:
    case ooe::block::u8_4:
        return UNSIGNED_BYTE;

    case ooe::block::f32_1:
    case ooe::block::f32_2:
    case ooe::block::f32_3:
    case ooe::block::f32_4:
    case ooe::block::f32_mat3:
    case ooe::block::f32_mat4:
        return FLOAT;

    default:
        throw error::runtime( "opengl::device: " ) << "Unknown block type: " << type;
    }
}

void set_attributes( attribute_set& set, const opengl::block::buffer_map::const_iterator begin,
    const opengl::block::buffer_map::const_iterator end )
{
    opengl::buffer& buffer = dynamic_cast< opengl::buffer& >( *begin->first );
    BindBuffer( buffer.target, buffer.id );
    s32 stride = 0;

    for ( opengl::block::buffer_map::const_iterator i = begin; i != end; ++i )
    {
        for ( u8 j = 0, indices = block_indices( i->second._1 ); j != indices; ++j )
            stride += block_stride( i->second._1 );
    }

    u8* offset = 0;

    for ( opengl::block::buffer_map::const_iterator i = begin; i != end; ++i )
    {
        set.insert( i->second._0 );

        for ( u8 j = 0, indices = block_indices( i->second._1 ); j != indices; ++j )
        {
            s32 size = block_size( i->second._1 );
            u32 type = block_type( i->second._1 );
            VertexAttribPointer( i->second._0 + j, size, type, false, stride, offset );
            offset += block_stride( i->second._1 );
        }
    }
}

void enable_attributes( attribute_set& x, attribute_set& y )
{
    typedef std::vector< s32 > attribute_vector;
    typedef std::insert_iterator< attribute_vector > inserter;
    attribute_vector v;
    std::set_difference( x.begin(), x.end(), y.begin(), y.end(), inserter( v, v.begin() ) );

    for ( attribute_vector::const_iterator i = v.begin(), end = v.end(); i != end; ++i )
        DisableVertexAttribArray( *i );

    v.clear();
    std::set_difference( y.begin(), y.end(), x.begin(), x.end(), inserter( v, v.begin() ) );

    for ( attribute_vector::const_iterator i = v.begin(), end = v.end(); i != end; ++i )
        EnableVertexAttribArray( *i );

    x.swap( y );
}

void enable_frame( const frame_ptr& generic_frame, u32 program, s32 draw_buffers_limit )
{
    if ( dynamic_cast< opengl::default_frame* >( generic_frame.get() ) )
    {
        BindFramebuffer( DRAW_FRAMEBUFFER, 0 );
        return;
    }

    opengl::frame& frame = dynamic_cast< opengl::frame& >( *generic_frame );
    s32 size = frame.colours.size();

    if ( frame.program != program )
        throw error::runtime( "opengl::device: " ) <<
            "Frame program " << frame.program << " != block program " << program;
    else if ( size > draw_buffers_limit )
        throw error::runtime( "opengl::device: " ) <<
            "Frame has " << size << " colour attachments, device supports " << draw_buffers_limit;

    frame.check();
    DrawBuffers( size, &frame.colours[ 0 ] );
}

void verify_texture( u32 width, u32 height, u32 limit )
{
    if ( width > limit || height > limit )
        throw error::runtime( "opengl::device: " ) <<
            "Image size " << width << 'x' << height << " > device texture size " << limit;
}

OOE_ANONYMOUS_END( ( ooe )( opengl ) )

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- device ---------------------------------------------------------------------------------------
class device
    : public ooe::device
{
public:
    device( const ooe::view_data&, bool );
    virtual ~device( void );

    virtual void draw( const block_ptr&, const frame_ptr&, u32 );
    virtual void swap( void );

    virtual void set( set_type, bool );
    virtual u32 limit( limit_type ) const;

    virtual texture_ptr texture( const image_pyramid&, texture::type, bool ) const;
    virtual texture_array_ptr texture_array( const image_metadata&, u32 ) const;
    virtual buffer_ptr buffer( up_t, buffer::type, buffer::usage_type ) const;
    virtual target_ptr target( const image_metadata& ) const;
    virtual shader_ptr shader( const std::string&, shader::type ) const;
    virtual program_ptr program( const shader_vector& ) const;
    virtual frame_ptr default_frame( u32, u32 ) const;

private:
    const view_data& view;
    platform::context_type context;
    attribute_set attributes;

    s32 draw_buffers_limit;
    s32 texture_size_limit;
    s32 texture_units_limit;
    s32 array_size_limit;
};

device::device( const ooe::view_data& view_, bool sync )
try
    : view( view_ ), context( context_construct( view ) ), attributes(), draw_buffers_limit(),
    texture_size_limit(), texture_units_limit(), array_size_limit()
{
    context_current( view, context );
    context_sync( view, context, sync );
    load_symbols();

    GetIntegerv( MAX_DRAW_BUFFERS, &draw_buffers_limit );
    GetIntegerv( MAX_TEXTURE_SIZE, &texture_size_limit );
    GetIntegerv( MAX_TEXTURE_IMAGE_UNITS, &texture_units_limit );
    GetIntegerv( MAX_ARRAY_TEXTURE_LAYERS, &array_size_limit );

    BlendFunc( SRC_ALPHA, ONE_MINUS_SRC_ALPHA );
    DepthFunc( LEQUAL );
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

void device::draw( const block_ptr& generic_block, const frame_ptr& frame, u32 instances )
{
    if ( !instances )
        throw error::runtime( "opengl::device: " ) << "Number of instances must be > 0";

    opengl::block& block = dynamic_cast< opengl::block& >( *generic_block );
    UseProgram( block.id );

    for ( block::uniform_map::const_iterator i = block.uniforms.begin(),
        end = block.uniforms.end(); i != end; ++i )
        i->second._0( i->first, i->second._1, i->second._2 );

    s32 j = 0;
    s32 size = block.textures.size() + block.texture_arrays.size();

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

    for ( block::texture_array_map::const_iterator i = block.texture_arrays.begin(),
        end = block.texture_arrays.end(); i != end; ++i, ++j )
    {
        Uniform1iv( i->first, 1, &j );
        ActiveTexture( TEXTURE0 + j );
        BindTexture( TEXTURE_2D_ARRAY, dynamic_cast< opengl::texture_array& >( *i->second ).id );
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
    enable_frame( frame, block.id, draw_buffers_limit );

    opengl::buffer& index = dynamic_cast< opengl::buffer& >( *block.index );
    BindBuffer( index.target, index.id );
    DrawElementsInstanced( TRIANGLES, index.size / sizeof( u16 ), UNSIGNED_SHORT, 0, instances );
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

    case array_size:
        return array_size_limit;

    default:
        throw error::runtime( "opengl::device: " ) << "Unknown limit type: " << type;
    }
}

texture_ptr device::
    texture( const image_pyramid& pyramid, texture::type filter, bool generate_mipmap ) const
{
    verify_texture( pyramid.width, pyramid.height, texture_size_limit );

    if ( is_compressed( pyramid.format ) )
        return new compressed_texture( pyramid, filter, generate_mipmap );
    else
        return new uncompressed_texture( pyramid, filter, generate_mipmap );
}

texture_array_ptr device::texture_array( const image_metadata& metadata, u32 depth ) const
{
    verify_texture( metadata.width, metadata.height, texture_size_limit );

    if ( depth > u32( array_size_limit ) )
        throw error::runtime( "opengl::device: " ) <<
            "Array size " << depth << " > array size limit " << array_size_limit;

    if ( is_compressed( metadata.format ) )
        return new compressed_texture_array( metadata, depth );
    else
        return new uncompressed_texture_array( metadata, depth );
}

buffer_ptr device::buffer( up_t size, buffer::type format, buffer::usage_type usage ) const
{
    return new opengl::buffer( size, format, usage );
}

target_ptr device::target( const image_metadata& metadata ) const
{
    return new opengl::target( metadata );
}

shader_ptr device::shader( const std::string& source, shader::type type ) const
{
    return new opengl::shader( source, type );
}

program_ptr device::program( const shader_vector& vector ) const
{
    return new opengl::program( vector );
}

frame_ptr device::default_frame( u32 width, u32 height ) const
{
    return new opengl::default_frame( width, height );
}

OOE_NAMESPACE_END( ( ooe )( opengl ) )

extern "C" device_ptr OOE_VISIBLE device_open( const view_data& view, bool sync )
{
    return new opengl::device( view, sync );
}
