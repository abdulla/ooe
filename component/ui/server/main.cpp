/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cstring>

#include "component/ui/make.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/math/math.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/graphics.hpp"
#include "foundation/visual/view.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const f32 width = 1024;
const f32 height = 640;
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
void process_events( event_queue& event_queue, vec3& translate, epoch_t timeout )
{
    int direction;
    event event;

    for ( event::type type; ( type = event_queue.next_event( event, timeout ) );
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
void make_input( const device_type& device, block_type& block, const box_tree::box_vector& boxes )
{
    buffer_type attribute = device->buffer( sizeof( f32 ) * 5 * boxes.size(), buffer::point );
    {
        map_type map = attribute->map( buffer::write );
        std::memcpy( map->data, &boxes[ 0 ], map->size );
    }

    block->input( "scale", 2, attribute, true );
    block->input( "translate", 2, attribute, true );
    block->input( "depth", 1, attribute, true );
}

//--- make_block -----------------------------------------------------------------------------------
block_type make_block( const device_type& device, const program_type& program, bool shadow )
{
    block_type block = program->block( make_index( device ) );
    block->input( "vertex", 2, make_point( device ) );
    block->input( "projection", orthographic( 0, width, height, 0, -64, 64 ) );
    block->input( "shadow", shadow );
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

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string& root, const std::string&, s32, c8** )
{
    // graphics library must be preloaded for linux
    library library( root + "../lib/libopengl.so", library::global_lazy );

    event_queue event_queue;
    view view( event_queue, width, height, false );
    device_type device = library.find< device_open_type >( "device_open" )( view, true );

    box_tree tree = make_tree( root + "../share/json/box_tree.json" );
    program_type program =
        make_program( device, root + "../share/glsl", root + "../share/json/box.effect" );
    frame_type frame = device->default_frame( width, height );

    block_type block_boxes = make_block( device, program, false );
    block_type block_shadows = make_block( device, program, true );
    device->set( device::depth_test, true );
    vec3 v( 0, 0, 0 );

    while ( !executable::has_signal() )
    {
        box_unit x( std::max( 0.f, v.x ), 0 );
        box_unit y( std::max( 0.f, v.y ), 0 );
        box_tree::box_vector boxes = tree.view( width, height, x, y, v.z );

        if ( boxes.size() )
        {
            f32 t_x = std::max( 0.f, -v.x );
            f32 t_y = std::max( 0.f, -v.y );
            box_tree::box_vector shadows = make_shadow( boxes );

            block_boxes->input( "view", t_x, t_y );
            make_input( device, block_boxes, boxes );
            block_shadows->input( "view", t_x, t_y );
            make_input( device, block_shadows, shadows );

            frame->clear();
            device->set( device::blend, false );
            device->draw( block_boxes, frame, boxes.size() );
            device->set( device::blend, true );
            device->draw( block_shadows, frame, shadows.size() );
            device->swap();
        }

        process_events( event_queue, v, epoch_t( 3600, 0 ) );
        v.z = std::max( 0.f, v.z );
    }

    return true;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 OOE_WEAK main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
