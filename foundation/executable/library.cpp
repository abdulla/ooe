/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/library.hpp"
#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;

	s32 mode( library::type flag )
	{
		switch ( flag )
		{
		case library::local_lazy:
			return RTLD_LOCAL | RTLD_LAZY;

		case library::local_now:
			return RTLD_LOCAL | RTLD_NOW;

		case library::global_lazy:
			return RTLD_GLOBAL | RTLD_LAZY;

		case library::global_now:
			return RTLD_GLOBAL | RTLD_NOW;

		default:
			throw error::runtime( "library: " ) << "Unknown type: " << flag;
		}
	}

	void* mode( library::find_type flag )
	{
		switch ( flag )
		{
		case library::all:
			return RTLD_DEFAULT;

		case library::next:
			return RTLD_NEXT;

		default:
			throw error::runtime( "library: " ) << "Unknown find type: " << flag;
		}
	}
}

namespace ooe
{
//--- library ------------------------------------------------------------------
	library::library( const std::string& name, type flag )
		: id( dlopen( name.c_str(), mode( flag ) ) )
	{
		if ( !id )
			throw error::runtime( "library: " ) << "Unable to open \"" << name << "\": " <<
				dlerror();
	}

	library::~library( void )
	{
		if ( dlclose( id ) )
			OOE_WARNING( "library", "Unable to close: " << dlerror() );
	}

	ooe::symbol<> library::find( const std::string& name ) const
	{
		ooe::symbol<> symbol( dlsym( id, name.c_str() ) );
		const c8* error = dlerror();

		if ( error )
			throw error::runtime( "library: " ) << "Unable to find \"" << name << "\": " << error;
		else if ( !symbol.function )	// needed due to gcc visibility
			throw error::runtime( "library: " ) << "Null symbol found";

		return symbol;
	}

	ooe::symbol<> library::find( const std::string& name, find_type flag )
	{
		ooe::symbol<> symbol( dlsym( mode( flag ), name.c_str() ) );
		const c8* error = dlerror();

		if ( error )
			throw error::runtime( "library: " ) << "Unable to find \"" << name << "\": " << error;
		else if ( !symbol.function )	// needed due to gcc visibility
			throw error::runtime( "library: " ) << "Null symbol found";

		return symbol;
	}
}
