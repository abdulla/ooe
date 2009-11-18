/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_LUA_STACK_HPP
#define OOE_COMPONENT_LUA_STACK_HPP

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"

struct lua_State;

namespace ooe
{
	namespace lua
	{
		namespace type
		{
			enum id
			{
				nil,
				boolean,
				lightuserdata,
				number,
				string,
				table,
				function,
				userdata,
				thread
			};
		}

		enum
		{
			registry_index		= -10000,
			environment_index	= -10001,
			globals_index		= -10002
		};

		typedef lua_State state;
		typedef s32 ( * cfunction )( state* );

		class stack;
		inline s32 upvalue( u32 );
	}

//--- lua::stack ---------------------------------------------------------------
	class OOE_VISIBLE lua::stack
	{
	public:
		stack( lua::state* );

		void pop( u32 );

		type::id type( s32 ) const;
		const c8* type_name( type::id ) const;

		f64 to_number( s32 ) const;
		bool to_boolean( s32 ) const;
		const c8* to_lstring( s32, up_t* ) const;
		void* to_userdata( s32 ) const;

		void push_nil( void );
		void push_number( f64 );
		void push_lstring( const c8*, up_t );
		void push_cclosure( cfunction, u32 );
		void push_boolean( bool );
		void push_lightuserdata( void* );

		void raw_get( s32 );
		void raw_geti( s32, u32 );
		void create_table( u32, u32 );

		void raw_set( s32 );
		void raw_seti( s32, u32 );

	private:
		lua::state* state;
	};

//--- lua ----------------------------------------------------------------------
	inline s32 lua::upvalue( u32 index )
	{
		return globals_index - index;
	}
}

#endif	// OOE_COMPONENT_LUA_STACK_HPP