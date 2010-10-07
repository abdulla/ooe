/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/math/math.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/graphics.hpp"
#include "foundation/visual/view.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const f32 width = 640;
const f32 height = 480;
const u32 texture_size = 256;
const image::type texture_format = image::rgb_u8;

shader_type make_shader( const device_type& device, const std::string& path, shader::type type )
{
    return device->shader( memory_vector( 1, descriptor( path ) ), type );
}

buffer_type point_buffer( const device_type& device )
{
    buffer_type point = device->buffer( sizeof( f32 ) * 4 * 4, buffer::point );
    {
        map_type map = point->map( buffer::write );
        f32* value = static_cast< f32* >( map->data );

        // top left
        value[ 0 ] = -1;
        value[ 1 ] = 1;
        value[ 2 ] = 0;
        value[ 3 ] = 1;

        // bottom left
        value[ 4 ] = -1;
        value[ 5 ] = -1;
        value[ 6 ] = 0;
        value[ 7 ] = 0;

        // top right
        value[ 8 ] = 1;
        value[ 9 ] = 1;
        value[ 10 ] = 1;
        value[ 11 ] = 1;

        // bottom right
        value[ 12 ] = 1;
        value[ 13 ] = -1;
        value[ 14 ] = 1;
        value[ 15 ] = 0;
    }
    return point;
}

buffer_type index_buffer( const device_type& device )
{
    buffer_type index = device->buffer( sizeof( u16 ) * 6, buffer::index );
    {
        map_type map = index->map( buffer::write );
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
    uncompressed_image image( texture_size, texture_size, texture_format );
    u8 pixel_size = image.pixel_size();

    for ( u8* i = image.as< u8 >(), * end = i + image.byte_size(); i != end; i += pixel_size )
    {
        i[ 0 ] = red;
        i[ 1 ] = green;
        i[ 2 ] = blue;
    }

    return image;
}

typedef unit::group< anonymous_t, anonymous_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "texture" );

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
    void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "test texture array\n";

    std::string root = executable::path()._0;
    library library( root + "../library/libopengl" + library::suffix, library::global_lazy );

    event_queue event_queue;
    view view( event_queue, width, height, false );
    device_type device =
        library.find< device_type ( const view_data&, bool ) >( "device_open" )( view, false );

    shader_vector vector;
    vector.push_back( make_shader( device, root + "../resource/glsl/unit.vs", shader::vertex ) );
    vector.push_back( make_shader( device, root + "../resource/glsl/unit.fs", shader::fragment ) );
    program_type program = device->program( vector );

    const s32 array_size = device->limit( device::array_size );
    texture_array_type texture_array =
        device->texture_array( texture_size, texture_size, array_size, texture_format );

    buffer_type point = point_buffer( device );
    block_type block = program->block( index_buffer( device ) );
    block->input( "vertex", 2, point );
    block->input( "coords", 2, point );
    block->input( "sampler", texture_array );
    block->input( "projection", orthographic( 0, width / height, 1, 0 ) );

    image image_array[] =
    {
        make_image( 255, 0, 0 ),
        make_image( 0, 255, 0 ),
        make_image( 0, 0, 255 )
    };

    frame_type frame = device->default_frame( width, height );

    for ( s32 i = 0; i != array_size; ++i )
    {
        texture_array->write( image_array[ i % 3 ], 0, 0, i );
        block->input( "index", i );

        frame->clear();
        device->draw( block, frame );
        device->swap();

        event event;
        while ( event_queue.next_event( event, epoch_t( 0, 0 ) ) ) {}
    }
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
