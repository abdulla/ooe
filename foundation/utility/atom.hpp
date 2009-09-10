/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_ATOMIC_HPP
#define OOE_FOUNDATION_UTILITY_ATOMIC_HPP

#include "foundation/utility/pointer.hpp"

namespace ooe
{
	inline void memory_barrier( void )
	{
#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 1
		__sync_synchronize();
#else
		__asm__ __volatile__( "" : : : "memory" );
#endif
	}

	template< typename type >
		class atom
	{
	public:
		atom( type value = 0 )
			: atomic( value )
		{
		}

		operator type( void ) const
		{
			return atomic;
		}

		void operator =( type value )
		{
			exchange( value );
		}

		type operator ++( void )
		{
			return exchange_add( 1 ) + 1;
		}

		type operator ++( int )
		{
			return exchange_add( 1 );
		}

		type operator --( void )
		{
			return exchange_add( -1 ) - 1;
		}

		type operator --( int )
		{
			return exchange_add( -1 );
		}

		type operator +=( type value )
		{
			return exchange_add( value ) + value;
		}

		type operator -=( type value )
		{
			return exchange_add( -value ) - value;
		}

		bool cas( type compare, type value )
		{
			return compare_exchange( compare, value );
		}

	private:
		volatile type atomic;

#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 1
		type exchange_add( type value )
		{
			return __sync_fetch_and_add( &atomic, value );
		}

		type exchange( type value )
		{
			return __sync_lock_test_and_set( &atomic, value );
		}

		bool compare_exchange( type compare, type value )
		{
			return __sync_bool_compare_and_swap( &atomic, compare, value );
		}
#endif
	};

//--- atom_ptr -----------------------------------------------------------------
	template< typename type, template< typename > class deleter = delete_ptr >
		struct atom_ptr
		: public shared_dereference< type, deleter< type >, atom< unsigned > >
	{
		atom_ptr( type* value = 0 )
			: shared_dereference< type, deleter< type >, atom< unsigned > >( value )
		{
		}
	};

//--- atom_array ---------------------------------------------------------------
	template< typename type, template< typename > class deleter = delete_array >
		struct atom_array
		: public shared_dereference< type, deleter< type >, atom< unsigned > >
	{
		atom_array( type* value = 0 )
			: shared_dereference< type, deleter< type >, atom< unsigned > >( value )
		{
		}
	};

//--- atom_free ----------------------------------------------------------------
	template< typename type, template< typename > class deleter = delete_free >
		struct atom_free
		: public shared_dereference< type, deleter< type >, atom< unsigned > >
	{
		atom_free( type* value = 0 )
			: shared_dereference< type, deleter< type >, atom< unsigned > >( value )
		{
		}
	};
}

#endif	// OOE_FOUNDATION_UTILITY_ATOMIC_HPP
