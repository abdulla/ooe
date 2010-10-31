/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "component/ui/box_tree.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/io/vfs.hpp"
#include "foundation/math/math.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/graphics.hpp"
#include "foundation/visual/view.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

const f32 width = 1024;
const f32 height = 640;
u32 acceleration = 4;

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
        translate.x -= acceleration;
        break;

    case key_right:
        translate.x += acceleration;
        break;

    case key_up:
        translate.y -= acceleration;
        break;

    case key_down:
        translate.y += acceleration;
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

//--- make_point -----------------------------------------------------------------------------------
buffer_type make_point( const device_type& device )
{
    buffer_type point = device->buffer( sizeof( f32 ) * 2 * 4, buffer::point );
    {
        map_type map = point->map( buffer::write );
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
buffer_type make_attribute( const device_type& device, const box_tree::box_vector& boxes )
{
    buffer_type attribute = device->buffer( sizeof( f32 ) * 5 * boxes.size(), buffer::point );
    {
        map_type map = attribute->map( buffer::write );
        std::memcpy( map->data, &boxes[ 0 ], map->size );
    }
    return attribute;
}

//--- make_block -----------------------------------------------------------------------------------
block_type make_block( const device_type& device, const program_type& program,
    const box_tree::box_vector& boxes )
{
    buffer_type point = make_point( device );
    buffer_type attribute = make_attribute( device, boxes );
    block_type block = program->block( make_index( device ) );
    block->input( "vertex", 2, point );
    block->input( "scale", 2, attribute, true );
    block->input( "translate", 2, attribute, true );
    block->input( "depth", 1, attribute, true );
    block->input( "projection", orthographic( 0, width, height, 0, -64, 64 ) );
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

//--- make_shadow ----------------------------------------------------------------------------------
box_tree::box_vector make_shadow( const box_tree::box_vector& boxes )
{
    box_tree::box_vector shadows;
    f32 level = -1024;
    f32 z;

    for ( box_tree::box_vector::const_reverse_iterator i = boxes.rbegin(), end = boxes.rend();
        i != end; ++i )
    {
        if ( !is_equal( level, i->_4 ) )
        {
            level = i->_4;
            z = 1. / 8;
        }

        f32 w = i->_0 + 12;
        f32 h = i->_1 + 16;
        f32 x = i->_2 - 6;
        f32 y = i->_3 - 6;
        shadows.push_back( make_tuple( w, h, x, y, i->_4 - z ) );
        z -= 1. / 256;
    }

    return shadows;
}

//--- make_box -------------------------------------------------------------------------------------
box make_box( const boost::property_tree::ptree& pt )
{
    u16 w = pt.get< u16 >( "width" );
    u16 h = pt.get< u16 >( "height" );
    u16 x = pt.get< u16 >( "x" );
    u16 y = pt.get< u16 >( "y" );
    return box( w, h, x, y );
}

//--- make_tree ------------------------------------------------------------------------------------
void make_tree( const boost::property_tree::ptree& pt, box_tree& bt, unit x, unit y, u16 z )
{
    for ( boost::property_tree::ptree::const_iterator i = pt.begin(), end = pt.end();
        i != end; ++i )
    {
        box box = make_box( i->second );
        u16 c = z + 1;
        unit b( y.integer + ( box.y >> c ), fmodf( box.y, 1 << c ) );
        unit a( x.integer + ( box.x >> c ), fmodf( box.x, 1 << c ) );

        if ( !bt.insert( box.width, box.height, a, b, z ) )
            throw error::runtime( "read_tree: " ) <<
                "Unable to insert: " << box.width << ' ' << box.height <<
                " ( " << a.integer << ", " << a.fraction << " )"
                " ( " << b.integer << ", " << b.fraction << " ) " << c << '\n';

        boost::optional< const boost::property_tree::ptree& > optional =
            i->second.get_child_optional( "children" );

        if ( optional )
            make_tree( *optional, bt, a, b, c );
    }
}

//--- read_tree ------------------------------------------------------------------------------------
box_tree read_tree( const std::string& path )
{
    boost::property_tree::ptree pt;
    read_json( canonical_path( path ), pt );

    box_tree bt( make_box( pt ) );
    make_tree( pt.get_child( "children" ), bt, unit( 0, 0 ), unit( 0, 0 ), 0 );
    return bt;
}

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string& root, const std::string&, s32, c8** )
{
    // graphics library must be preloaded for linux
    library library( root + "../lib/libopengl.so", library::global_lazy );

    event_queue event_queue;
    view view( event_queue, width, height, false );
    device_type device = library.find< device_open_type >( "device_open" )( view, true );

    box_tree tree = read_tree( root + "../share/json/box_tree.json" );
    shader_vector shaders = make_shaders( device, root );
    program_type program = device->program( shaders );
    frame_type frame = device->default_frame( width, height );

    device->set( device::depth_test, true );
    vec3 v( 0, 0, 0 );

    while ( !executable::has_signal() )
    {
        unit x( std::max( 0.f, v.x ), 0 );
        unit y( std::max( 0.f, v.y ), 0 );
        box_tree::box_vector boxes = tree.view( width, height, x, y, v.z );

        if ( boxes.size() )
        {
            box_tree::box_vector shadows = make_shadow( boxes );
            block_type block_boxes = make_block( device, program, boxes );
            block_type block_shadows = make_block( device, program, shadows );
            mat4 m = translate( mat4::identity,
                vec3( std::max( 0.f, -v.x ), std::max( 0.f, -v.y ), 0 ) );

            device->set( device::blend, false );
            block_boxes->input( "model_view", m );
            block_boxes->input( "shadow", false );
            device->set( device::blend, true );
            block_shadows->input( "model_view", m );
            block_shadows->input( "shadow", true );

            frame->clear();
            device->draw( block_boxes, frame, boxes.size() );
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
