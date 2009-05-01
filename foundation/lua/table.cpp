/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/lua/error.hpp"
#include "foundation/lua/table.hpp"

namespace ooe
{
//--- lua::table ---------------------------------------------------------------
	lua::table::table( const lua::stack& stack_ )
		: stack( stack_ ), ref()
	{
		stack.createtable( 0, 0 );
		ref = stack.ref();
	}

	lua::table::table( const lua::stack& stack_, s32 index )
		: stack( stack_ ), ref()
	{
		if ( stack.type( index ) != id::table )
			throw error::lua( "table: " ) << "Value at index is not a table";

		stack.pushvalue( index );
		ref = stack.ref();
	}

	lua::table::~table( void )
	{
		stack.unref( ref );
	}

	lua::table::table( const table& copy )
		: stack( copy.stack ), ref()
	{
		stack.rawgeti( registry, copy.ref );
		ref = stack.ref();
	}

	lua::table& lua::table::operator =( const table& copy )
	{
		stack = copy.stack;
		stack.rawgeti( registry, copy.ref );

		s32 old_ref = ref;
		ref = stack.ref();
		stack.unref( old_ref );
		return *this;
	}

	lua::table::iterator lua::table::begin( void )
	{
		return iterator( this );
	}

	lua::table::iterator lua::table::end( void )
	{
		return iterator( 0 );
	}

	up_t lua::table::size( void ) const
	{
		stack.rawgeti( registry, ref );
		up_t value = stack.objlen( -1 );
		stack.pop( 1 );
		return value;
	}

	void lua::table::push( void ) const
	{
		stack.rawgeti( registry, ref );
	}

	lua::table::key< std::string > lua::table::operator []( const std::string& string ) const
	{
		return key< std::string >( *this, string );
	}

	lua::table::key< u32 > lua::table::operator []( u32 integer ) const
	{
		if ( !integer )
			throw error::lua( "table: " ) << "Indices start at 1";

		return key< u32 >( *this, integer );
	}

//--- lua::table::key< std::string > -------------------------------------------
	lua::table::key< std::string >::key( const lua::table& table_, const std::string& string_ )
		: table( table_ ), string( string_ )
	{
	}

	lua::id::type lua::table::key< std::string >::type( void ) const
	{
		table.stack.rawgeti( registry, table.ref );
		traits< const std::string& >::push( table.stack, string );
		table.stack.rawget( -2 );
		id::type id = table.stack.type( -1 );
		table.stack.pop( 1 );
		return id;
	}

//--- lua::table::key< u32 > ---------------------------------------------------
	lua::table::key< u32 >::key( const lua::table& table_, u32 integer_ )
		: table( table_ ), integer( integer_ )
	{
	}

	lua::id::type lua::table::key< u32 >::type( void ) const
	{
		table.stack.rawgeti( registry, table.ref );
		table.stack.rawgeti( -1, integer );
		id::type id = table.stack.type( -1 );
		table.stack.pop( 1 );
		return id;
	}

//--- lua::key -----------------------------------------------------------------
	lua::key::key( lua::stack& stack_, s32 index_ )
		: stack( stack_ ), index( index_ )
	{
	}

	lua::id::type lua::key::type( void ) const
	{
		return stack.type( index );
	}

//--- lua::table::iterator -----------------------------------------------------
	lua::table::iterator::iterator( lua::table* table_ )
		: table( table_ )
	{
		if ( !table )
			return;

		lua::stack& stack = table->stack;
		stack.rawgeti( registry, table->ref );
		stack.pushnil();
		stack.pushnil();
		++*this;
	}

	lua::table::iterator& lua::table::iterator::operator ++( void )
	{
		lua::stack& stack = table->stack;
		stack.pop( 1 );

		if ( stack.next( -2 ) )
			return *this;

		stack.pop( 1 );
		table = 0;
		return *this;
	}

	lua::table::iterator::value_type lua::table::iterator::operator *( void ) const
	{
		lua::stack& stack = table->stack;
		return value_type( lua::key( stack, -2 ), lua::key( stack, -1 ) );
	}

	bool lua::table::iterator::operator !=( const iterator& equal ) const
	{
		return table != equal.table;
	}
}
