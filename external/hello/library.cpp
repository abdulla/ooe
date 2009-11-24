/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "component/lua/facade.hpp"
#include "component/registry/builder.hpp"
#include "component/registry/local.hpp"
#include "component/registry/remote.hpp"

namespace
{
	using namespace ooe;

	class print
	{
	public:
		print( const std::string& value_ )
			: value( value_ )
		{
		}

		~print( void )
		{
			std::cout << value << '\n';
		}

		void say( void )
		{
			std::cout << this << " said: " << value << '\n';
		}

	private:
		std::string value;
	};

	void hello( void )
	{
		std::cout << "hello library\n";
	}

	construct_ptr< print > allocate( const std::string& value )
	{
		return new print( value );
	}

	void deallocate( destruct_ptr< print > value )
	{
		delete value;
	}

	tuple< bool, f32 > lua_gauntlet( bool b, f32 f )
	{
		return make_tuple( b, f );
	}
}

extern "C" ooe::module OOE_VISIBLE module_open( void )
{
	module module;

	builder< facade::remote, facade::lua > builder( module );
	builder.insert( "hello", hello );
	builder.insert( "allocate", allocate );
	builder.insert( "deallocate", deallocate );
	builder.insert( "say", &print::say );
	builder.insert( "lua_gauntlet", lua_gauntlet );

	return module;
}
