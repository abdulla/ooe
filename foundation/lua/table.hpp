/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_LUA_TABLE_HPP
#define OOE_FOUNDATION_LUA_TABLE_HPP

#include "foundation/lua/traits.hpp"

namespace std
{
	struct forward_iterator_tag;
}

namespace ooe
{
	namespace lua
	{
		class table;
		class key;
	}

//--- lua::table ---------------------------------------------------------------
	class OOE_VISIBLE lua::table
	{
	private:
		template< typename >
			class key;

		mutable lua::stack stack;
		s32 ref;

	public:
		class iterator;

		table( const lua::stack& );
		table( const lua::stack&, s32 );
		~table( void );

		table( const table& );
		table& operator =( const table& );

		iterator begin( void );
		iterator end( void );
		up_t size( void ) const;
		void push( void ) const;

		key< std::string > operator []( const std::string& ) const;
		key< u32 > operator []( u32 ) const;	// remember, lua indices start at 1
	};

	namespace lua
	{
		template<>
			struct is_specialised< table >;

		template< typename type >
			struct traits< type, typename enable_if< is_like< type, table > >::type >;

		template<>
			class table::key< std::string >;

		template<>
			class table::key< u32 >;
	}

//--- is_specialised -----------------------------------------------------------
	template<>
		struct lua::is_specialised< lua::table >
		: public true_type
	{
	};

//--- lua::traits< type, table > -----------------------------------------------
	template< typename type >
		struct lua::traits< type, typename enable_if< is_like< type, lua::table > >::type >
	{
		static void push( stack&, const table& table )
		{
			table.push();
		}

		static table to( stack& stack, s32 index )
		{
			type_check< table >( stack, index, id::table );
			return table( stack, index );
		}
	};

//--- lua::table::key< std::string > -------------------------------------------
	template<>
		class OOE_VISIBLE lua::table::key< std::string >
	{
	public:
		template< typename t >
			key& operator =( const t& value )
		{
			table.stack.rawgeti( registry, table.ref );
			traits< const std::string& >::push( table.stack, string );
			traits< t >::push( table.stack, value );
			table.stack.rawset( -3 );
			return *this;
		}

		template< typename t >
			operator t( void ) const
		{
			table.stack.rawgeti( registry, table.ref );
			traits< const std::string& >::push( table.stack, string );
			table.stack.rawget( -2 );
			t value = traits< t >::to( table.stack, -1 );
			table.stack.pop( 1 );
			return value;
		}

		id::type type( void ) const;

	private:
		const lua::table& table;
		const std::string string;

		key( const lua::table&, const std::string& ) OOE_HIDDEN;

		friend class lua::table;
	};

//--- lua::table::key< u32 > ---------------------------------------------------
	template<>
		class OOE_VISIBLE lua::table::key< u32 >
	{
	public:
		template< typename t >
			key& operator =( const t& value )
		{
			table.stack.rawgeti( registry, table.ref );
			traits< t >::push( table.stack, value );
			table.stack.rawseti( -2, integer );
			return *this;
		}

		template< typename t >
			operator t( void ) const
		{
			table.stack.rawgeti( registry, table.ref );
			table.stack.rawgeti( -1, integer );
			t value = traits< t >::to( table.stack, -1 );
			table.stack.pop( 1 );
			return value;
		}

		id::type type( void ) const;

	private:
		const lua::table& table;
		const u32 integer;

		key( const lua::table&, u32 ) OOE_HIDDEN;

		friend class lua::table;
	};

//--- lua::key -----------------------------------------------------------------
	class OOE_VISIBLE lua::key
	{
	public:
		key( lua::stack&, s32 );

		template< typename t >
			operator t( void ) const
		{
			return traits< t >::to( stack, index );
		}

		id::type type( void ) const;

	private:
		lua::stack& stack;
		const s32 index;
	};

//--- lua::table::iterator -----------------------------------------------------
	class OOE_VISIBLE lua::table::iterator
	{
	public:
		typedef std::forward_iterator_tag iterator_category;
		typedef std::pair< lua::key, lua::key > value_type;

		iterator( lua::table* );

		iterator& operator ++( void );
		value_type operator *( void ) const;

		bool operator !=( const iterator& ) const;

	private:
		lua::table* table;
	};
}

#endif	// OOE_FOUNDATION_LUA_TABLE_HPP
