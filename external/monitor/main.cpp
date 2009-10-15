/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/program.hpp"
#include "foundation/general/sight.hpp"
#include "foundation/image/jpeg.hpp"
#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/miscellany.hpp"

#ifdef __APPLE__
	#include "foundation/general/event_queue.hpp"
#else
	#include "foundation/executable/timer.hpp"
#endif

namespace
{
	using namespace ooe;

	class detect
		: private noncopyable
	{
	public:
		detect( const std::string& directory_, up_t percent_, u16 width, u16 height )
			: directory( directory_ ), percent( percent_ ), count( 0 ),
			image( width, height, sight::format() )
		{
		}

		void process( const u8* frame )
		{
			u8* pointer = image.as< u8 >();
			up_t byte_size = image.byte_size();
			up_t hit = 0;

			for ( up_t i = 0; i != byte_size; ++i )
			{
				if ( std::abs( frame[ i ] - pointer[ i ] ) > 32 )
					++hit;
			}

			std::memcpy( pointer, frame, byte_size );

			if ( hit > byte_size * percent / 100 )
				write();
		}

	private:
		const std::string directory;
		const up_t percent;
		up_t count;
		uncompressed_image image;

		void write( void )
		{
			std::time_t now = std::time( 0 );
			c8 in[ 32 ];

			if ( !strftime( in, sizeof( in ), "%F %T", localtime( &now ) ) )
			{
				OOE_WARNING( "detect",  "Unable to convert time to string" );
				return;
			}

			std::string name;
			name << directory << "/frame-(" << in << ")-" << ++count << ".jpg";
			jpeg::encode( image, descriptor( name, descriptor::write_new ) );
			std::cout << "Wrote image to '" << name << "'\n";
		}
	};

	bool launch( const std::string&, const std::string&, s32 argc, c8** argv )
	{
		std::string directory = ".";
		up_t percent = 5;
		u16 width = 320;
		u16 height = 240;

		for ( s32 option; ( option = getopt( argc, argv, "d:h:p:w:" ) ) != -1; )
		{
			switch ( option )
			{
			case 'd':
				directory = optarg;
				break;

			case 'h':
				height = static_cast< u16 >( std::strtoul( optarg, 0, 10 ) );
				break;

			case 'p':
				percent = clamp< up_t >( std::strtoul( optarg, 0, 10 ), 1, 99 );
				break;

			case 'w':
				width = static_cast< u16 >( std::strtoul( optarg, 0, 10 ) );
				break;

			default:
				std::cout <<
					"    -d <path>     Directory to write images to        [default .]\n"
					"    -p <percent>  Percentage (1-99) change to detect  [default 5]\n"
					"    -w <width>    Camera width                        [default 320]\n"
					"    -h <height>   Camera height                       [default 240]\n";

				return false;
			}
		}

		detect detect( directory, percent, width, height );
		sight sight( make_function( detect, &detect::process ), width, height );

#ifdef __APPLE__
		event_queue queue;
		event event;
		while ( queue.next_event( event ) ) {}
#else
		epoch_t snooze( 1000000, 0 );
		while ( true ) timer::sleep( snooze );
#endif

		return true;
	}
}

//--- main ---------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
