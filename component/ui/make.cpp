/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "component/ui/make.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/io/memory.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

//--- make_box -------------------------------------------------------------------------------------
box make_box( const boost::property_tree::ptree& pt )
{
    u16 w = pt.get< u16 >( "width" );
    u16 h = pt.get< u16 >( "height" );
    u16 x = pt.get< u16 >( "x" );
    u16 y = pt.get< u16 >( "y" );
    return box( w, h, x, y );
}

//--- load_tree ------------------------------------------------------------------------------------
void load_tree( const boost::property_tree::ptree& pt, box_tree& bt )
{
    for ( boost::property_tree::ptree::const_iterator i = pt.begin(), end = pt.end();
        i != end; ++i )
    {
        box box = make_box( i->second );
        box_tree::iterator j = bt.insert( box.width, box.height, box.x, box.y );

        if ( j == bt.end() )
            throw error::runtime( "read_tree: " ) << "Unable to insert box ( " << box.width <<
                ' ' << box.height << ' ' << box.x << ' ' << box.y << " )";

        boost::optional< const boost::property_tree::ptree& > optional =
            i->second.get_child_optional( "children" );

        if ( optional )
            load_tree( *optional, *j );
    }
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- make_program ---------------------------------------------------------------------------------
program_type make_program
    ( const device_type& device, const std::string& root, const std::string& path )
{
    boost::property_tree::ptree pt;
    read_json( canonical_path( path ), pt );
    std::string header;
    shader_vector vector;

    struct id_type
    {
        const c8* name;
        shader::type type;
    } id[] =
    {
        { "header", shader::vertex },
        { "vertex", shader::vertex },
        { "geometry", shader::geometry },
        { "fragment", shader::fragment }
    };

    for ( id_type* i = id, * i_end = i + ( sizeof( id ) / sizeof( *id ) ); i != i_end; ++i )
    {
        boost::optional< boost::property_tree::ptree& > child = pt.get_child_optional( i->name );

        if ( !child )
            continue;

        for ( boost::property_tree::ptree::iterator j = child->begin(), j_end = child->end();
            j != j_end; ++j )
        {
            memory memory( root + '/' + j->second.data() );
            std::string source( memory.as< c8 >(), memory.size() );

            if ( i == id )
                header << source << '\n';
            else
            {
                shader_type shader = device->shader( header + source, i->type );
                vector.push_back( shader );
            }
        }
    }

    return device->program( vector );
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

//--- make_tree ------------------------------------------------------------------------------------
box_tree make_tree( const std::string& path )
{
    boost::property_tree::ptree pt;
    read_json( canonical_path( path ), pt );

    box_tree bt( make_box( pt ) );
    load_tree( pt.get_child( "children" ), bt );
    return bt;
}

OOE_NAMESPACE_END( ( ooe ) )
