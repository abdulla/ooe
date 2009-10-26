/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include <cstdlib>

#include <sys/time.h>
#include <uuid/uuid.h>

#include "foundation/executable/program.hpp"
#include "foundation/executable/timer.hpp"
#include "foundation/utility/atom.hpp"

namespace
{
	using namespace ooe;

	const up_t limit = 1000000;
	atom< u32 > seed( 0 );

	f32 getpid_atom_test( void )
	{
		timer timer;

		for ( up_t i = 0; i != limit; ++i )
		{
			getpid();
			seed++;
		}

		return timer.elapsed();
	}

	f32 time_rand_test( void )
	{
		timer timer;

		for ( up_t i = 0; i != limit; ++i )
		{
			time( 0 );
			rand();
		}

		return timer.elapsed();
	}

	f32 gettimeofday_rand_test( void )
	{
		timer timer;

		for ( up_t i = 0; i != limit; ++i )
		{
			timer::epoch();
			rand();
		}

		return timer.elapsed();
	}

	f32 mkstemp_test( void )
	{
		timer timer;
		char name[] = "/XXXXXX";

		for ( up_t i = 0; i != limit; ++i )
			close( mkstemp( name ) );

		return timer.elapsed();
	}

	f32 tempnam_test( void )
	{
		timer timer;

		for ( up_t i = 0; i != limit; ++i )
			std::free( tempnam( "/", 0 ) );

		return timer.elapsed();
	}

	f32 uuid_generate_test( void )
	{
		timer timer;
		uuid_t uuid;

		for ( up_t i = 0; i != limit; ++i )
			uuid_generate( uuid );

		return timer.elapsed();
	}

	f32 uuid_generate_random_test( void )
	{
		timer timer;
		uuid_t uuid;

		for ( up_t i = 0; i != limit; ++i )
			uuid_generate_random( uuid );

		return timer.elapsed();
	}

	f32 uuid_generate_time_test( void )
	{
		timer timer;
		uuid_t uuid;

		for ( up_t i = 0; i != limit; ++i )
			uuid_generate_time( uuid );

		return timer.elapsed();
	}

	bool launch( const std::string&, const std::string&, s32, c8** )
	{
		std::cout << "Time taken for " << limit << " calls\n";
		std::cout << "getpid_atom_test:          " << getpid_atom_test() << " seconds\n";
		std::cout << "time_rand_test:            " << time_rand_test() << " seconds\n";
		std::cout << "gettimeofday_rand_test:    " << gettimeofday_rand_test() << " seconds\n";
		std::cout << "mkstemp_test:              " << mkstemp_test() << " seconds\n";
		std::cout << "tempnam_test:              " << tempnam_test() << " seconds\n";
		std::cout << "uuid_generate_test:        " << uuid_generate_test() << " seconds\n";
		std::cout << "uuid_generate_random_test: " << uuid_generate_random_test() << " seconds\n";
		std::cout << "uuid_generate_time_test:   " << uuid_generate_time_test() << " seconds\n";
		return true;
	}
}

//--- main ---------------------------------------------------------------------
extern "C" s32 main( s32 argc, c8** argv/*, c8** envp*/ )
{
	return executable::launch( launch, argc, argv );
}
