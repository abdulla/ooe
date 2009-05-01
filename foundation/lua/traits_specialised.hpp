/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_LUA_TRAITS_SPECIALISED_HPP
#define OOE_FOUNDATION_LUA_TRAITS_SPECIALISED_HPP

namespace ooe
{
	namespace lua
	{
		struct nil {};
		struct stack_index;
		struct binary_string;
		struct pointer_base;

		template< typename >
			class pointer;

		template< typename >
			struct release;

		template< typename >
			struct acquire;

		template<>
			struct is_specialised< nil >;

		template<>
			struct is_specialised< stack_index >;

		template<>
			struct is_specialised< binary_string >;

		template< typename type >
			struct is_specialised< pointer< type > >;

		template< typename type >
			struct is_specialised< release< type > >;

		template< typename type >
			struct is_specialised< acquire< type > >;

		template< typename type >
			struct traits< type, typename enable_if< is_like< type, nil > >::type >;

		template< typename type >
			struct traits< type, typename enable_if< is_like< type, stack_index > >::type >;

		template< typename type >
			struct traits< type, typename enable_if< is_like< type, binary_string > >::type >;

		template< typename type >
			struct traits< pointer< type >, typename enable_if< is_object< type > >::type >;

		template< typename type >
			struct traits< release< type >, void >;

		template< typename type >
			struct traits< acquire< type >, void >;
	}

//--- lua::stack_index ---------------------------------------------------------
	struct lua::stack_index
	{
		const s32 value;

		stack_index( s32 value_ )
			: value( value_ )
		{
		}
	};

//--- lua::binary_string -------------------------------------------------------
	struct lua::binary_string
	{
		const void* string;
		const up_t size;

		binary_string( const void* string_, up_t size_ )
			: string( string_ ), size( size_ )
		{
		}
	};

//--- lua::pointer_base --------------------------------------------------------
	struct lua::pointer_base
	{
	public:
		pointer_base( lua::stack&, s32 );
		~pointer_base( void );

		pointer_base( const pointer_base& );
		pointer_base& operator =( const pointer_base& );

	private:
		lua::stack stack;
		s32 ref;
	};

//--- lua::pointer -------------------------------------------------------------
	template< typename type >
		struct lua::pointer
		: pointer_base
	{
	public:
		pointer( lua::stack& stack_, s32 index )
			: pointer_base( stack_, index ), value( traits< type* >::to( stack_, index ) )
		{
		}

		operator type*( void ) const
		{
			return value;
		}

		type* operator ->( void ) const
		{
		}

		type& operator *( void ) const
		{
			return *value;
		}

		type* get( void ) const
		{
			return value;
		}

	private:
		type* value;
	};

//--- lua::release -------------------------------------------------------------
	template< typename type >
		struct lua::release
	{
		type* pointer;

		release( type* pointer_ )
			: pointer( pointer_ )
		{
		}
	};

//--- lua::acquire -------------------------------------------------------------
	template< typename type >
		struct lua::acquire
	{
		type* pointer;

		acquire( type* pointer_ )
			: pointer( pointer_ )
		{
		}
	};

//--- is_specialised -----------------------------------------------------------
	template<>
		struct lua::is_specialised< lua::nil >
		: public true_type
	{
	};

	template<>
		struct lua::is_specialised< lua::stack_index >
		: public true_type
	{
	};

	template<>
		struct lua::is_specialised< lua::binary_string >
		: public true_type
	{
	};

	template< typename type >
		struct lua::is_specialised< lua::pointer< type > >
		: public true_type
	{
	};

	template< typename type >
		struct lua::is_specialised< lua::release< type > >
		: public true_type
	{
	};

	template< typename type >
		struct lua::is_specialised< lua::acquire< type > >
		: public true_type
	{
	};

//--- lua::traits< type, nil > -------------------------------------------------
	template< typename type >
		struct lua::traits< type, typename enable_if< is_like< type, lua::nil > >::type >
	{
		static void push( stack& stack, const nil& )
		{
			stack.pushnil();
		}
	};

//--- lua::traits< type, stack_index > -----------------------------------------
	template< typename type >
		struct lua::traits< type,
		typename enable_if< is_like< type, lua::stack_index > >::type >
	{
		static void push( stack& stack, const stack_index& index )
		{
			stack.pushvalue( index.value );
			stack.remove( index.value );
		}
	};

//--- lua::traits< type, binary_string > ---------------------------------------
	template< typename type >
		struct lua::traits< type,
		typename enable_if< is_like< type, lua::binary_string > >::type >
	{
		static void push( stack& stack, const binary_string& binary )
		{
			stack.pushlstring( static_cast< const c8* >( binary.string ), binary.size );
		}

		static binary_string to( stack& stack, s32 index )
		{
			type_check< binary_string >( stack, index, id::string );
			up_t size;
			const c8* string = stack.tolstring( index, &size );
			return binary_string( string, size );
		}
	};

//--- lua::traits< type, pointer > ---------------------------------------------
	template< typename type >
		struct lua::traits< lua::pointer< type >,
		typename enable_if< lua::is_object< type > >::type >
	{
		static pointer< type > to( stack& stack, s32 index )
		{
			return pointer< type >( stack, index );
		}
	};

//--- lua::traits< type, release > ---------------------------------------------
	template< typename type >
		struct lua::traits< lua::release< type >, void >
	{
		typedef tag< bool > tag_type;

		static release< type > to( stack& stack, s32 index )
		{
			type_check< release< type > >( stack, index, id::userdata );
			tag_type& data = native< tag_type >( stack, index );
			const_cast< bool& >( data.value ) = false;
			return static_cast< type* >( data.pointer );
		}
	};

//--- lua::traits< type, acquire > ---------------------------------------------
	template< typename type >
		struct lua::traits< lua::acquire< type >, void >
	{
		typedef tag< bool > tag_type;

		static void push( stack& stack, const acquire< type >& acquire )
		{
			const c8* name = typeid( type ).name();
			stack.getmetatable( name );
			type_check( stack, -1, id::table );

			new( stack.newuserdata( sizeof( tag_type ) ) ) tag_type( acquire.pointer, true );
			stack.insert( -2 );
			stack.setmetatable( -2 );
		}
	};
}

#endif // OOE_FOUNDATION_LUA_TRAITS_SPECIALISED_HPP
