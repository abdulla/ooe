/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cmath>
#include <cstring>

#include <boost/property_tree/ptree.hpp>

#include "component/ui/box_tree.hpp"
#include "component/ui/convert_source.hpp"
#include "component/ui/make.hpp"
#include "component/ui/text_layout.hpp"
#include "component/ui/tile_source.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/math/math.hpp"
#include "foundation/parallel/thread_pool.hpp"
#include "foundation/utility/arithmetic.hpp"
#include "foundation/utility/binary.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/graphics.hpp"
#include "foundation/visual/view.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef tuple< f32, f32 > pin_type;

const f32 width = 1024;
const f32 height = 640;
const u16 page_size = 256;
const image_format::type format = image_format::rgba_u8;
u32 velocity = 4;

//--- zoom_out -------------------------------------------------------------------------------------
void zoom_out( vec3& move, pin_type& pin )
{
    if ( move.z < 0 )
        return;

    move.x -= u16( pin._0 ) >> u16( move.z + 1 );
    move.y -= u16( pin._1 ) >> u16( move.z + 1 );
}

//--- zoom_in --------------------------------------------------------------------------------------
void zoom_in( vec3& move, pin_type& pin )
{
    move.x += u16( pin._0 ) >> u16( move.z );
    move.y += u16( pin._1 ) >> u16( move.z );
}

//--- process_key ----------------------------------------------------------------------------------
void process_key( u32 value, bool press, vec3& move, pin_type& pin )
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
        move.x -= velocity / exp2f( move.z );
        break;

    case key_right:
        move.x += velocity / exp2f( move.z );
        break;

    case key_up:
        move.y -= velocity / exp2f( move.z );
        break;

    case key_down:
        move.y += velocity / exp2f( move.z );
        break;

    case '-':
        move.z -= 1;
        zoom_out( move, pin );
        break;

    case '=':
        move.z += 1;
        zoom_in( move, pin );
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
void process_events( event_queue& queue, vec3& move, pin_type& pin, epoch_t timeout )
{
    int direction;
    event event;

    for ( event::type type; ( type = queue.dequeue( event, timeout ) );
        timeout = epoch_t( 0, 0 ) )
    {
        switch ( type )
        {
        case event::motion_flag:
            pin = pin_type( event.motion.x, event.motion.y );
            break;

        case event::button_flag:
            break;

        case event::key_flag:
            process_key( event.key.value, event.key.press, move, pin );
            break;

        case event::scroll_flag:
            move.x += event.scroll.x / exp2f( move.z );
            move.y += event.scroll.y / exp2f( move.z );
            break;

        case event::swipe_flag:
            break;

        case event::magnify_flag:
            direction = s16( move.z + event.magnify.value ) - s16( move.z );
            move.z += event.magnify.value;

            if ( direction < 0 )
                zoom_out( move, pin );
            else if ( direction > 0 )
                zoom_in( move, pin );

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

//--- make_unit ------------------------------------------------------------------------------------
box_unit make_unit( f32 value )
{
    value = std::max( 0.f, value );
    return box_unit( value, std::fmod( value, 1 ) );
}

//--- make_input -----------------------------------------------------------------------------------
u32 make_input( const device_ptr& device, block_ptr& block, const box_tree::box_vector& box,
    const box_tree::aux_vector& aux, f32 x, f32 y )
{
    u32 box_size = 0;

    for ( box_tree::aux_vector::const_iterator j = aux.begin(), end = aux.end(); j != end; ++j )
    {
        if ( static_cast< colour_node* >( j->_4 )->colour.alpha )
            ++box_size;
    }

    if ( !box_size )
        return 0;

    u8 point_size = 5 * sizeof( f32 ) + 4 * sizeof( u8 );
    buffer_ptr point = device->buffer( point_size * box_size, buffer::point );
    {
        map_ptr map = point->map( buffer::write );
        void* data = map->data;

        for ( up_t i = box.size(), j = 0; i-- && j != box_size; )
        {
            ooe::colour& colour = static_cast< colour_node* >( aux[ i ]._4 )->colour;

            if ( !colour.alpha )
                continue;

            std::memcpy( data, &box[ i ], 5 * sizeof( f32 ) );
            data = add< void >( data, 5 * sizeof( f32 ) );
            std::memcpy( data, &colour, 4 * sizeof( u8 ) );
            data = add< void >( data, 4 * sizeof( u8 ) );

            ++j;
        }
    }

    block->input( "view", x, y );
    block->input( "scale", block::f32_2, true, point );
    block->input( "translate", block::f32_2, true, point );
    block->input( "depth", block::f32_1, true, point );
    block->input( "colour", block::u8_4, true, point );
    return box_size;
}

//--- make_block -----------------------------------------------------------------------------------
block_ptr make_block( const program_ptr& program, const buffer_ptr& index, const buffer_ptr& point )
{
    block_ptr block = program->block( index );
    block->input( "vertex", block::f32_2, false, point );
    block->input( "projection", orthographic( 0, width, height, 0, -64, 64 ) );
    return block;
}

//--- text_node ------------------------------------------------------------------------------------
class text_node
    : public node
{
public:
    text_node( const property_tree& tree, const block_ptr& in, text_layout& layout_ )
        : tuple( in, 0 ), layout( layout_ ), text( tree.size() ), level( -128 )
    {
        up_t j = 0;

        for ( property_tree::const_iterator i = tree.begin(), end = tree.end(); i != end; ++i, ++j )
        {
            text[ j ].data = i->second.get< std::string >( "text" );
            text[ j ].x = i->second.get( "x", 0 );
            text[ j ].y = i->second.get( "y", 0 );
            text[ j ].level = i->second.get( "level", 5 );
            text[ j ].colour = make_colour( i->second, 255 );
        }
    }

    virtual ~text_node( void )
    {
    }

    virtual block_tuple block( const box_tree::box_tuple& box, const box_tree::aux_tuple& aux )
    {
        tuple._0->input( "translate", box._2, box._3 );
        s8 box_level = -box._4;

        if ( box_level == level )
            return tuple;

        level = box_level;
        colour& colour = static_cast< colour_node* >( aux._4 )->colour;
        tuple._0->input( "depth", box._4 );
        tuple._0->input( "background", colour.red, colour.green, colour.blue, colour.alpha );
        tuple._1 = layout.input( tuple._0, text, box._0, level );
        return tuple;
    }

private:
    block_tuple tuple;
    text_layout& layout;
    text_vector text;
    s8 level;
};

//--- make_text ------------------------------------------------------------------------------------
class make_text
{
public:
    make_text( const device_ptr& device, const std::string& root, const buffer_ptr& index_,
        const buffer_ptr& point_, page_cache& cache )
        : program( make_program( device, root + "../share/glsl",
        root + "../share/json/text.effect" ) ), index( index_ ), point( point_ ), library(),
        face( library, root + "../share/font/ubuntu-regular.ttf" ),
        source( face, 512, root + "../cache" ), layout( device, cache, source )
    {
    }

    node* operator ()( const property_tree& tree )
    {
        block_ptr block = make_block( program, index, point );
        return new text_node( tree, block, layout );
    }

private:
    const program_ptr program;
    const buffer_ptr& index;
    const buffer_ptr& point;
    font::library library;
    font::face face;
    font_source source;
    text_layout layout;
};

//--- tile_node ------------------------------------------------------------------------------------
class tile_node
    : public node
{
public:
    tile_node( const block_ptr& data_, const device_ptr& device,
        page_cache& cache, tile_source& tile_ )
        : data( data_ ), tile( tile_ ), convert( tile, cache.format() ),
        texture( device, cache, convert )
    {
        texture.input( data, "texture" );
    }

    virtual ~tile_node( void )
    {
    }

    virtual block_tuple block( const box_tree::box_tuple& box, const box_tree::aux_tuple& aux )
    {
        u32 size = tile.size();
        u8 level_limit = log2f( size / tile.page_size() );
        tile_source::area_tuple area = tile.area();
        u8 level = clamp< s8 >( log2f( area._0 / box._0 ), 0, level_limit );
        s32 w = aux._0 * area._0;
        s32 h = aux._1 * area._1;
        s32 x = aux._2 * area._0;
        s32 y = aux._3 * area._1;

        x = std::max( 0, x - w );
        y = std::max( 0, y - h );
        w = std::min< s32 >( w * 3, area._0 - x );
        h = std::min< s32 >( h * 3, area._1 - y );
        texture.load( w, h, x, y, level );

        if ( level != level_limit )
            texture.load( w, h, x, y, level + 1 );

        u32 border = ( 1 << level ) - 1;
        f32 u = divide( area._0 - border, size );
        f32 v = divide( area._1 - border, size );

        data->input( "scale", box._0, box._1 );
        data->input( "translate", box._2, box._3 );
        data->input( "depth", box._4 );
        data->input( "texcoord", u, v );
        return make_tuple( data, 1 );
    }

private:
    block_ptr data;
    tile_source tile;
    convert_source convert;
    virtual_texture texture;
};

//--- make_tile ------------------------------------------------------------------------------------
class make_tile
{
public:
    make_tile( const device_ptr& device_, const std::string& root_, const buffer_ptr& index_,
        const buffer_ptr& point_, page_cache& cache_ )
        : device( device_ ), root( root_ ), program( make_program( device, root + "../share/glsl",
        root + "../share/json/tile.effect" ) ), index( index_ ), point( point_ ), cache( cache_ )
    {
    }

    node* operator ()( const property_tree& tree )
    {
        block_ptr block = make_block( program, index, point );
        tile_source source( root + "../" + tree.get< std::string >( "tile" ) );
        return new tile_node( block, device, cache, source );
    }

private:
    const device_ptr& device;
    const std::string& root;
    const program_ptr program;
    const buffer_ptr& index;
    const buffer_ptr& point;
    page_cache& cache;
};

//--- draw_aux -------------------------------------------------------------------------------------
void draw_aux( device_ptr& device, const frame_ptr& frame, const box_tree::box_vector& box,
    const box_tree::aux_vector& aux, f32 x, f32 y )
{
    for ( up_t i = 0, end = box.size(); i != end; ++i )
    {
        ooe::node* node = static_cast< colour_node* >( aux[ i ]._4 )->node;

        if ( !node )
            continue;

        node::block_tuple bt = node->block( box[ i ], aux[ i ] );
        bt._0->input( "view", x, y );
        device->draw( bt._0, frame, bt._1 );
    }
}

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string& root, const std::string&, s32, c8** )
{
    // graphics library must be preloaded for linux
    library library( root + "../lib/libopengl" OOE_EXTENSION, library::global_lazy );

    event_queue queue;
    view view( queue, width, height, false );
    device_ptr device = library.find< device_open_type >( "device_open" )( view, true );

    thread_pool pool( "pool" );
    page_cache cache( device, pool, page_size, format );
    frame_ptr frame = device->default_frame( width, height );
    buffer_ptr index = make_index( device );
    buffer_ptr point = make_point( device );

    make_text text( device, root, index, point, cache );
    make_tile tile( device, root, index, point, cache );
    node_map map;
    map[ "text" ] = make_function( text, &make_text::operator () );
    map[ "tile" ] = make_function( tile, &make_tile::operator () );

    // box rendering
    box_tree tree = make_tree( root + "../share/json/ui_tree.json", map );
    program_ptr program =
        make_program( device, root + "../share/glsl", root + "../share/json/box.effect" );

    block_ptr block = make_block( program, index, point );
    device->set( device::blend, true );
    device->set( device::depth_test, true );
    vec3 move( 0, 0, 0 );
    pin_type pin( width / 2, height / 2 );

    while ( !executable::has_signal() )
    {
        box_tree::view_tuple vt =
            tree.view( width, height, make_unit( move.x ), make_unit( move.y ), move.z );

        if ( vt._0.size() )
        {
            frame->clear();
            f32 v_x = std::max( 0.f, -move.x * exp2f( move.z ) );
            f32 v_y = std::max( 0.f, -move.y * exp2f( move.z ) );
            u32 instances = make_input( device, block, vt._0, vt._1, v_x, v_y );

            if ( instances )
                device->draw( block, frame, instances );

            draw_aux( device, frame, vt._0, vt._1, v_x, v_y );
            device->swap();
        }

        epoch_t timeout( 3600, 0 );

        if ( cache.pending() )
        {
            cache.write();
            timeout = epoch_t( 0, 0 );
        }

        process_events( queue, move, pin, timeout );
        move.z = std::max( 0.f, move.z );
    }

    return true;
}

OOE_ANONYMOUS_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
