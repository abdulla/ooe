/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_LUA_STACK_HPP
#define OOE_FOUNDATION_LUA_STACK_HPP

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"

struct lua_State;

namespace ooe
{
	namespace lua
	{
		namespace id
		{
			enum type
			{
				none			= -1,
				nil				= 0,
				boolean			= 1,
				lightuserdata	= 2,
				number			= 3,
				string			= 4,
				table			= 5,
				function		= 6,
				userdata		= 7,
				thread			= 8
			};
		}

		enum
		{
			registry	= -10000,
			environment	= -10001,
			global		= -10002,

			ref_none	= -2,
			ref_nil		= -1
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

		u32 gettop( void );
		void pop( u32 );
		void pushvalue( s32 );
		void remove( s32 );
		void insert( s32 );
		void replace( s32 );

		id::type type( s32 ) const;
		const c8* type_name( id::type ) const;

		f32 tonumber( s32 ) const;
		bool toboolean( s32 ) const;
		const c8* tolstring( s32, up_t* ) const;
		up_t objlen( s32 ) const;
		void* touserdata( s32 ) const;

		void pushnil( void );
		void pushnumber( f32 );
		void pushlstring( const c8*, up_t );
		void pushcclosure( cfunction, u32 );
		void pushboolean( bool );
		void pushlightuserdata( void* );

		void rawget( s32 );
		void rawgeti( s32, u32 );
		void createtable( u32, u32 );
		void* newuserdata( up_t );
		bool getmetatable( s32 );

		void rawset( s32 );
		void rawseti( s32, u32 );
		void setmetatable( s32 );

		void call( u32, u32 = -1u );
		void pcall( u32, u32 = -1u );
		void error( void );
		bool next( s32 );
		void concat( u32 );

		bool newmetatable( const c8* );
		void getmetatable( const c8* );
		void where( u32 );
		s32 ref( void );
		void unref( s32 );

	private:
		lua::state* state;
	};

//--- lua::upvalue -------------------------------------------------------------
	inline s32 lua::upvalue( u32 index )
	{
		return global - index;
	}
}

#endif	// OOE_FOUNDATION_LUA_STACK_HPP
