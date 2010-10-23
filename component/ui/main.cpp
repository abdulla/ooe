/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cmath>

#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/io/vfs.hpp"
#include "foundation/math/math.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/graphics.hpp"
#include "foundation/visual/view.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const f32 width = 1024;
const f32 height = 640;
u32 acceleration = 4;

//--- process_key ----------------------------------------------------------------------------------
void process_key( u32 value, bool press, vec3& translate, vec3& scale )
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
        translate.x += acceleration;
        break;

    case key_right:
        translate.x -= acceleration;
        break;

    case key_up:
        translate.y += acceleration;
        break;

    case key_down:
        translate.y -= acceleration;
        break;

    case '=':
        scale.x *= 2;
        scale.y *= 2;
        break;

    case '-':
        scale.x /= 2;
        scale.y /= 2;
        break;

    case '.':
        acceleration = acceleration ? acceleration << 1 : 1;
        break;

    case ',':
        acceleration = acceleration ? acceleration >> 1 : 1;
        break;

    case key_escape:
        executable::quit();
        break;

    default:
        break;
    }
}

//--- process_events -------------------------------------------------------------------------------
void process_events( event_queue& event_queue, vec3& translate, vec3& scale, epoch_t timeout )
{
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
            process_key( event.key.value, event.key.press, translate, scale );
            break;

        case event::exit:
            executable::quit();
            break;

        default:
            break;
        }
    }
}

//--- make_point -----------------------------------------------------------------------------------
buffer_type make_point( const device_type& device )
{
    buffer_type point = device->buffer( sizeof( f32 ) * 4 * 4, buffer::point );
    {
        map_type map = point->map( buffer::write );
        f32* value = static_cast< f32* >( map->data );

        // top left
        value[ 0 ] = -.5;
        value[ 1 ] = .5;
        value[ 2 ] = 0;
        value[ 3 ] = 1;

        // bottom left
        value[ 4 ] = -.5;
        value[ 5 ] = -.5;
        value[ 6 ] = 0;
        value[ 7 ] = 0;

        // top right
        value[ 8 ] = .5;
        value[ 9 ] = .5;
        value[ 10 ] = 1;
        value[ 11 ] = 1;

        // bottom right
        value[ 12 ] = .5;
        value[ 13 ] = -.5;
        value[ 14 ] = 1;
        value[ 15 ] = 0;
    }
    return point;
}

//--- make_index -----------------------------------------------------------------------------------
buffer_type make_index( const device_type& device )
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

//--- make_attribute -------------------------------------------------------------------------------
buffer_type make_attribute( const device_type& device )
{
    buffer_type attribute = device->buffer( sizeof( f32 ) * 5 * 2, buffer::point );
    {
        map_type map = attribute->map( buffer::write );
        f32* value = static_cast< f32* >( map->data );

        value[ 0 ] = 100;
        value[ 1 ] = 100;
        value[ 2 ] = 100;
        value[ 3 ] = 200;
        value[ 4 ] = 0;

        value[ 5 ] = 200;
        value[ 6 ] = 200;
        value[ 7 ] = 200;
        value[ 8 ] = 100;
        value[ 9 ] = 0;
    }
    return attribute;
}

//--- make_block -----------------------------------------------------------------------------------
block_type make_block( const device_type& device, const program_type& program )
{
    buffer_type point = make_point( device );
    buffer_type attribute = make_attribute( device );
    block_type block = program->block( make_index( device ) );
    block->input( "vertex", 2, point );
    block->input( "coords", 2, point );
    block->input( "translate", 2, attribute, true );
    block->input( "scale", 2, attribute, true );
    block->input( "z_index", 1, attribute, true );
    block->input( "projection", orthographic( 0, width, height, 0, 0, 64 ) );
    return block;
}

//--- make_shaders ---------------------------------------------------------------------------------
shader_vector make_shaders( const device_type& device, const std::string& root )
{
    vfs vfs;
    vfs.insert( root + "../share/glsl", "/" );
    shader_include include( device, vfs );

    shader_vector vector;
    vector.push_back( include.compile( "box.vs", shader::vertex ) );
    vector.push_back( include.compile( "box.fs", shader::fragment ) );
    return vector;
}

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string& root, const std::string&, s32, c8** )
{
    // graphics library must be preloaded for linux
    library library( root + "../lib/libopengl.so", library::global_lazy );

    event_queue event_queue;
    view view( event_queue, width, height, false );
    device_type device = library.find< device_open_type >( "device_open" )( view, true );

    shader_vector shaders = make_shaders( device, root );
    program_type program = device->program( shaders );
    block_type block = make_block( device, program );
    frame_type frame = device->default_frame( width, height );

    vec3 t( width / 2, height / 2, 0 );
    vec3 s( 1, 1, 1 );
    device->set( device::blend, true );

    while ( !executable::has_signal() )
    {
        frame->clear();
        block->input( "model_view", translate( mat4::identity, t ) * scale( mat4::identity, s ) );

        device->draw( block, frame, 2 );
        device->swap();

        process_events( event_queue, t, s, epoch_t( 3600, 0 ) );
    }

    return true;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 OOE_WEAK main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
