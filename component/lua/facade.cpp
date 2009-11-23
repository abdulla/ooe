/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "component/lua/facade.hpp"
#include "component/registry/local.hpp"
#include "component/registry/registry.hpp"

namespace
{
	using namespace ooe;

	class source
	{
	public:
		source( const std::string& path )
			: library( path ), module( library.find< ooe::module ( void ) >( "module_open" )() )
		{
		}

		const ooe::module& get( void ) const
		{
			return module;
		}

	private:
		ooe::library library;
		ooe::module module;
	};

	typedef std::vector< std::string > find_vector;

	s32 find( lua::state* state )
	{
		lua::stack stack = lua::verify_arguments( state, 1 );

		find_vector input;
		lua::to< find_vector >::call( stack, input, 1 );
		interface interface;

		for ( find_vector::const_iterator i = input.begin(), end = input.end(); i != end; ++i )
		{
			up_t j = i->find( '/' );

			if ( j == up_t( -1 ) )
				throw error::runtime( "lua::find: " ) << "Invalid function \"" << *i << '\"';

			interface.insert( i->substr( 0, j ), i->substr( j + 1 ) );
		}

		registry registry;
		registry::info_vector v = registry.find( interface );
		find_vector output;

		for ( registry::info_vector::const_iterator i = v.begin(), end = v.end(); i != end; ++i )
		{
			if ( i->_0 == registry::library )
				output.push_back( i->_1 );
		}

		lua::push< find_vector >::call( stack, output );
		return 1;
	}

	s32 load( lua::state* state )
	{
		lua::stack stack = lua::verify_arguments( state, 1 );

		std::string path;
		lua::to< std::string >::call( stack, path, 1 );

		shared_ptr< source > source_ptr = new source( path );
		const module& module = source_ptr->get();
		const module::vector_type& names = module.get();
		const facade::local::vector_type& local = static_cast< const facade::local* >
			( module.find( typeid( facade::local ).name() ) )->get();
		const facade::lua::vector_type& lua = static_cast< const facade::lua* >
			( module.find( typeid( facade::lua ).name() ) )->get();

		stack.create_table();

		for ( up_t i = 0, end = names.size(); i != end; ++i )
		{
			lua::push< std::string >::call( stack, names[ i ]._0 + '/' + names[ i ]._1 );

			if ( names[ i ]._1[ 0 ] == 'F' )
				lua::push< void ( * )( void ) >::call( stack, local[ i ].function );
			else if ( names[ i ]._1[ 0 ] == 'M' )
				lua::push< void ( any::* )( void ) >::call( stack, local[ i ].member );
			else
			{
				stack.pop( 2 );
				throw error::runtime( "lua::load: " ) << "Function \"" << names[ i ]._0 <<
					"\" of type \"" << names[ i ]._1 << "\" has unknown pointer";
			}

			stack.push_cclosure( lua[ i ], 1 );
			stack.raw_set( -3 );
		}

		lua::push< shared_ptr< source > >::call( stack, source_ptr );

		return 2;
	}
}

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

//--- lua ----------------------------------------------------------------------
	void lua::setup( stack& stack, const std::string& path )
	{
		push< const c8* >::call( stack, "ooe" );
		stack.create_table( 0, 3 );

		//--- executable -------------------------------------------------------
		push< const c8* >::call( stack, "executable" );
		stack.create_table( 0, 1 );

		push< const c8* >::call( stack, "path" );
		push< std::string >::call( stack, path );
		stack.raw_set( -3 );

		stack.raw_set( -3 );

		//--- library ----------------------------------------------------------
		push< const c8* >::call( stack, "library" );
		stack.create_table( 0, 1 );

		push< const c8* >::call( stack, "suffix" );
		push< const c8* >::call( stack, library::suffix );
		stack.raw_set( -3 );

		stack.raw_set( -3 );

		//--- registry ---------------------------------------------------------
		push< const c8* >::call( stack, "registry" );
		stack.create_table( 0, 2 );

		push< const c8* >::call( stack, "find" );
		stack.push_cclosure( find );
		stack.raw_set( -3 );

		push< const c8* >::call( stack, "load" );
		stack.push_cclosure( load );
		stack.raw_set( -3 );

		stack.raw_set( -3 );

		//----------------------------------------------------------------------

		stack.raw_set( globals_index );
	}
}
