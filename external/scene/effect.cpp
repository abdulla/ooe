/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <iostream>

#include "external/scene/effect.hpp"
#include "foundation/io/memory.hpp"

namespace ooe
{
//--- effect -------------------------------------------------------------------
	effect::effect( const descriptor& desc, const vfs& vfs, const video& video )
		: program( video.program() )
	{
		std::string source = memory( desc ).as< c8 >();

		for ( up_t i = 0, j = 0; ( j = source.find( '\n', i ) ) != std::string::npos; i = j + 1 )
		{
			if ( source[ i ] == '#' || source[ i + 1 ] == '\n' )
				continue;

			std::string line = source.substr( i, j - i );
			up_t k = line.rfind( '.' );

			if ( k == std::string::npos )
				continue;

			shader::type type = line.substr( k + 1 ) == "vs" ? shader::vertex : shader::fragment;
			scoped_ptr< ooe::shader > shader( video.shader( type ) );
			memory memory( vfs[ line ] );
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
	void variable::insert( const uniform_ptr& uniform, const void* pointer )
	{
		vector.push_back( tuple_type( uniform, pointer ) );
	}

	void variable::apply( void ) const
	{
		for ( vector_type::const_iterator i = vector.begin(), end = vector.end(); i != end; ++i )
			i->_0->load( i->_1 );
	}
}
