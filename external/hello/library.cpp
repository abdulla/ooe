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
}

extern "C" ooe::module OOE_VISIBLE module_open( void )
{
	module module;

	builder< facade::remote, facade::lua > builder( module );
	builder.insert( "hello", hello );
	builder.insert( "allocate", allocate );

	return module;
}
