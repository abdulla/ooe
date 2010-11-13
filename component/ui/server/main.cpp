/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cmath>
#include <cstring>

#include <boost/property_tree/ptree.hpp>

#include "component/ui/box_tree.hpp"
#include "component/ui/make.hpp"
#include "component/ui/text_layout.hpp"
#include "component/ui/tile_source.hpp"
#include "foundation/executable/library.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/math/math.hpp"
#include "foundation/parallel/thread_pool.hpp"
#include "foundation/visual/event_queue.hpp"
#include "foundation/visual/graphics.hpp"
#include "foundation/visual/view.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

typedef std::vector< shared_ptr< node > > node_vector;

const f32 width = 1024;
const f32 height = 640;
const u16 page_size = 256;
const image::type format = image::rgba_u8;
u32 velocity = 4;

//--- zoom_out -------------------------------------------------------------------------------------
void zoom_out( vec3& move )
{
    if ( move.z < 0 )
        return;

    move.x -= u16( width ) >> u16( move.z + 2 );
    move.y -= u16( height ) >> u16( move.z + 2 );
}

//--- zoom_in --------------------------------------------------------------------------------------
void zoom_in( vec3& move )
{
    move.x += u16( width ) >> u16( move.z + 1 );
    move.y += u16( height ) >> u16( move.z + 1 );
}

//--- process_key ----------------------------------------------------------------------------------
void process_key( u32 value, bool press, vec3& move )
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
        zoom_out( move );
        break;

    case '=':
        move.z += 1;
        zoom_in( move );
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
void process_events( event_queue& queue, vec3& move, epoch_t timeout )
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
            process_key( event.key.value, event.key.press, move );
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
                zoom_out( move );
            else if ( direction > 0 )
                zoom_in( move );

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

//--- text_node ------------------------------------------------------------------------------------
class text_node
    : public node
{
public:
    text_node( const block_type& in, text_layout& layout_, const property_tree& tree )
        : tuple( in, 0 ), layout( layout_ ), text()
    {
        text.data = tree.get< std::string >( "text" );
        text.x = tree.get( "x", 0 );
        text.y = tree.get( "y", 0 );
        text.red = tree.get( "red", 0 );
        text.green = tree.get( "green", 0 );
        text.blue = tree.get( "blue", 0 );
        text.level = 255;
    }

    virtual ~text_node( void )
    {
    }

    virtual block_tuple block( const box_tree::box_tuple& box, const box_tree::aux_tuple& )
    {
        tuple._0->input( "translate", box._2, box._3 );
        u8 level = std::max( 0.f, 5 - box._4 );

        if ( level == text.level )
            return tuple;

        text.level = level;
        tuple._0->input( "depth", box._4 );
        tuple._1 = layout.input( tuple._0, text, box._0 );
        return tuple;
    }

private:
    block_tuple tuple;
    text_layout& layout;
    ooe::text text;
};

//--- make_text ------------------------------------------------------------------------------------
class make_text
{
public:
    make_text( const device_type& device, const std::string& root, const buffer_type& index_,
        const buffer_type& point_, page_cache& cache )
        : program( make_program( device, root + "../share/glsl",
        root + "../share/json/text.effect" ) ), index( index_ ), point( point_ ), library(),
        face( library, root + "../share/font/ubuntu-regular.ttf" ),
        source( face, 512, root + "../cache" ), layout( device, cache, source ), vector()
    {
    }

    node* operator ()( const property_tree& tree )
    {
        block_type block = make_block( program, index, point );
        vector.push_back( new text_node( block, layout, tree ) );
        return vector.back();
    }

private:
    const program_type program;
    const buffer_type& index;
    const buffer_type& point;
    font::library library;
    font::face face;
    font_source source;
    text_layout layout;
    node_vector vector;
};

//--- tile_node ------------------------------------------------------------------------------------
class tile_node
    : public node
{
public:
    tile_node( const block_type& data_, const device_type& device, page_cache& cache,
        tile_source& source_ )
        : data( data_ ), source( source_ ), texture( device, cache, source )
    {
        texture.input( data, "texture" );
    }

    virtual ~tile_node( void )
    {
    }

    virtual block_tuple block( const box_tree::box_tuple& box, const box_tree::aux_tuple& aux )
    {
        u32 size = source.size();
        u8 level = std::max( 0.f, log2f( size / box._0 ) );
        texture.load( aux._2 * size, aux._3 * size, aux._0 * size, aux._1 * size, level );

        data->input( "scale", box._0, box._1 );
        data->input( "translate", box._2, box._3 );
        data->input( "depth", box._4 );
        return make_tuple( data, 1 );
    }

private:
    block_type data;
    tile_source source;
    virtual_texture texture;
};

//--- make_tile ------------------------------------------------------------------------------------
class make_tile
{
public:
    make_tile( const device_type& device_, const std::string& root_, const buffer_type& index_,
        const buffer_type& point_, page_cache& cache_ )
        : device( device_ ), root( root_ ), program( make_program( device, root + "../share/glsl",
        root + "../share/json/tile.effect" ) ), index( index_ ), point( point_ ), cache( cache_ ),
        vector()
    {
    }

    node* operator ()( const property_tree& tree )
    {
        block_type block = make_block( program, index, point );
        tile_source source( root + "../" + tree.get< std::string >( "tile" ) );
        vector.push_back( new tile_node( block, device, cache, source ) );
        return vector.back();
    }

private:
    const device_type& device;
    const std::string& root;
    const program_type program;
    const buffer_type& index;
    const buffer_type& point;
    page_cache& cache;
    node_vector vector;
};

//--- draw_aux -------------------------------------------------------------------------------------
void draw_aux( device_type& device, const frame_type& frame, const box_tree::box_vector& box,
    const box_tree::aux_vector& aux, f32 x, f32 y )
{
    for ( up_t i = 0, end = box.size(); i != end; ++i )
    {
        ooe::node* node = static_cast< ooe::node* >( aux[ i ]._4 );

        if ( !node )
            continue;

        node::block_tuple bt = node->block( box[ i ], aux[ i ] );

        if ( !bt._1 )
            continue;

        bt._0->input( "view", x, y );
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
    page_cache cache( device, pool, page_size, format );
    frame_type frame = device->default_frame( width, height );
    buffer_type index = make_index( device );
    buffer_type point = make_point( device );

    make_text text( device, root, index, point, cache );
    make_tile tile( device, root, index, point, cache );
    node_map map;
    map[ "text" ] = make_function( text, &make_text::operator () );
    map[ "tile" ] = make_function( tile, &make_tile::operator () );

    // box rendering
    box_tree tree = make_tree( root + "../share/json/ui_tree.json", map );
    program_type program_box =
        make_program( device, root + "../share/glsl", root + "../share/json/box.effect" );

    block_type block_boxes = make_block( program_box, index, point );
    block_boxes->input( "shadow", false );
    block_type block_shadows = make_block( program_box, index, point );
    block_shadows->input( "shadow", true );
    device->set( device::depth_test, true );
    vec3 move( 0, 0, 0 );

    while ( !executable::has_signal() )
    {
        box_tree::view_tuple vt =
            tree.view( width, height, make_unit( move.x ), make_unit( move.y ), move.z );

        if ( vt._0.size() )
        {
            box_tree::box_vector shadows = make_shadow( vt._0 );
            f32 v_x = std::max( 0.f, -move.x * exp2f( move.z ) );
            f32 v_y = std::max( 0.f, -move.y * exp2f( move.z ) );

            make_input( device, block_boxes, vt._0, v_x, v_y );
            make_input( device, block_shadows, shadows, v_x, v_y );

            while ( cache.pending() )
                cache.write();

            frame->clear();
            device->set( device::blend, false );
            device->draw( block_boxes, frame, vt._0.size() );
            device->set( device::blend, true );
            draw_aux( device, frame, vt._0, vt._1, v_x, v_y );
            device->draw( block_shadows, frame, shadows.size() );
            device->swap();
        }

        process_events( queue, move, epoch_t( 3600, 0 ) );
        move.z = std::max( 0.f, move.z );
    }

    return true;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
