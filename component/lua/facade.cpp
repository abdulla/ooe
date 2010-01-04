/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/lua/facade.hpp"
#include "component/registry/local.hpp"
#include "component/registry/registry.hpp"

namespace
{
	using namespace ooe;
	typedef std::vector< std::string > find_vector;

	template< s32 ( * function )( lua::state* state ) >
		struct embed
	{
		struct defer
		{
			static s32 call( lua::state* state )
			{
				return function( state );
			}
		};

		static s32 call( lua::state* state )
		{
			return lua::invoke< defer >::call( state );
		}
	};

	s32 find( lua::state* state )
	{
		lua::stack stack = lua::verify_arguments( state, 1 );

		find_vector input;
		lua::to< find_vector >::call( stack, input, 1 );
		interface interface;

		for ( find_vector::const_iterator i = input.begin(), end = input.end(); i != end; ++i )
		{
			up_t j = i->find( '/' );

			if ( j == ~up_t( 0 ) )
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

		typedef shared_ptr< ooe::source > source_ptr;
		source_ptr source = new ooe::source( path );
		const module& module = source->get();
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

		lua::push< source_ptr >::call( stack, source );
		stack.raw_seti( -2, 1 );
		return 1;
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
	void lua::component_setup( stack stack )
	{
		push< const c8* >::call( stack, "ooe" );
		stack.create_table( 0, 3 );

		//--- registry ---------------------------------------------------------
		push< const c8* >::call( stack, "registry" );
		stack.create_table( 0, 2 );

		push< const c8* >::call( stack, "find" );
		stack.push_cclosure( embed< find >::call );
		stack.raw_set( -3 );

		push< const c8* >::call( stack, "load" );
		stack.push_cclosure( embed< load >::call );
		stack.raw_set( -3 );

		stack.raw_set( -3 );

		//----------------------------------------------------------------------

		stack.raw_set( globals_index );
	}

	void lua::throw_exception( state* state, const c8* what, const c8* where )
	{
		std::string string;
		string << what << "\n\nStack trace:" << where;

		stack stack( state );
		push< std::string >::call( stack, string );
		stack.error();
	}
}
