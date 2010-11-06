/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/ui/make.hpp"
#include "component/ui/text_layout.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/math/math.hpp"
#include "foundation/parallel/thread_pool.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/view.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const f32 width = 640;
const f32 height = 480;

typedef unit::group< anonymous_t, anonymous_t, 1 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "virtual_texture" );

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
    void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "test virtual texture and font source\n";

    std::string root = executable::path()._0;
    library opengl_library( root + "../lib/libopengl.so", library::global_lazy );

    event_queue event_queue;
    view view( event_queue, width, height, false );
    device_type device = opengl_library.find< device_open_type >( "device_open" )( view, true );

    font::library font_library;
    font::face font_face( font_library, root + "../share/font/ubuntu-regular.ttf" );
    font_source font_source( font_face, 512, root + "../cache" );

    thread_pool pool;
    page_cache cache( device, pool, font_source.format(), font_source.page_size() );
    virtual_texture vt( device, cache, font_source );
    text_layout layout( device, vt, font_source );

    program_type program =
        make_program( device, root + "../share/glsl", root + "../share/json/font.effect" );
    frame_type frame = device->default_frame( width, height );
    std::string string =
        "AVA V AVA AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz 0123456789";

    block_type block = program->block( make_index( device ) );
    block->input( "vertex", 2, make_point( device ) );
    block->input( "projection", orthographic( 0, width, height, 0 ) );
    block->input( "view", 0.f, 0.f, 0.f );
    block->input( "colour", 255, 255, 255 );
    vt.input( block, "vt" );
    device->set( device::blend, true );

    for ( std::string::iterator i = string.begin(), end = string.end(); i != end; ++i )
    {
        std::string substring( string.begin(), i + 1 );
        u32 instances = layout.input( block, substring, 4, width );

        while ( cache.pending() )
            cache.write();

        frame->clear();
        device->draw( block, frame, instances );
        device->swap();

        event event;
        while ( event_queue.next_event( event, epoch_t( 0, 0 ) ) ) {}
    }
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
