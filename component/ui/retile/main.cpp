/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

#include "component/ui/tile_source.hpp"
#include "foundation/executable/program.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/parallel/thread_pool.hpp"
#include "foundation/utility/arithmetic.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

std::string root;
std::string decoder_type;
std::string encoder_type;
ooe::decoder_type decoder;
ooe::encoder_type encoder;

//--- recode ---------------------------------------------------------------------------------------
void recode( u32 size, u32 y, u8 level )
{
    for ( u32 x = 0; x != size; ++x )
    {
        pyramid_index index( x, y, level );

        std::string in_path = make_path( root, index, decoder_type );
        image image = decoder( in_path );

        erase( in_path );

        std::string out_path = make_path( root, index, encoder_type );
        encoder( image, descriptor( out_path, descriptor::write_new ) );
    }
}

//--- launch ---------------------------------------------------------------------------------------
bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
{
    for ( s32 option; ( option = getopt( argc, argv, "r:t:" ) ) != -1; )
    {
        switch ( option )
        {
        case 'r':
            root = canonical_path( optarg );
            break;

        case 't':
            encoder_type = optarg;
            break;

        default:
            std::cout <<
                "    -r <path>  Root directory containing tiles\n"
                "    -t <type>  Type to convert tiles to\n";

            return false;
        }
    }

    if ( root.empty() || encoder_type.empty() )
    {
        std::cout << "The directory and type must be specified\n";
        return false;
    }

    boost::property_tree::ptree pt;
    read_json( root + "/metadata", pt );
    decoder_type = pt.get< std::string >( "type" );
    decoder = find_decoder( decoder_type );
    encoder = find_encoder( encoder_type );

    u8 level_limit = pt.get< u8 >( "level_limit" );
    u32 width = pt.get< u32 >( "width" );
    u32 height = pt.get< u32 >( "height" );
    u16 page_size = 1 << u8( std::ceil( log2f( std::max( width, height ) ) ) - level_limit );
    u32 w = ceiling< u32 >( width, page_size );
    u32 h = ceiling< u32 >( height, page_size );
    thread_pool pool( "pool" );

    for ( s8 level = level_limit; level != -1; --level, w = ceiling( w, 2u ), h = ceiling( h, 2u ) )
    {
        for ( u32 y = 0; y != h; ++y )
            async( pool, make_function( recode ), w, y, level );
    }

    pt.put( "type", encoder_type );
    write_json( root + "/metadata", pt );
    return true;
}

OOE_ANONYMOUS_END( ( ooe ) )

//--- main -----------------------------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
    return executable::launch( launch, argc, argv );
}
