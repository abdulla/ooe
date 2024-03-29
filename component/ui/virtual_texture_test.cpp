/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include "component/ui/make.hpp"
#include "component/ui/text_layout.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/math/math.hpp"
#include "foundation/parallel/thread_pool.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/view.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

const f32 width = 200;
const f32 height = 800;

typedef unit::group< anonymous_t, anonymous_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "virtual_texture" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

OOE_TEST( 0 )
{
    std::cerr << "virtual_texture and font_source";

    std::string root = executable::path()._0;
    library opengl_library( root + "../lib/libopengl" OOE_EXTENSION, library::global_lazy );

    event_queue queue;
    view view( queue, width, height, false );
    device_ptr device = opengl_library.find< device_open_type >( "device_open" )( view, true );

    font::library font_library;
    font::face font_face( font_library, root + "../share/font/roboto-regular.ttf" );
    font_source font_source( font_face, 512, root + "../cache" );

    thread_pool pool( "pool" );
    page_cache cache( device, pool, font_source.page_size(), font_source.format() );
    text_layout layout( device, cache, font_source );

    program_ptr program =
        make_program( device, root + "../share/glsl", root + "../share/json/text.effect" );
    frame_ptr frame = device->default_frame( width, height );
    const c8 data[] = "AVA V AVA AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz 0123456789";

    text_vector text( 5 );
    text[ 0 ].data = "M";
    text[ 0 ].colour = colour( 255, 255, 0, 255 );
    text[ 0 ].level = 8;
    text[ 1 ].data = "MM";
    text[ 1 ].colour = colour( 127, 127, 255, 255 );
    text[ 1 ].level = 7;
    text[ 2 ].data = "test";
    text[ 2 ].colour = colour( 255, 0, 0, 255 );
    text[ 2 ].level = 4;
    text[ 3 ].data = "test";
    text[ 3 ].colour = colour( 0, 255, 0, 255 );
    text[ 3 ].level = 5;
    text[ 4 ].colour = colour( 255, 255, 255, 255 );
    text[ 4 ].level = 4;

    block_ptr block = program->block( make_index( device ) );
    block->input( "vertex", block::f32_2, false, make_point( device ) );
    block->input( "projection", orthographic( 0, width, height, 0 ) );
    device->set( device::blend, true );

    for ( const c8* i = data, * end = i + sizeof( data ); i != end; ++i )
    {
        text[ 4 ].data = std::string( data, i + 1 );
        u32 instances = layout.input( block, text, width, 0 );

        while ( cache.pending() )
            cache.write();

        frame->clear();
        device->draw( block, instances, frame );
        device->swap();

        event event;
        while ( queue.dequeue( event, epoch_t( 0, 0 ) ) ) {}
    }
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
