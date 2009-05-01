/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/io/memory.hpp"
#include "foundation/lua/table.hpp"
#include "foundation/lua/vm.hpp"
#include "foundation/scene/effect.hpp"

namespace ooe
{
//--- effect -------------------------------------------------------------------
	effect::effect( const descriptor& desc, const vfs& vfs, const video& video )
		: program( video.program() )
	{
		lua::vm vm;
		lua::stack stack = vm.stack();

		lua::table global( stack, lua::global );
		global[ "vertex" ] = shader::vertex;
		global[ "fragment" ] = shader::fragment;
		vm.load( "effect", desc );

		typedef lua::table::iterator iterator_type;
		lua::table table = global[ "table" ];

		for ( iterator_type i = table.begin(), end = table.end(); i != end; ++i )
		{
			const iterator_type::value_type value( *i );
			scoped_ptr< ooe::shader > shader( video.shader( value.second ) );
			memory memory( vfs[ value.first ] );
			shader->compile( memory.as< c8 >(), memory.size() );
			program->attach( shader );
		}

		program->link();
	}

	program& effect::get( void ) const
	{
		return *program;
	}

//--- variable -----------------------------------------------------------------
	void variable::insert( const uniform_type& uniform, const void* pointer )
	{
		vector.push_back( tuple_type( uniform, pointer ) );
	}

	void variable::apply( void ) const
	{
		for ( vector_type::const_iterator i = vector.begin(), end = vector.end(); i != end; ++i )
			i->_0->load( i->_1 );
	}
}
