/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "foundation/executable/library.hpp"
#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;

	s32 mode( library::type type )
	{
		switch ( type )
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
			throw error::runtime( "library: " ) << "Unknown type: " << type;
		}
	}

	void* mode( library::find_type type )
	{
		switch ( type )
		{
		case library::all:
			return RTLD_DEFAULT;

		case library::next:
			return RTLD_NEXT;

		default:
			throw error::runtime( "library: " ) << "Unknown find type: " << type;
		}
	}

	void* find( void* id, const std::string& name )
	{
		void* symbol = dlsym( id, name.c_str() );
		const c8* error = dlerror();

		if ( error )
			throw error::runtime( "library: " ) << "Unable to find \"" << name << "\": " << error;
		else if ( !symbol )	// needed due to gcc visibility
			throw error::runtime( "library: " ) << "Null symbol found";

		return symbol;
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

	void* library::find( const std::string& name ) const
	{
		return ::find( id, name );
	}

	void* library::find( const std::string& name, find_type flag )
	{
		return ::find( mode( flag ), name );
	}
}
