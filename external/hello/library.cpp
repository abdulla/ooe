/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "component/javascript/facade.hpp"
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

	tuple< bool, f32 > gauntlet( bool b, f32 f )
	{
		return make_tuple( b, f );
	}

	bool* mismatch( void )
	{
		return 0;
	}

	typedef std::map< std::string, up_t > map_type;
	map_type stdmap( const map_type& map )
	{
		for ( map_type::const_iterator i = map.begin(), end = map.end(); i != end; ++i )
			std::cout << "key: " << i->first << ", value: " << i->second << '\n';

		return map;
	}
}

extern "C" ooe::module OOE_VISIBLE module_open( void )
{
	module module;

	builder< facade::remote, facade::lua, facade::javascript > builder( module );
	builder.insert( "hello", hello );
	builder.insert( "allocate", allocate );
	builder.insert( "deallocate", deallocate );
	builder.insert( "say", &print::say );
	builder.insert( "gauntlet", gauntlet );
	builder.insert( "mismatch", mismatch );
	builder.insert( "stdmap", stdmap );

	return module;
}
