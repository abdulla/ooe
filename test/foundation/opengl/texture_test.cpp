/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/image/dds.hpp"
#include "foundation/math/math.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/graphics.hpp"
#include "foundation/visual/view.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

const f32 width = 640;
const f32 height = 480;
const u32 size = 256;
const image_format::type format = image_format::rgba_dxt1;

const c8 vertex_shader[] =
    "uniform mat4 projection;\n\
    attribute vec2 vertex;\n\
    varying vec2 texcoord;\n\
    \n\
    void main( void )\n\
    {\n\
        gl_Position = projection * vec4( vertex, 0, 1 );\n\
        texcoord = vertex;\n\
    }";

const c8 fragment_shader[] =
    "#extension GL_EXT_gpu_shader4 : enable\n\
    \n\
    uniform sampler2DArray sampler;\n\
    uniform int index;\n\
    varying vec2 texcoord;\n\
    \n\
    void main( void )\n\
    {\n\
        gl_FragColor = texture2DArray( sampler, vec3( texcoord, index ) );\n\
    }";

buffer_ptr make_point( const device_ptr& device )
{
    buffer_ptr point = device->buffer( sizeof( f32 ) * 2 * 4, buffer::point );
    {
        map_ptr map = point->map( buffer::write );
        f32* value = static_cast< f32* >( map->data );

        // top left
        value[ 0 ] = 0;
        value[ 1 ] = 1;

        // bottom left
        value[ 2 ] = 0;
        value[ 3 ] = 0;

        // top right
        value[ 4 ] = 1;
        value[ 5 ] = 1;

        // bottom right
        value[ 6 ] = 1;
        value[ 7 ] = 0;
    }
    return point;
}

buffer_ptr make_index( const device_ptr& device )
{
    buffer_ptr index = device->buffer( sizeof( u16 ) * 6, buffer::index );
    {
        map_ptr map = index->map( buffer::write );
        u16* value = static_cast< u16* >( map->data );

        value[ 0 ] = 0;
        value[ 1 ] = 1;
        value[ 2 ] = 2;
        value[ 3 ] = 2;
        value[ 4 ] = 1;
        value[ 5 ] = 3;
    }
    return index;
}

image make_image( u8 red, u8 green, u8 blue )
{
    image image( size, size, image_format::rgba_u8 );
    u8 pixel_size = ooe::pixel_size( image );

    for ( u8* i = image.as< u8 >(), * end = i + byte_size( image ); i != end; i += pixel_size )
    {
        i[ 0 ] = red;
        i[ 1 ] = green;
        i[ 2 ] = blue;
        i[ 3 ] = 255;
    }

    return dxt::encode( image, format );
}

typedef unit::group< anonymous_t, anonymous_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "texture" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
    void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "test texture array\n";

    std::string root = executable::path()._0;
    library library( root + "../lib/libopengl.so", library::global_lazy );

    event_queue queue;
    view view( queue, width, height, false );
    device_ptr device = library.find< device_open_type >( "device_open" )( view, false );

    const s32 array_size = device->limit( device::array_size );
    texture_array_ptr texture_array =
        device->texture_array( image_metadata( size, size, format ), array_size );
    OOE_CHECK( "texture array size " << array_size << " > 0 ", array_size );

    shader_vector vector;
    vector.push_back( device->shader( vertex_shader, shader::vertex ) );
    vector.push_back( device->shader( fragment_shader, shader::fragment ) );
    program_ptr program = device->program( vector );

    block_ptr block = program->block( make_index( device ) );
    block->input( "vertex", block::f32_2, false, make_point( device ) );
    block->input( "sampler", texture_array );
    block->input( "projection", orthographic( 0, width / height, 1, 0 ) );

    image input[] =
    {
        make_image( 255, 0, 0 ),
        make_image( 0, 255, 0 ),
        make_image( 0, 0, 255 )
    };

    frame_ptr frame = device->default_frame( width, height );

    for ( s32 i = 0; i != array_size; ++i )
    {
        texture_array->write( input[ i % 3 ], 0, 0, i );
        block->input( "index", i );

        frame->clear();
        device->draw( block, frame, 1 );
        device->swap();

        event event;
        while ( queue.dequeue( event, epoch_t( 0, 0 ) ) ) {}
    }
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
