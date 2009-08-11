/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <cmath>

#include "test/unit/assert.hpp"
#include "test/unit/error.hpp"

namespace ooe
{
	void unit::assert( const c8* message, bool condition )
	{
		if ( !condition )
			throw error::assertion() << message;
	}

	void unit::assert( const std::string& message, bool condition )
	{
		assert( message.c_str(), condition );
	}

	void unit::expect( const c8* message, bool condition )
	{
		if ( !condition )
			std::cerr << message << '\n';
	}

	void unit::expect( const std::string& message, bool condition )
	{
		expect( message.c_str(), condition );
	}

	void unit::fail( const c8* message )
	{
		assert( message, false );
	}

	void unit::fail( const std::string& message )
	{
		fail( message.c_str() );
	}
}
