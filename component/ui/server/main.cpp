/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cstring>

#include "component/ui/make.hpp"
#include "component/ui/tile_source.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/image/png.hpp"
#include "foundation/math/math.hpp"
#include "foundation/parallel/thread_pool.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/graphics.hpp"
#include "foundation/visual/view.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef std::vector< shared_ptr< node > > node_vector;

const f32 width = 1024;
const f32 height = 640;
image::type format = image::rgba_u8;
u16 page_size = 256;
u32 velocity = 4;

//--- zoom_out -------------------------------------------------------------------------------------
void zoom_out( vec3& translate )
{
    if ( translate.z < 0 )
        return;

    translate.x -= u16( width ) >> u16( translate.z + 2 );
    translate.y -= u16( height ) >> u16( translate.z + 2 );
}

//--- zoom_in --------------------------------------------------------------------------------------
void zoom_in( vec3& translate )
{
    translate.x += u16( width ) >> u16( translate.z + 1 );
    translate.y += u16( height ) >> u16( translate.z + 1 );
}

//--- process_key ----------------------------------------------------------------------------------
void process_key( u32 value, bool press, vec3& translate )
{
    if ( !press )
        return;

    switch ( value )
    {
    case key_shift_left:
    case key_shift_right:
    case key_command_left:
    case key_command_right:
        break;

    case key_left:
        translate.x -= velocity;
        break;

    case key_right:
        translate.x += velocity;
        break;

    case key_up:
        translate.y -= velocity;
        break;

    case key_down:
        translate.y += velocity;
        break;

    case '-':
        translate.z -= 1;
        zoom_out( translate );
        break;

    case '=':
        translate.z += 1;
        zoom_in( translate );
        break;

    case '.':
        velocity = velocity ? velocity << 1 : 1;
        break;

    case ',':
        velocity = velocity ? velocity >> 1 : 1;
        break;

    case key_escape:
        executable::quit();
        break;

    default:
        break;
    }
}

//--- process_events -------------------------------------------------------------------------------
void process_events( event_queue& queue, vec3& translate, epoch_t timeout )
{
    int direction;
    event event;

    for ( event::type type; ( type = queue.dequeue( event, timeout ) );
        timeout = epoch_t( 0, 0 ) )
    {
        switch ( type )
        {
        case event::motion_flag:
        case event::button_flag:
            break;

        case event::key_flag:
            process_key( event.key.value, event.key.press, translate );
            break;

        case event::scroll_flag:
            translate.x += event.scroll.x;
            translate.y += event.scroll.y;
            break;

        case event::swipe_flag:
            break;

        case event::magnify_flag:
            direction = s16( translate.z + event.magnify.value ) - s16( translate.z );
            translate.z += event.magnify.value;

            if ( direction < 0 )
                zoom_out( translate );
            else if ( direction > 0 )
                zoom_in( translate );

            break;

        case event::rotate_flag:
        case event::gesture_begin:
        case event::gesture_end:
            break;

        case event::exit:
            executable::quit();
            break;

        default:
            break;
        }
    }
}

//--- make_input -----------------------------------------------------------------------------------
void make_input( const device_type& device, block_type& block, const box_tree::box_vector& boxes,
    f32 x, f32 y )
{
    buffer_type attribute = device->buffer( sizeof( f32 ) * 5 * boxes.size(), buffer::point );
    {
        map_type map = attribute->map( buffer::write );
        std::memcpy( map->data, &boxes[ 0 ], map->size );
    }

    block->input( "view", x, y );
    block->input( "scale", 2, attribute, true );
    block->input( "translate", 2, attribute, true );
    block->input( "depth", 1, attribute, true );
}

//--- make_block -----------------------------------------------------------------------------------
block_type make_block
    ( const program_type& program, const buffer_type& index, const buffer_type& point )
{
    block_type block = program->block( index );
    block->input( "vertex", 2, point );
    block->input( "projection", orthographic( 0, width, height, 0, -64, 64 ) );
    return block;
}

//--- make_shadow ----------------------------------------------------------------------------------
box_tree::box_vector make_shadow( const box_tree::box_vector& boxes )
{
    box_tree::box_vector shadows;

    for ( box_tree::box_vector::const_reverse_iterator i = boxes.rbegin(), end = boxes.rend();
        i != end; ++i )
    {
        f32 w = i->_0 + 12;
        f32 h = i->_1 + 16;
        f32 x = i->_2 - 6;
        f32 y = i->_3 - 6;
        shadows.push_back( make_tuple( w, h, x, y, i->_4 - .5 ) );
    }

    return shadows;
}

//--- make_text ------------------------------------------------------------------------------------
class make_text
{
public:
    make_text( const program_type& program_, const buffer_type& index_, const buffer_type& point_,
        text_layout& layout_ )
        : program( program_ ), index( index_ ), point( point_ ), layout( layout_ ), vector()
    {
    }

    node* operator ()( const std::string& string )
    {
        block_type block = make_block( program, index, point );
        vector.push_back( new text_node( layout, block, string ) );
        return vector.back();
    }

private:
    const program_type& program;
    const buffer_type& index;
    const buffer_type& point;
    text_layout& layout;
    node_vector vector;
};

//--- draw_aux -------------------------------------------------------------------------------------
void draw_aux( device_type& device, const frame_type& frame, const box_tree::box_vector& box,
    const box_tree::aux_vector& aux )
{
    for ( up_t i = 0, end = box.size(); i != end; ++i )
    {
        ooe::node* node = static_cast< ooe::node* >( aux[ i ]._4 );

        if ( !node )
            continue;

        node::block_tuple bt = node->block( box[ i ], aux[ i ] );
        device->draw( bt._0, frame, bt._1 );
    }
}

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string& root, const std::string&, s32, c8** )
{
    // graphics library must be preloaded for linux
    library library( root + "../lib/libopengl.so", library::global_lazy );

    event_queue queue;
    view view( queue, width, height, false );
    device_type device = library.find< device_open_type >( "device_open" )( view, true );

    thread_pool pool;
    page_cache cache( device, pool, format, page_size );
    frame_type frame = device->default_frame( width, height );
    buffer_type index = make_index( device );
    buffer_type point = make_point( device );

    // font rendering
    font::library font_library;
    font::face font_face( font_library, root + "../share/font/ubuntu-regular.ttf" );
    font_source font_source( font_face, 512, root + "../cache" );

    program_type program_font =
        make_program( device, root + "../share/glsl", root + "../share/json/tile.effect" );
    text_layout layout( device, cache, font_source );

    // tile rendering
    decoder_map decoder;
    decoder[ "png" ] = uncompressed_decode< png::decode >;
    /*tile_source tile_source( root + "../cache/blue-marble-d2", decoder );
    virtual_texture tile_texture( device, cache, tile_source );*/

    // box rendering
    make_text text( program_font, index, point, layout );
    node_map map;
    map[ "text" ] = make_function( text, &make_text::operator () );

    box_tree tree = make_tree( root + "../share/json/ui_tree.json", map );
    program_type program_box =
        make_program( device, root + "../share/glsl", root + "../share/json/box.effect" );

    block_type block_boxes = make_block( program_box, index, point );
    block_boxes->input( "shadow", false );
    block_type block_shadows = make_block( program_box, index, point );
    block_shadows->input( "shadow", true );
    device->set( device::depth_test, true );
    vec3 translate( 0, 0, 0 );

    while ( !executable::has_signal() )
    {
        box_unit x( std::max( 0.f, translate.x ), 0 );
        box_unit y( std::max( 0.f, translate.y ), 0 );
        box_tree::view_tuple vt = tree.view( width, height, x, y, translate.z );

        if ( vt._0.size() )
        {
            box_tree::box_vector shadows = make_shadow( vt._0 );
            f32 v_x = std::max( 0.f, -translate.x );
            f32 v_y = std::max( 0.f, -translate.y );

            make_input( device, block_boxes, vt._0, v_x, v_y );
            make_input( device, block_shadows, shadows, v_x, v_y );

            while ( cache.pending() )
                cache.write();

            frame->clear();
            device->set( device::blend, false );
            device->draw( block_boxes, frame, vt._0.size() );
            device->set( device::blend, true );
            draw_aux( device, frame, vt._0, vt._1 );
            device->draw( block_shadows, frame, shadows.size() );
            device->swap();
        }

        process_events( queue, translate, epoch_t( 3600, 0 ) );
        translate.z = std::max( 0.f, translate.z );
    }

    return true;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
