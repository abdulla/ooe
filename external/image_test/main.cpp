/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/program.hpp"
#include "foundation/image/dds.hpp"
#include "foundation/image/jpeg.hpp"
#include "foundation/image/jpeg2000.hpp"
#include "foundation/image/png.hpp"

namespace
{
	using namespace ooe;

	bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
	{
		const c8* dds = 0;
		const c8* jpeg = 0;
		const c8* png = 0;

		for ( s32 option; ( option = getopt( argc, argv, "d:j:p:" ) ) != -1; )
		{
			switch ( option )
			{
			case 'd':
				dds = optarg;
				break;

			case 'j':
				jpeg = optarg;
				break;

			case 'p':
				png = optarg;
				break;

			default:
				std::cout <<
					"    -d <file>    DDS file to use\n"
					"    -j <file>    JPEG file to use\n"
					"    -p <file>    PNG file to use\n";
				return false;
			}
		}

		if ( !dds || !jpeg || !png )
		{
			std::cout << "Please specify a DDS, JPEG, PNG file using -d, -j, -p respectively\n";
			return false;
		}

		uncompressed_image a = png::decode( descriptor( png ) );
		compressed_image b = dxt::encode( a, compressed_image::rgba_dxt5 );
		png::encode( dxt::decode( b ), descriptor( "test-1.png", descriptor::write_new ) );

		dds::encode( b, descriptor( "test-2.dds", descriptor::write_new ) );
		compressed_image c = dds::decode( descriptor( "test-2.dds" ) );
		png::encode( dxt::decode( c ), descriptor( "test-3.png", descriptor::write_new ) );

		compressed_image d = dds::decode( descriptor( dds ) );
		png::encode( dxt::decode( d ), descriptor( "test-4.png", descriptor::write_new ) );

		uncompressed_image e = jpeg::decode( descriptor( jpeg ) );
		jpeg2000::encode( e, descriptor( "test-5.jp2", descriptor::write_new ) );
		return true;
	}
}

//--- main ---------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
