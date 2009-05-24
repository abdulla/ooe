/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_LUA_TYPE_HPP
	#define OOE_FOUNDATION_LUA_TYPE_HPP

#include <new>

#include "foundation/lua/error.hpp"
#include "foundation/lua/traits.hpp"

namespace ooe
{
	namespace lua
	{
		template< typename >
			struct do_destruct;

		template< typename, typename >
			struct do_construct;

		template< typename, typename >
			struct do_function;

		template< typename, typename >
			struct do_object;

		template< typename >
			struct is_value;

		class type_base;

		template< typename >
			struct type;

		template< typename, typename >
			struct inherit;

		template< typename, typename = void >
			struct operands;

		template< typename, typename = void >
			struct operand1;

//--- specialisations ----------------------------------------------------------
		template< typename t, typename r, typename s >
			struct do_object< t, r s::* >;

		template< typename type >
			struct operands< type,
			typename enable_if< is_member_function_pointer< type > >::type >;

		template< typename type >
			struct operands< type, typename enable_if< is_function_pointer< type > >::type >;

		template< typename t >
			struct operand1< t,
			typename enable_if< is_member_function_pointer< t > >::type >;

		template< typename t >
			struct operand1< t, typename enable_if< is_function_pointer< t > >::type >;

//--- operator helpers ---------------------------------------------------------
		template< typename type >
			tuple< const c8*, type >
			add( type, typename enable_if_c< operands< type >::value == 2 >::type* = 0 );

		template< typename type >
			tuple< const c8*, type >
			sub( type, typename enable_if_c< operands< type >::value == 2 >::type* = 0 );

		template< typename type >
			tuple< const c8*, type >
			mul( type, typename enable_if_c< operands< type >::value == 2 >::type* = 0 );

		template< typename type >
			tuple< const c8*, type >
			div( type, typename enable_if_c< operands< type >::value == 2 >::type* = 0 );

		template< typename type >
			tuple< const c8*, type >
			mod( type, typename enable_if_c< operands< type >::value == 2 >::type* = 0 );

		template< typename type >
			tuple< const c8*, type >
			pow( type, typename enable_if_c< operands< type >::value == 2 >::type* = 0 );

		template< typename type >
			tuple< const c8*, type >
			unm( type, typename enable_if_c< operands< type >::value == 1 >::type* = 0 );

		template< typename type >
			tuple< const c8*, type >
			concat( type, typename enable_if_c< operands< type >::value == 2 >::type* = 0 );

		template< typename type >
			tuple< const c8*, type >
			len( type, typename enable_if_c< operands< type >::value == 1 >::type* = 0 );

		template< typename type >
			tuple< const c8*, type >
			eq( type, typename enable_if_c< operands< type >::value == 2 >::type* = 0 );

		template< typename type >
			tuple< const c8*, type >
			lt( type, typename enable_if_c< operands< type >::value == 2 >::type* = 0 );

		template< typename type >
			tuple< const c8*, type >
			le( type, typename enable_if_c< operands< type >::value == 2 >::type* = 0 );

		template< typename type >
			tuple< const c8*, type >
			index( type, typename enable_if_c< operands< type >::value == 2 &&
			!is_cstring< typename operand1< type >::type >::value &&
			!is_stdstring< typename operand1< type >::type >::value >::type* = 0 );

		template< typename type >
			tuple< const c8*, type >
			newindex( type, typename enable_if_c< operands< type >::value == 3 &&
			!is_cstring< typename operand1< type >::type >::value &&
			!is_stdstring< typename operand1< type >::type >::value >::type* = 0 );
	}

//--- lua::is_value ------------------------------------------------------------
	template< typename type >
		struct lua::is_value
	{
		static const bool value = !is_member_pointer< type >::value &&
			!is_function_pointer< type >::value;
	};

//--- lua::type_base -----------------------------------------------------------
	class OOE_VISIBLE lua::type_base
	{
	protected:
		lua::stack stack;
		const s32 ref;

		type_base( const lua::stack&, const c8* );
		~type_base( void );
	};

//--- lua::type ----------------------------------------------------------------
	template< typename t >
		struct lua::type
		: public type_base
	{
		type( const lua::stack& stack_ )
			: type_base( stack_, typeid( typename no_qual< t >::type ).name() )
		{
			stack.rawgeti( registry, ref );

			// metatable.__gc = t::~t
			traits< const c8* >::push( stack, "__gc" );
			traits< cfunction >::push( stack, do_destruct< t >::dispatch );
			stack.rawset( -3 );

			stack.pop( 1 );
		}

		template< typename m >
			type& insert( const c8* name, m member,
			typename enable_if< is_member_function_pointer< m > >::type* = 0 )
		{
			stack.rawgeti( registry, ref );

			// metatable.name = member
			traits< const c8* >::push( stack, name );
			traits< m >::push( stack, member );
			stack.pushcclosure( do_function< t, m >::dispatch, 1 );
			stack.rawset( -3 );

			stack.pop( 1 );
			return *this;
		}

		template< typename f >
			type& insert( const c8* name, f function,
			typename enable_if< is_function_pointer< f > >::type* = 0 )
		{
			stack.rawgeti( registry, ref );

			// metatable.name = function
			traits< const c8* >::push( stack, name );
			traits< f >::push( stack, function );
			stack.rawset( -3 );

			stack.pop( 1 );
			return *this;
		}

		template< typename m >
			type& insert( const c8* name, m member,
			typename enable_if< is_member_object_pointer< m > >::type* = 0 )
		{
			stack.rawgeti( registry, ref );

			// metatable.name = table
			traits< const c8* >::push( stack, name );
			stack.createtable( 0, 2 );

			// table.__index = read
			traits< const c8* >::push( stack, "__index" );
			traits< m >::push( stack, member );
			stack.pushcclosure( do_object< t, m >::dispatch_get, 1 );
			stack.rawset( - 3 );

			// table.__newindex = write
			traits< const c8* >::push( stack, "__newindex" );
			traits< m >::push( stack, member );
			stack.pushcclosure( do_object< t, m >::dispatch_set, 1 );
			stack.rawset( - 3 );

			stack.rawset( -3 );
			stack.pop( 1 );
			return *this;
		}

		template< typename v >
			type& insert( const c8* name, v value, typename enable_if< is_value< v > >::type* = 0 )
		{
			stack.rawgeti( registry, ref );

			// metatable.name = value
			traits< const c8* >::push( stack, name );
			traits< v >::push( stack, value );
			stack.rawset( -3 );

			stack.pop( 1 );
			return *this;
		}

		template< typename m >
			type& insert( tuple< const c8*, m > tuple,
			typename enable_if< is_member_function_pointer< m > >::type* = 0 )
		{
			return insert( tuple._0, tuple._1 );
		}

		template< typename f >
			type& insert( tuple< const c8*, f > tuple,
			typename enable_if< is_function_pointer< f > >::type* = 0 )
		{
			return insert( tuple._0, tuple._1 );
		}
	};

//--- lua::inherit -------------------------------------------------------------
	template< typename t, typename s >
		struct lua::inherit
		: public lua::type< t >
	{
		inherit( const lua::stack& stack_ )
			: type< t >( stack_ )
		{
			this->stack.rawgeti( registry, this->ref );

			// t.metatable.metatable = s.metatable
			this->stack.getmetatable( typeid( typename no_qual< s >::type ).name() );
			this->stack.setmetatable( -2 );

			this->stack.pop( 1 );
		}
	};

//--- lua::operands ------------------------------------------------------------
	template< typename type >
		struct lua::operands< type,
		typename enable_if< is_member_function_pointer< type > >::type >
	{
		static const u32 value = callable_traits< type >::arity + 1;
	};

	template< typename type >
		struct lua::operands< type, typename enable_if< is_function_pointer< type > >::type >
	{
		static const u32 value = callable_traits< type >::arity;
	};

//--- lua::operand1 ------------------------------------------------------------
	template< typename t >
		struct lua::operand1< t,
		typename enable_if< is_member_function_pointer< t > >::type >
	{
		typedef typename callable_traits< t >::arg0_type type;
	};

	template< typename t >
		struct lua::operand1< t, typename enable_if< is_function_pointer< t > >::type >
	{
		typedef typename callable_traits< t >::arg1_type type;
	};

//--- operator helpers ---------------------------------------------------------
	template< typename type >
		tuple< const c8*, type >
		lua::add( type op, typename enable_if_c< lua::operands< type >::value == 2 >::type* )
	{
		return make_tuple( "__add", op );
	}

	template< typename type >
		tuple< const c8*, type >
		lua::sub( type op, typename enable_if_c< lua::operands< type >::value == 2 >::type* )
	{
		return make_tuple( "__sub", op );
	}

	template< typename type >
		tuple< const c8*, type >
		lua::mul( type op, typename enable_if_c< lua::operands< type >::value == 2 >::type* )
	{
		return make_tuple( "__mul", op );
	}

	template< typename type >
		tuple< const c8*, type >
		lua::div( type op, typename enable_if_c< lua::operands< type >::value == 2 >::type* )
	{
		return make_tuple( "__div", op );
	}

	template< typename type >
		tuple< const c8*, type >
		lua::mod( type op, typename enable_if_c< lua::operands< type >::value == 2 >::type* )
	{
		return make_tuple( "__mod", op );
	}

	template< typename type >
		tuple< const c8*, type >
		lua::pow( type op, typename enable_if_c< lua::operands< type >::value == 2 >::type* )
	{
		return make_tuple( "__pow", op );
	}

	template< typename type >
		tuple< const c8*, type >
		lua::unm( type op, typename enable_if_c< lua::operands< type >::value == 1 >::type* )
	{
		return make_tuple( "__unm", op );
	}

	template< typename type >
		tuple< const c8*, type >
		lua::concat( type op,
		typename enable_if_c< lua::operands< type >::value == 2 >::type* )
	{
		return make_tuple( "__concat", op );
	}

	template< typename type >
		tuple< const c8*, type >
		lua::len( type op, typename enable_if_c< lua::operands< type >::value == 1 >::type* )
	{
		return make_tuple( "__len", op );
	}

	template< typename type >
		tuple< const c8*, type >
		lua::eq( type op, typename enable_if_c< lua::operands< type >::value == 2 >::type* )
	{
		return make_tuple( "__eq", op );
	}

	template< typename type >
		tuple< const c8*, type >
		lua::lt( type op, typename enable_if_c< lua::operands< type >::value == 2 >::type* )
	{
		return make_tuple( "__lt", op );
	}

	template< typename type >
		tuple< const c8*, type >
		lua::le( type op, typename enable_if_c< lua::operands< type >::value == 2 >::type* )
	{
		return make_tuple( "__le", op );
	}

	template< typename type >
		tuple< const c8*, type >
		lua::index( type op, typename enable_if_c< lua::operands< type >::value == 2 &&
		!is_cstring< typename lua::operand1< type >::type >::value &&
		!is_stdstring< typename lua::operand1< type >::type >::value >::type* = 0 )
	{
		return make_tuple( "__array", op );
	}

	template< typename type >
		tuple< const c8*, type >
		lua::newindex( type op, typename enable_if_c< lua::operands< type >::value == 3 &&
		!is_cstring< typename lua::operand1< type >::type >::value &&
		!is_stdstring< typename lua::operand1< type >::type >::value >::type* = 0 )
	{
		return make_tuple( "__newarray", op );
	}

//--- lua::do_destruct ---------------------------------------------------------
	template< typename t >
		struct lua::do_destruct
	{
		static s32 dispatch( lua::state* state )
		{
			stack stack( state );
			typedef tag< bool > tag_type;
			tag_type& data = native< tag_type >( stack, 1 );

			if ( data.value )
				delete static_cast< t* >( data.pointer );

			return 0;
		}
	};

//--- lua::do_object -----------------------------------------------------------
	template< typename t, typename r, typename s >
		struct lua::do_object< t, r s::* >
	{
		static s32 dispatch_get( lua::state* state )
		{
			stack stack( state );
			t* object = traits< t* >::to( stack, 1 );

			typedef r t::* member_type;
			member_type member = traits< member_type >::to( stack, upvalue( 1 ) );

			typedef typename add_reference< r >::type return_type;
			traits< return_type >::push( stack, object->*member );
			return 1;
		}

		static s32 dispatch_set( lua::state* state )
		{
			stack stack( state );
			t* object = traits< t* >::to( stack, 1 );

			typedef r t::* member_type;
			member_type member = traits< member_type >::to( stack, upvalue( 1 ) );

			typedef typename add_reference< r >::type return_type;
			object->*member = traits< return_type >::to( stack, 2 );
			return 0;
		}
	};
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/lua/type.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_LUA_TYPE_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()
	#define TO( z, n, _ ) traits< t ## n >::to( stack, -BOOST_PP_SUB( LIMIT, n ) )

namespace ooe
{
	namespace lua
	{
		template< typename t BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			struct do_construct< t, ooe::constructor< BOOST_PP_ENUM_PARAMS( LIMIT, t ) > >;

		template< typename t, typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			struct do_function< t, r ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

		template< typename t, typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			struct do_function< t, r ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) const >;

		template< typename t BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			struct do_function< t, void ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;

		template< typename t BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			struct do_function< t, void ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) const >;

		template< typename t BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			stack_index constructor( stack& stack );

//--- operator helpers ---------------------------------------------------------
		template< typename r, typename t BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			tuple< const c8*, r ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
			call( r ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) );

		template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
			tuple< const c8*, r ( * )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
			call( r ( * )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) );
	}

//--- lua::do_construct --------------------------------------------------------
	template< typename t BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct lua::do_construct< t, ooe::constructor< BOOST_PP_ENUM_PARAMS( LIMIT, t ) > >
	{
		static s32 dispatch( lua::state* state )
		{
			stack stack( state );
			t* type = 0;
			OOE_ERROR( type = new BOOST_PP_IF( LIMIT, t( BOOST_PP_ENUM( LIMIT, TO, ~ ) ), t ) );

			typedef tag< bool > tag_type;
			new( stack.newuserdata( sizeof( tag_type ) ) ) tag_type( type, true );
			stack.pushvalue( upvalue( 1 ) );
			stack.setmetatable( -2 );

			return 1;
		}
	};

//--- lua::do_function ---------------------------------------------------------
	template< typename t, typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct lua::do_function< t, r ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static s32 dispatch( lua::state* state )
		{
			stack stack( state );

			t* object = traits< t* >::to( stack, 1 );
			typedef r ( t::* member_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			member_type member = traits< member_type >::to( stack, upvalue( 1 ) );

			OOE_ERROR( traits< r >::
				push( stack, ( object->*member )( BOOST_PP_ENUM( LIMIT, TO, ~ ) ) ) );
			return 1;
		}
	};

	template< typename t, typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct lua::do_function< t, r ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) const >
		: public do_function< t, r ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
	};

	template< typename t BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct lua::do_function< t, void ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
		static s32 dispatch( lua::state* state )
		{
			stack stack( state );

			t* object = traits< t* >::to( stack, 1 );
			typedef void ( t::* member_type )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) );
			member_type member = traits< member_type >::to( stack, upvalue( 1 ) );

			OOE_ERROR( ( object->*member )( BOOST_PP_ENUM( LIMIT, TO, ~ ) ) );
			return 0;
		}
	};

	template< typename t BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct lua::do_function< t, void ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) const >
		: public do_function< t, void ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
	{
	};

//--- lua::constructor ---------------------------------------------------------
	template< typename t BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		lua::stack_index lua::constructor( stack& stack )
	{
		const c8* name = typeid( typename no_qual< t >::type ).name();
		stack.getmetatable( name );

		if ( stack.type( -1 ) != id::table )
		{
			stack.pop( 1 );
			error::lua lua( "constructor: " );
			throw static_cast< error::runtime& >( lua )
				<< "Type \"" << demangle( name ) << "\" not defined";
		}

		typedef ooe::constructor< BOOST_PP_ENUM_PARAMS( LIMIT, t ) > construct;
		stack.pushcclosure( do_construct< t, construct >::dispatch, 1 );
		return stack.gettop();
	}

//--- operator helpers ---------------------------------------------------------
	template< typename r, typename t BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		tuple< const c8*, r ( t::* )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		lua::call( r ( t::* op )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) )
	{
		return make_tuple( "__call", op );
	}

	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		tuple< const c8*, r ( * )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		lua::call( r ( * op )( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) )
	{
		return make_tuple( "__call", op );
	}
}

	#undef TO
	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
