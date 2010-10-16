/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/ui/text_layout.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/io/vfs.hpp"
#include "foundation/math/math.hpp"
#include "foundation/parallel/thread_pool.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/view.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const f32 width = 640;
const f32 height = 480;

shader_vector make_shaders( const device_type& device, const std::string& root )
{
    vfs vfs;
    vfs.insert( root + "../share/glsl", "/" );
    shader_include include( device, vfs );
    include.insert( "virtual_texture.hs" );

    shader_vector vector;
    vector.push_back( include.compile( "null.vs", shader::vertex ) );
    vector.push_back( include.compile( "font.fs", shader::fragment ) );
    vector.push_back( include.compile( "virtual_texture.fs", shader::fragment ) );
    return vector;
}

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
    device_type device = opengl_library.
        find< device_type ( const view_data&, bool ) >( "device_open" )( view, true );

    font::library font_library;
    font::face font_face( font_library, root + "../share/font/ubuntu-regular.ttf" );
    font_source font_source( font_face, 512, root + "../cache" );

    thread_pool pool;
    page_cache cache( device, pool, font_source.format(), font_source.page_size() );
    virtual_texture vt( device, cache, font_source );
    text_layout layout( device, vt, font_source );

    device->set( device::blend, true );
    program_type program = device->program( make_shaders( device, root ) );
    frame_type frame = device->default_frame( width, height );
    std::string string = "AaBbCcDdEeFfGgHhIiJjKkLlMmNnOoPpQqRrSsTtUuVvWwXxYyZz0123456789";

    for ( std::string::iterator i = string.begin(), end = string.end(); i != end; ++i )
    {
        block_type block = layout.block( program, std::string( string.begin(), i ), 4 );
        block->input( "projection", orthographic( 0, width, height, 0 ) );
        block->input( "model_view", mat4::identity );
        block->input( "colour", 255, 255, 255 );
        vt.input( "vt", block );

        while ( cache.pending() )
            cache.write();

        frame->clear();
        device->draw( block, frame );
        device->swap();

        event event;
        while ( event_queue.next_event( event, epoch_t( 0, 0 ) ) ) {}
    }
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
