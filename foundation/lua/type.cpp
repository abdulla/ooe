/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/lua/type.hpp"

namespace
{
	using namespace ooe;
	using namespace lua;

	s32 dispatch_get( lua::state* state )
	{
		stack stack( state );
		stack.pushvalue( 1 );	// stack[ -1 ] = userdata

		// loop through metatables: userdata.metatable, userdata.metatable.metatable, etc
		while ( true )
		{
			// getmetatable( -1 )
			if ( !stack.getmetatable( -1 ) )
				break;

			stack.remove( -2 );	// got metatable, so remove metatable source

			// argument is not string, so use index overload
			if ( stack.type( 2 ) != id::string )
			{
				lua::traits< const c8* >::push( stack, "__array" );
				stack.rawget( -2 );

				if ( stack.type( -1 ) != id::function )
				{
					stack.pop( 1 );
					continue;
				}

				// function( userdata, value )
				stack.insert( 1 );
				stack.pop( 1 );
				stack.call( 2 );
				return 1;
			}

			stack.pushvalue( 2 );
			stack.rawget( -2 );

			id::type type = stack.type( -1 );

			if ( type == id::nil )
			{
				stack.pop( 1 );
				continue;
			}
			else if ( type == id::function )	// type( key ) == function
				return 1;
			else if ( type != id::table )		// type( key ) != table
				return 1;

			// key.__index
			lua::traits< const c8* >::push( stack, "__index" );
			stack.rawget( -2 );

			if ( stack.type( -1 ) != id::function )	// type( __index ) != function
			{
				stack.pop( 2 );
				continue;
			}

			// function( userdata )
			stack.insert( 1 );
			stack.pop( 3 );
			stack.call( 1 );
			return 1;
		}

		// raise error?
		return 0;
	}

	s32 dispatch_set( lua::state* state )
	{
		stack stack( state );
		stack.pushvalue( 1 );	// stack[ -1 ] = userdata

		// loop through metatables: userdata.metatable, userdata.metatable.metatable, etc
		while ( true )
		{
			// getmetatable( userdata )
			if ( !stack.getmetatable( -1 ) )
				break;

			stack.remove( -2 );

			// argument is not string, so use index overload
			if ( stack.type( 2 ) != id::string )
			{
				lua::traits< const c8* >::push( stack, "__newarray" );
				stack.rawget( -2 );

				if ( stack.type( -1 ) != id::function )
				{
					stack.pop( 1 );
					continue;
				}

				// function( userdata, key, value )
				stack.insert( 1 );
				stack.pop( 1 );
				stack.call( 3 );
				return 0;
			}

			stack.pushvalue( 2 );
			stack.rawget( -2 );

			if ( stack.type( -1 ) != id::table )	// type( key ) != table
			{
				stack.pop( 1 );
				continue;
			}

			// key.__newindex
			lua::traits< const c8* >::push( stack, "__newindex" );
			stack.rawget( -2 );

			if ( stack.type( -1 ) != id::function )	// type( __newindex ) != function
			{
				stack.pop( 2 );
				continue;
			}

			// function( userdata, value )
			stack.insert( 1 );
			stack.pop( 2 );
			stack.remove( -2 );
			stack.call( 2 );
			return 0;
		}

		// raise error?
		return 0;
	}
}

namespace ooe
{
//--- lua ----------------------------------------------------------------------
	lua::type_base::type_base( const lua::stack& stack_, const c8* name )
		: stack( stack_ ), ref()
	{
		if ( stack.newmetatable( name ) )
		{
			// metatable.__index = dispatch_get
			lua::traits< const c8* >::push( stack, "__index" );
			lua::traits< cfunction >::push( stack, dispatch_get );
			stack.rawset( -3 );

			// metatable.__newindex = dispatch_set
			lua::traits< const c8* >::push( stack, "__newindex" );
			lua::traits< cfunction >::push( stack, dispatch_set );
			stack.rawset( -3 );
		}

		// registry[ ref ] = metatable
		const_cast< s32& >( ref ) = stack.ref();
	}

	lua::type_base::~type_base( void )
	{
		stack.unref( ref );
	}
}
