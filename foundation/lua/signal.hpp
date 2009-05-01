/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_LUA_SIGNAL_HPP
#define OOE_FOUNDATION_LUA_SIGNAL_HPP

#include "foundation/lua/traits.hpp"
#include "foundation/utility/signal.hpp"

namespace ooe
{
	namespace lua
	{
		template< typename type >
			class slot;

		template< typename type >
			class signal;

		template< typename type >
			struct is_specialised< ooe::signal< type > >;

		template< typename type >
			struct traits< ooe::signal< type >, void >;
	}

//--- lua::slot ----------------------------------------------------------------
	template< typename type >
		class lua::slot
	{
	public:
		slot( stack& stack, s32 index )
			: function( stack, index ),
			slot_native( make_function( function, &lua::function< type >::operator () ) )
		{
		}

		slot_base& get( void )
		{
			return slot_native;
		}

	private:
		lua::function< type > function;
		ooe::slot< type > slot_native;
	};

//--- lua::signal --------------------------------------------------------------
	template< typename type >
		struct lua::signal
	{
	public:
		static s32 push_back( lua::state* state )
		{
			stack stack( state );
			type_check( stack, 1, id::userdata );
			type_check( stack, 2, id::userdata );

			signal_type& signal = *native< signal_type* >( stack, 1 );
			slot_type& slot = native< slot_type >( stack, 2 );
			signal.push_back( slot.get() );
			return 0;
		}

		static s32 make_slot( lua::state* state )
		{
			stack stack( state );
			type_check( stack, 1, id::function );
			new( stack.newuserdata( sizeof( slot_type ) ) ) slot_type( stack, 1 );

			if ( stack.newmetatable( typeid( slot_type ).name() ) )
			{
				// metatable.__gc = slot_type::~slot_type
				traits< const c8* >::push( stack, "__gc" );
				traits< cfunction >::push( stack, slot_gc );
				stack.rawset( -3 );
			}

			stack.setmetatable( -2 );
			return 1;
		}

	private:
		typedef function< type > function_type;
		typedef slot< type > slot_type;
		typedef ooe::signal< type > signal_type;

		static s32 slot_gc( lua::state* state )
		{
			stack stack( state );
			native< slot_type >( stack, 1 ).~slot_type();
			return 0;
		}
	};

//--- lua::is_specialised ------------------------------------------------------
	template< typename type >
		struct lua::is_specialised< ooe::signal< type > >
		: public true_type
	{
	};

//--- lua::traits< type, ooe::signal > -----------------------------------------
	template< typename type >
		struct lua::traits< ooe::signal< type >, void >
	{
		typedef ooe::signal< type > signal_type;

		static void push( stack& stack, const signal_type& signal )
		{
			new( stack.newuserdata( sizeof( signal_type* ) ) ) const signal_type*( &signal );

			if ( stack.newmetatable( typeid( signal_type ).name() ) )
			{
				typedef lua::signal< type > lua_signal;

				// metatable.__index = metatable
				traits< const c8* >::push( stack, "__index" );
				stack.pushvalue( -2 );
				stack.rawset( -3 );

				// metatable.push_back = push_back
				traits< const c8* >::push( stack, "push_back" );
				traits< cfunction >::push( stack, lua_signal::push_back );
				stack.rawset( -3 );

				// metatable.make_slot = make_slot
				traits< const c8* >::push( stack, "make_slot" );
				traits< cfunction >::push( stack, lua_signal::make_slot );
				stack.rawset( -3 );
			}

			stack.setmetatable( -2 );
		}
	};
}

#endif	// OOE_FOUNDATION_LUA_SIGNAL_HPP
