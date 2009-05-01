/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef BOOST_PP_IS_ITERATING

	#ifndef OOE_FOUNDATION_UTILITY_SIGNAL_HPP
	#define OOE_FOUNDATION_UTILITY_SIGNAL_HPP

#include "foundation/utility/function.hpp"
#include "foundation/utility/noncopyable.hpp"

namespace ooe
{
	template< typename >
		class last_value;

	class slot_base;

	template< typename >
		class slot;

	class signal_base;

	template< typename >
		struct signal;

//--- last_value ---------------------------------------------------------------
	template< typename r >
		class last_value
	{
	public:
		last_value( void )
			: value()
		{
		}

		void operator ()( typename call_traits< r >::param_type value_ )
		{
			value = value_;
		}

		typename call_traits< r >::reference operator ()( void )
		{
			return value;
		}

	private:
		r value;
	};

//--- slot_base ----------------------------------------------------------------
	class slot_base
		: private noncopyable
	{
	public:
		bool connected( void ) const
		{
			return next;
		}

		void disconnect( void )
		{
			if ( !connected() )
				return;

			next->prev = prev;
			prev->next = next;
		}

	protected:
		slot_base( slot_base* next_ = 0, slot_base* prev_ = 0 )
			: next( next_ ), prev( prev_ )
		{
		}

		~slot_base( void )
		{
			disconnect();
		}

	private:
		slot_base* next;
		slot_base* prev;

		friend class signal_base;
	};

//--- signal_base --------------------------------------------------------------
	class signal_base
		: private noncopyable
	{
	public:
		void push_back( slot_base& slot )
		{
			this->root.prev->next = &slot;
			slot.prev = this->root.prev;
			slot.next = &this->root;
			this->root.prev = &slot;
		}

		void push_front( slot_base& slot )
		{
			this->root.next->prev = &slot;
			slot.next = this->root.next;
			slot.prev = &this->root;
			this->root.next = &slot;
		}

		void pop_back( void )
		{
			this->root.prev = this->root.prev->prev;
			this->root.prev->next = &this->root;
		}

		void pop_front( void )
		{
			this->root.next = this->root.next->next;
			this->root.next->prev = &this->root;
		}

		void clear( void )
		{
			this->root.next = &this->root;
			this->root.prev = &this->root;
		}

	protected:
		slot_base root;

		signal_base( void )
			: root( &root, &root )
		{
		}

		template< typename type >
			const type* next( const slot_base* curr ) const
		{
			return static_cast< const type* >( curr->next );
		}
	};

//--- connect ------------------------------------------------------------------
	inline void connect( signal_base* signal, slot_base& slot )
	{
		if ( signal )
			signal->push_back( slot );
	}
}

	#define BOOST_PP_ITERATION_LIMITS ( 0, OOE_PP_LIMIT )
	#define BOOST_PP_FILENAME_1 "foundation/utility/signal.hpp"
	#include BOOST_PP_ITERATE()
	#undef BOOST_PP_FILENAME_1
	#undef BOOST_PP_ITERATION_LIMITS

	#endif	// OOE_FOUNDATION_UTILITY_SIGNAL_HPP

#else	// BOOST_PP_IS_ITERATING

	#define LIMIT BOOST_PP_ITERATION()

namespace ooe
{
//--- slot ---------------------------------------------------------------------
	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		class slot< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		: public slot_base
	{
	public:
		typedef function< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) > function_type;

		slot( void )
			: slot_base(), call()
		{
		}

		slot( const function_type& call_ )
			: slot_base(), call( call_ )
		{
		}

		slot& operator =( const function_type& call_ )
		{
			call = call_;
			return *this;
		}

	private:
		function_type call;

		r operator ()( BOOST_PP_ENUM_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a ) ) const
		{
			return call( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
		}

		friend class signal< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >;
	};

//--- signal -------------------------------------------------------------------
	template< typename r BOOST_PP_ENUM_TRAILING_PARAMS( LIMIT, typename t ) >
		struct signal< r ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) >
		: public signal_base
	{
		template< typename combiner >
			void operator ()( BOOST_PP_ENUM_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a )
			BOOST_PP_COMMA_IF( LIMIT ) combiner& combine ) const
		{
			typedef slot< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) > slot_type;

			for ( const slot_type* slot = next< slot_type >( &this->root );
				slot != &this->root; slot = next< slot_type >( slot ) )
				combine( ( *slot )( BOOST_PP_ENUM_PARAMS( LIMIT, a ) ) );
		}

		void operator ()( BOOST_PP_ENUM_BINARY_PARAMS
			( LIMIT, typename call_traits< t, >::param_type a ) ) const
		{
			typedef slot< void ( BOOST_PP_ENUM_PARAMS( LIMIT, t ) ) > slot_type;

			for ( const slot_type* slot = next< slot_type >( &this->root );
				slot != &this->root; slot = next< slot_type >( slot ) )
				( *slot )( BOOST_PP_ENUM_PARAMS( LIMIT, a ) );
		}
	};
}

	#undef LIMIT

#endif	// BOOST_PP_IS_ITERATING
