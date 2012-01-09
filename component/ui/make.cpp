/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "component/ui/make.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/io/memory.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

//--- make_box -------------------------------------------------------------------------------------
box make_box( const property_tree& pt )
{
    u16 w = pt.get< u16 >( "width" );
    u16 h = pt.get< u16 >( "height" );
    u16 x = pt.get< u16 >( "x" );
    u16 y = pt.get< u16 >( "y" );
    return box( w, h, x, y );
}

//--- make_aux -------------------------------------------------------------------------------------
opaque_ptr make_aux( const property_tree& pt, const node_map& map )
{
    boost::optional< std::string > node = pt.get_optional< std::string >( "node" );
    boost::optional< const property_tree& > data = pt.get_child_optional( "data" );

    if ( !node || !data )
        return opaque_ptr( static_cast< s32* >( 0 ) );

    node_map::const_iterator i = map.find( *node );

    if ( i == map.end() )
        throw error::runtime( "make_tree: " ) << "Unknown node \"" << *node << '\"';

    return opaque_ptr( i->second( *data ) );
}

//--- load_tree ------------------------------------------------------------------------------------
void load_tree( const property_tree& pt, const node_map& node, box_tree& bt )
{
    for ( property_tree::const_iterator i = pt.begin(), end = pt.end(); i != end; ++i )
    {
        box box = make_box( i->second );
        colour colour = make_colour( i->second );
        opaque_ptr pointer = make_aux( i->second, node );
        box_tree::iterator j = bt.insert( box, colour, pointer );

        if ( j == bt.end() )
            throw error::runtime( "read_tree: " ) << "Unable to insert box ( " << box.width <<
                ' ' << box.height << ' ' << box.x << ' ' << box.y << " )";

        boost::optional< const property_tree& > children =
            i->second.get_child_optional( "children" );

        if ( children )
            load_tree( *children, node, *j );
    }
}

//--- make_shader ----------------------------------------------------------------------------------
shader_ptr make_shader( const std::string& name, const device_ptr& device,
    const std::string& source, shader::type type )
{
    try
    {
        return device->shader( source, type );
    }
    catch ( error::runtime& error )
    {
        throw error::runtime( "make_shader: " ) << '\"' << name << "\": " << error.what();
    }
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- make_program ---------------------------------------------------------------------------------
program_ptr make_program
    ( const device_ptr& device, const std::string& root, const std::string& path )
{
    property_tree pt;
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
        boost::optional< property_tree& > child = pt.get_child_optional( i->name );

        if ( !child )
            continue;

        for ( property_tree::iterator j = child->begin(), j_end = child->end(); j != j_end; ++j )
        {
            std::string name = j->second.data();
            memory memory( root + '/' + name );
            std::string source( memory.as< c8 >(), memory.size() );

            if ( i == id )
                header << source << '\n';
            else
            {
                vector.push_back( make_shader( name, device, header + source, i->type ) );
            }
        }
    }

    return device->program( vector );
}

//--- make_colour ----------------------------------------------------------------------------------
colour make_colour( const property_tree& pt )
{
    boost::optional< const property_tree& > child = pt.get_child_optional( "colour" );
    u8 rgba[] = { 0, 0, 0, 255 };

    if ( child )
    {
        property_tree::const_iterator j = child->begin();
        property_tree::const_iterator end = child->end();

        for ( up_t i = 0; i != sizeof( rgba ) && j != end; ++i, ++j )
            rgba[ i ] = j->second.get_value< u8 >();
    }

    return colour( rgba[ 0 ], rgba[ 1 ], rgba[ 2 ], rgba[ 3 ] );
}

//--- make_tree ------------------------------------------------------------------------------------
box_tree make_tree( const std::string& path, const node_map& node )
{
    property_tree pt;
    read_json( canonical_path( path ), pt );

    box_tree bt( make_box( pt ), make_colour( pt ), make_aux( pt, node ) );
    load_tree( pt.get_child( "children" ), node, bt );
    return bt;
}

OOE_NAMESPACE_END( ( ooe ) )
