/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "component/lua/script.hpp"
#include "component/registry/local.hpp"

namespace ooe
{
//--- facade::lua --------------------------------------------------------------
	const facade::lua::vector_type& facade::lua::get( void ) const
	{
		return vector;
	}

	void facade::lua::insert( up_t index, ooe::lua::cfunction call )
	{
		vector_type::iterator i = vector.begin();
		std::advance( i, index );
		vector.insert( i, call );
	}

//--- lua::script --------------------------------------------------------------
	lua::script::script( const std::string& path, stack& stack )
		: library( path ), module( library.find< ooe::module ( void ) >( "module_open" )() )
	{
		const module::vector_type& names = module.get();
		const facade::local::vector_type& local =
			static_cast< const facade::local* >( module.find( "local" ) )->get();
		const facade::lua::vector_type& lua =
			static_cast< const facade::lua* >( module.find( "lua" ) )->get();

		stack.create_table();

		for ( up_t i = 0, end = names.size(); i != end; ++i )
		{
			push< std::string >::call( stack, names[ i ]._0 + '/' + names[ i ]._1 );

			if ( names[ i ]._1[ 0 ] == 'P' )
				push< void ( * )( void ) >::call( stack, local[ i ].function );
			else if ( names[ i ]._1[ 0 ] == 'M' )
				push< void ( any::* )( void ) >::call( stack, local[ i ].member );
			else
			{
				stack.pop( 2 );
				throw error::runtime( "lua:script: " ) << "Function \"" << names[ i ]._0 <<
					"\" of type \"" << names[ i ]._1 << "\" has unknown pointer";
			}

			stack.push_cclosure( lua[ i ], 1 );
			stack.raw_set( -3 );
		}
	}
}
