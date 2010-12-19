/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/ui/miscellany.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/math/math.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/graphics.hpp"
#include "foundation/visual/view.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

const f32 width = 640;
const f32 height = 480;
const u32 size = 256;

const c8 vertex_shader[] =
    "uniform mat4 projection;\n\
    uniform float depth;\n\
    attribute vec2 vertex;\n\
    \n\
    void main( void )\n\
    {\n\
        gl_Position = projection * vec4( vertex, depth, 1 );\n\
    }";

const c8 fragment_shader[] =
    "#extension GL_EXT_gpu_shader4 : enable\n\
    \n\
    uniform vec3 colour;\n\
    varying out vec4 texture;\n\
    \n\
    void main( void )\n\
    {\n\
        texture = vec4( colour, 1. );\n\
    }";

typedef unit::group< anonymous_t, anonymous_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "frame" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
    void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "test frame buffer\n";

    std::string root = executable::path()._0;
    library library( root + "../lib/libopengl.so", library::global_lazy );

    event_queue queue;
    view view( queue, width, height, false );
    device_ptr device = library.find< device_open_type >( "device_open" )( view, false );
    device->set( device::depth_test, true );

    shader_vector vector;
    vector.push_back( device->shader( vertex_shader, shader::vertex ) );
    vector.push_back( device->shader( fragment_shader, shader::fragment ) );
    program_ptr program = device->program( vector );

    block_ptr block = program->block( make_index( device ) );
    block->input( "vertex", block::f32_2, false, make_point( device ) );
    block->input( "projection", orthographic( 0, width / height, 1, 0 ) );

    image_metadata texture_metadata( width, height, image_format::rgb_u8 );
    image_metadata target_metadata( width, height, image_format::depth_u24 );
    texture_ptr texture = device->texture( texture_metadata, texture::nearest, false );
    target_ptr target = device->target( target_metadata );

    frame_ptr frame = program->frame( width, height );
    frame->output( "texture", texture );
    frame->output( target );

    frame_ptr default_frame = device->default_frame( width, height );
    default_frame->clear();

    for ( s32 i = 0; i != 256; ++i )
    {
        frame->clear();
        device->draw( block, frame, 1 );

        block->input( "depth", 1.f );
        block->input( "colour", 0.f, 1.f, 0.f );
        device->draw( block, frame, 1 );

        block->input( "depth", 0.f );
        block->input( "colour", 1.f, 0.f, 0.f );
        device->draw( block, frame, 1 );

        default_frame->write( frame );
        device->swap();

        event event;
        while ( queue.dequeue( event, epoch_t( 0, 0 ) ) ) {}
    }
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
