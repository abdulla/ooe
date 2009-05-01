/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cstdlib>

#include "foundation/executable/program.hpp"
#include "foundation/image/jpeg.hpp"
#include "foundation/image/png.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/utility/error.hpp"

#include "external/graph/resource.hpp"
#include "external/tools/bt.hpp"
#include "external/tools/height.hpp"
#include "external/tools/interface.hpp"
#include "external/tools/md2.hpp"

namespace
{
	using namespace ooe;

	f32 scale_value[ 3 ] = { 0 };
	u32 frame_index = 0;

	uncompressed_image load_image( const std::string& path )
	{
		resource< image_decode_t > decoder;
		decoder.insert( "jpg", uncompressed_decode< jpeg::decode > );
		decoder.insert( "png", uncompressed_decode< png::decode > );
		return decoder[ path ]( descriptor( path ) );
	}

	const c8* call_bt( const std::string& path, const std::string& name )
	{
		make_bt( load_image( path ), name );
		return "Binary terrain";
	}

	const c8* call_height( const std::string& path, const std::string& name )
	{
		make_height( load_image( path ), scale_value, name );
		return "Height map";
	}

	const c8* call_md2( const std::string& path, const std::string& name )
	{
		make_md2( descriptor( path ), frame_index, scale_value, name );
		return "MD2 file";
	}

	std::string name_only( const std::string& name )
	{
		up_t i = name.rfind( '/' );
		i = i == std::string::npos ? 0 : i + 1;
		up_t j = name.rfind( '.' );
		j = j == std::string::npos || j < i ? std::string::npos : j - i;
		return name.substr( i, j );
	}

	bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
	{
		const c8* ( * call )( const std::string&, const std::string& ) = 0;

		for ( s32 option; ( option = getopt( argc, argv, "btx:y:z:mf:h" ) ) != -1; )
		{
			switch ( option )
			{
			case 'b':
				call = call_bt;
				break;

			case 't':
				call = call_height;
				break;

			case 'm':
				call = call_md2;
				break;

			case 'x':
				scale_value[ 0 ] = static_cast< f32 >( std::atof( optarg ) );
				break;

			case 'y':
				scale_value[ 1 ] = static_cast< f32 >( std::atof( optarg ) );
				break;

			case 'z':
				scale_value[ 2 ] = static_cast< f32 >( std::atof( optarg ) );
				break;

			case 'f':
				frame_index = std::atoi( optarg );
				break;

			default:
				std::cout <<
					"-b            Convert a grayscale image to binary terrain\n"
					"-t            Convert a height map to a model\n"
					"-m            Convert a MD2 file to a model\n"
					"-x <float>    Floating-point value to scale the x-axis by\n"
					"-y <float>    Floating-point value to scale the y-axis by\n"
					"-z <float>    Floating-point value to scale the z-axis by\n"
					"-f <frame>    Frame of MD2 file to convert\n"
					"-h            This help message\n";

				return false;
			}
		}

		if ( optind == argc )
		{
			std::cout << "Input file unspecified\n";
			return false;
		}
		else if ( !call )
		{
			std::cout << "Tool unspecified\n";
			return false;
		}

		const std::string input = argv[ optind ];
		const std::string output = name_only( input );
		const c8* type = call( input, output );

		std::cerr <<
			"Created model\n"
			"\ttype: " << type << "\n"
			"\tinput: " << input << "\n"
			"\toutput: " << output << "\n";

		return true;
	}
}

namespace ooe
{
	void tools::encode_write( file& file, const void* pointer, up_t size )
	{
		const u8* data = static_cast< const u8* >( pointer );
		c8 buffer[ 16 ];

		for ( up_t i = 0; i != size; ++i )
		{
			up_t length = std::snprintf( buffer, sizeof( buffer ), "\\%u", data[ i ] );
			file.write( buffer, length );
		}
	}
}

//--- main ---------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
