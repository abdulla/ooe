/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_LUA_TRAITS_FORWARD_HPP
#define OOE_FOUNDATION_LUA_TRAITS_FORWARD_HPP

namespace ooe
{
	namespace lua
	{
		template< typename >
			struct tag;

		template< typename >
			struct function_native;

		template< typename >
			struct is_boolean;

		template< typename >
			struct is_arithmetic;

		template< typename >
			struct is_fundamental_ptr;

		template< typename >
			struct is_pointer;

		template< typename >
			struct is_reference;

		template< typename >
			struct is_object;

		template< typename >
			struct is_function;

		template< typename >
			struct is_member;

		template< typename >
			struct is_array;

		template< typename >
			struct is_specialised;

		template< typename, typename = void >
			struct traits;

		template< typename type >
			struct traits< type, typename enable_if< is_boolean< type > >::type >;

		template< typename type >
			struct traits< type, typename enable_if< is_arithmetic< type > >::type >;

		template< typename type >
			struct traits< type, typename enable_if< is_cstring< type > >::type >;

		template< typename type >
			struct traits< type, typename enable_if< is_stdstring< type > >::type >;

		template< typename type >
			struct traits< type, typename enable_if< is_fundamental_ptr< type > >::type >;

		template< typename type >
			struct traits< type, typename enable_if< is_pointer< type > >::type >;

		template< typename type >
			struct traits< type, typename enable_if< is_reference< type > >::type >;

		template< typename type >
			struct traits< type, typename enable_if< is_object< type > >::type >;

		template< typename type >
			struct traits< type, typename enable_if< is_member< type > >::type >;

		template< typename type >
			struct traits< type, typename enable_if< is_function< type > >::type >;

		template< typename type >
			struct traits< type, typename enable_if< is_like< type, cfunction > >::type >;

		template< typename type >
			struct traits< type, typename enable_if< is_array< type > >::type >;

		template<>
			struct traits< void, void >;

		template< typename >
			struct array;

		template< typename type >
			type& native( stack&, s32 );

		template< typename >
			void type_check( stack&, s32, id::type );

		inline void type_check( stack&, s32, id::type );
	}

//--- lua::tag -----------------------------------------------------------------
	template< typename type >
		struct lua::tag
	{
		void* const pointer;
		const type value;

		tag( void* pointer_, type value_ )
			: pointer( pointer_ ), value( value_ )
		{
		}
	};

//--- lua::is_boolean ----------------------------------------------------------
	template< typename type >
		struct lua::is_boolean
		: public is_like< type, bool >
	{
	};

//--- lua::is_arithmetic -------------------------------------------------------
	template< typename type >
		struct lua::is_arithmetic
	{
		static const bool value = !is_boolean< type >::value &&
			( ooe::is_arithmetic< typename no_ref< type >::type >::value ||
			is_enum< typename no_ref< type >::type >::value );
	};

//--- lua::is_fundamental_ptr --------------------------------------------------
	template< typename type >
		struct lua::is_fundamental_ptr
	{
		static const bool value = !is_cstring< type >::value &&
			ooe::is_pointer< typename no_ref< type >::type >::value &&
			is_fundamental< typename no_qual< type >::type >::value;
	};

//--- lua::is_pointer ----------------------------------------------------------
	template< typename type >
		struct lua::is_pointer
	{
		static const bool value = ooe::is_pointer< typename no_ref< type >::type >::value &&
			( is_class< typename no_qual< type >::type >::value ||
			is_union< typename no_qual< type >::type >::value );
	};

//--- lua::is_reference --------------------------------------------------------
	template< typename type >
		struct lua::is_reference
	{
		static const bool value = !is_stdstring< type >::value && !is_pointer< type >::value &&
			!is_specialised< typename no_ref< type >::type >::value &&
			( is_class< typename no_qual< type >::type >::value ||
			is_union< typename no_qual< type >::type >::value ) &&
			ooe::is_reference< typename remove_cv< type >::type >::value;
	};

//--- lua::is_object -----------------------------------------------------------
	template< typename type >
		struct lua::is_object
	{
		static const bool value = !is_stdstring< type >::value &&
			!is_specialised< typename no_ref< type >::type >::value &&
			( is_class< typename remove_cv< type >::type >::value ||
			is_union< typename remove_cv< type >::type >::value );
	};

//--- lua::is_function ---------------------------------------------------------
	template< typename type >
		struct lua::is_function
	{
		static const bool value = !is_like< type, cfunction >::value &&
			is_function_pointer< typename no_ref< type >::type >::value;
	};

//--- lua::is_member -----------------------------------------------------------
	template< typename type >
		struct lua::is_member
		: public is_member_pointer< typename no_ref< type >::type >
	{
	};

//--- lua::is_array ------------------------------------------------------------
	template< typename type >
		struct lua::is_array
		: public ooe::is_array< typename no_ref< type >::type >
	{
	};

//--- lua::is_specialised ------------------------------------------------------
	template< typename type >
		struct lua::is_specialised
		: public false_type
	{
	};

//--- lua::traits< type, boolean > ---------------------------------------------
	template< typename type >
		struct lua::traits< type, typename enable_if< lua::is_boolean< type > >::type >
	{
		static void push( stack& stack, bool boolean )
		{
			stack.pushboolean( boolean );
		}

		static bool to( stack& stack, s32 index )
		{
			type_check< bool >( stack, index, id::boolean );
			return stack.toboolean( index );
		}
	};

//--- lua::traits< type, arithmetic > ------------------------------------------
	template< typename type >
		struct lua::traits< type, typename enable_if< lua::is_arithmetic< type > >::type >
	{
		typedef typename no_ref< type >::type value_type;

		static void push( stack& stack, value_type arithmetic )
		{
			stack.pushnumber( static_cast< f32 >( arithmetic ) );
		}

		static value_type to( stack& stack, s32 index )
		{
			type_check< value_type >( stack, index, id::number );
			return static_cast< value_type >( stack.tonumber( index ) );
		}
	};

//--- lua::traits< type, cstring > ---------------------------------------------
	template< typename type >
		struct lua::traits< type, typename enable_if< is_cstring< type > >::type >
	{
		static void push( stack& stack, const c8* string )
		{
			stack.pushlstring( string, std::char_traits< c8 >::length( string ) );
		}

		static const c8* to( stack& stack, s32 index )
		{
			type_check< const c8* >( stack, index, id::string );
			return stack.tolstring( index, 0 );
		}
	};

//--- lua::traits< type, stdstring > -------------------------------------------
	template< typename type >
		struct lua::traits< type, typename enable_if< is_stdstring< type > >::type >
	{
		static void push( stack& stack, const std::string& string )
		{
			stack.pushlstring( string.c_str(), string.size() );
		}

		static std::string to( stack& stack, s32 index )
		{
			type_check< std::string >( stack, index, id::string );
			up_t size;
			const c8* string = stack.tolstring( index, &size );
			return std::string( string, size );
		}
	};

//--- lua::traits< type, fundamental_ptr > -------------------------------------
	template< typename type >
		struct lua::traits< type, typename enable_if< lua::is_fundamental_ptr< type > >::type >
	{
		typedef typename no_ref< type >::type value_type;

		static void push( stack& stack, value_type pointer )
		{
			typedef typename no_qual< type >::type unqualified_type;
			stack.pushlightuserdata( const_cast< unqualified_type* >( pointer ) );
		}

		static value_type to( stack& stack, s32 index )
		{
			type_check< value_type >( stack, index, id::lightuserdata );
			return static_cast< value_type >( stack.touserdata( index ) );
		}
	};

//--- lua::traits< type, pointer > ---------------------------------------------
	template< typename type >
		struct lua::traits< type, typename enable_if< lua::is_pointer< type > >::type >
	{
		typedef typename no_ref< type >::type value_type;
		typedef typename no_qual< type >::type unqualified_type;
		typedef tag< bool > tag_type;

		static void push( stack& stack, value_type pointer )
		{
			stack.getmetatable( typeid( unqualified_type ).name() );

			if ( stack.type( -1 ) != id::table )
			{
				stack.pop( 1 );
				stack.pushlightuserdata( pointer );
				return;
			}

			new( stack.newuserdata( sizeof( tag_type ) ) ) tag_type( pointer, false );
			stack.insert( -2 );
			stack.setmetatable( -2 );
		}

		static value_type to( stack& stack, s32 index )
		{
			stack.getmetatable( typeid( unqualified_type ).name() );
			id::type is_table = stack.type( -1 );
			stack.pop( 1 );

			if ( is_table != id::table )
			{
				type_check< value_type >( stack, index, id::lightuserdata );
				return static_cast< value_type >( stack.touserdata( index ) );
			}

			type_check< value_type >( stack, index, id::userdata );
			tag_type& data = native< tag_type >( stack, index );
			return static_cast< value_type >( data.pointer );
		}
	};

//--- lua::traits< type, reference > -------------------------------------------
	template< typename type >
		struct lua::traits< type, typename enable_if< lua::is_reference< type > >::type >
	{
		typedef typename no_ref< type >::type value_type;

		static void push( stack& stack, type reference )
		{
			traits< value_type* >::push( stack, const_cast< value_type* >( &reference ) );
		}

		static type to( stack& stack, s32 index )
		{
			return *traits< value_type* >::to( stack, index );
		}
	};

//--- lua::traits< type, object > ----------------------------------------------
	template< typename type >
		struct lua::traits< type, typename enable_if< lua::is_object< type > >::type >
	{
		typedef tag< bool > tag_type;

		static void push( stack& stack, const type& value )
		{
			const c8* name = typeid( type ).name();
			stack.getmetatable( name );
			type_check( stack, -1, id::table );

			new( stack.newuserdata( sizeof( tag_type ) ) ) tag_type( new type( value ), true );
			stack.insert( -2 );
			stack.setmetatable( -2 );
		}

		static type to( stack& stack, s32 index )
		{
			type_check< type >( stack, index, id::userdata );
			tag_type& data = native< tag_type >( stack, index );
			return *static_cast< type* >( data.pointer );
		}
	};

//--- lua::traits< type, member > ----------------------------------------------
	template< typename type >
		struct lua::traits< type, typename enable_if< lua::is_member< type > >::type >
	{
		typedef typename no_ref< type >::type value_type;

		static void push( stack& stack, value_type member )
		{
			void* pointer = stack.newuserdata( sizeof( value_type ) );
			std::memcpy( pointer, &member, sizeof( value_type ) );
		}

		static value_type to( stack& stack, s32 index )
		{
			type_check< value_type >( stack, index, id::userdata );
			value_type member;
			std::memcpy( &member, stack.touserdata( index ), sizeof( value_type ) );
			return member;
		}
	};

//--- lua::traits< type, function > --------------------------------------------
	template< typename type >
		struct lua::traits< type, typename enable_if< lua::is_function< type > >::type >
	{
		typedef typename add_pointer< typename no_qual< type >::type >::type value_type;

		static void push( stack& stack, value_type function )
		{
			union { value_type in; void* out; } pun;
			pun.in = function;
			stack.pushlightuserdata( pun.out );
			stack.pushcclosure( lua::function_native< value_type >::dispatch, 1 );
		}
	};

//--- lua::traits< type, cfunction > -------------------------------------------
	template< typename type >
		struct lua::traits< type, typename enable_if< is_like< type, lua::cfunction > >::type >
	{
		static void push( stack& stack, cfunction function )
		{
			stack.pushcclosure( function, 0 );
		}
	};

//--- lua::traits< type, array > -----------------------------------------------
	template< typename type >
		struct lua::traits< type, typename enable_if< lua::is_array< type > >::type >
	{
		typedef typename no_ref< type >::type value_type;
		typedef typename remove_extent< value_type >::type return_type;
		typedef tag< up_t > tag_type;

		typedef array< return_type > array_type;
		static const up_t size = extent< value_type >::value;

		static void push( stack& stack, value_type& array )
		{
			new( stack.newuserdata( sizeof( tag_type ) ) ) tag_type( array, size );

			if ( stack.newmetatable( typeid( value_type ).name() ) )
			{
				// metatable.__index = index
				traits< const c8* >::push( stack, "__index" );
				traits< cfunction >::push( stack, array_type::index );
				stack.rawset( -3 );

				// metatable.__newindex = newindex
				traits< const c8* >::push( stack, "__newindex" );
				traits< cfunction >::push( stack, array_type::newindex );
				stack.rawset( -3 );

				// metatable.__len = len
				traits< const c8* >::push( stack, "__len" );
				traits< cfunction >::push( stack, array_type::len );
				stack.rawset( -3 );
			}

			stack.setmetatable( -2 );
		}

		static return_type* to( stack& stack, s32 index )
		{
			type_check< value_type >( stack, index, id::userdata );
			tag_type& data = native< tag_type >( stack, 1 );
			return static_cast< return_type* >( data.pointer );
		}
	};

//--- lua::traits< void, void > ------------------------------------------------
	template<>
		struct lua::traits< void, void >
	{
		static void to( stack&, s32 )
		{
		}
	};

//--- lua::array ---------------------------------------------------------------
	template< typename type >
		struct lua::array
	{
		typedef tag< up_t > tag_type;

		static s32 index( lua::state* state )
		{
			stack stack( state );
			tag_type& data = native< tag_type >( stack, 1 );
			type* array = static_cast< type* >( data.pointer );
			up_t index = traits< up_t >::to( stack, 2 ) - 1;
			traits< type >::push( stack, array[ index ] );
			return 1;
		}

		static s32 newindex( lua::state* state )
		{
			stack stack( state );
			tag_type& data = native< tag_type >( stack, 1 );
			type* array = static_cast< type* >( data.pointer );
			up_t index = traits< up_t >::to( stack, 2 ) - 1;
			array[ index ] = traits< type& >::to( stack, 3 );
			return 0;
		}

		static s32 len( lua::state* state )
		{
			stack stack( state );
			tag_type& data = native< tag_type >( stack, 1 );
			traits< up_t >::push( stack, data.value );
			return 1;
		}
	};

//--- lua::native --------------------------------------------------------------
	template< typename type >
		type& lua::native( stack& stack, s32 index )
	{
		return *static_cast< type* >( stack.touserdata( index ) );
	}

//--- lua::type_check ----------------------------------------------------------
	template< typename type >
		void lua::type_check( stack& stack, s32 index, id::type id )
	{
		if ( stack.type( index ) == id )
			return;

		stack.where( 1 );
		error::lua lua( traits< const c8* >::to( stack, -1 ) );
		stack.pop( 1 );

		throw static_cast< error::runtime& >( lua ) << "bad argument " << index <<
			" (" << demangle( typeid( type ).name() ) <<
			" expected, got " << stack.type_name( stack.type( index ) ) << ")";
	}

	void lua::type_check( stack& stack, s32 index, id::type id )
	{
		if ( stack.type( index ) == id )
			return;

		error::runtime error( "bad argument " );
		error << index << " (" << stack.type_name( id ) << " expected, got " <<
			stack.type_name( stack.type( index ) ) << ")";

		stack.where( 1 );
		traits< const c8* >::push( stack, error.what() );
		stack.concat( 2 );
		stack.error();
	}
}

#endif	// OOE_FOUNDATION_LUA_TRAITS_FORWARD_HPP
