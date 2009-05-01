/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_PARALLEL_THREAD_HPP
#define OOE_FOUNDATION_PARALLEL_THREAD_HPP

#include <pthread.h>

#include "foundation/utility/function.hpp"
#include "foundation/utility/macro.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/noncopyable.hpp"
#include "foundation/utility/tuple.hpp"

namespace ooe
{
//--- thread -------------------------------------------------------------------
	class OOE_VISIBLE thread
		: private noncopyable
	{
	public:
		typedef function< void* ( void* ) > function_type;
		typedef ooe::tuple< function_type, void* > tuple_type;

		thread( void );
		thread( const function_type&, void* );
		~thread( void );

		bool operator ==( const thread& ) const;
		void* join( void );

		static void yield( void );
		static void exit( void* ) OOE_NORETURN;

	private:
		pthread_t pthread;
		tuple_type tuple;
		bool joined;
	};

	inline bool operator !=( const thread& x, const thread& y )
	{
		return !( x == y );
	}

//--- tls_base -----------------------------------------------------------------
	class OOE_VISIBLE tls_base
		: private noncopyable
	{
	protected:
		typedef void* ( * create_type )( void );
		typedef void ( * destroy_type )( void* );

		tls_base( create_type, destroy_type );
		~tls_base( void );

		void* get( void ) const;
		void clear( destroy_type );

	private:
		pthread_key_t pthread_key;
		create_type create;
	};

//--- tls ----------------------------------------------------------------------
	template< typename type >
		struct tls
		: public tls_base
	{
		tls( void )
			: tls_base( ooe::create< type >, destroy< type > )
		{
		}

		operator type&( void ) const
		{
			return *static_cast< type* >( this->get() );
		}

		tls& operator =( const type& value )
		{
			*static_cast< type* >( this->get() ) = value;
			return *this;
		}

		void clear( void )
		{
			tls_base::clear( destroy< type > );
		}
	};
}

#endif	// OOE_FOUNDATION_PARALLEL_THREAD_HPP
