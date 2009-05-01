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

	class atom
	{
	public:
		atom( int value = 0 )
			: atomic( value )
		{
		}

		operator int( void ) const
		{
			return atomic;
		}

		void operator =( int value )
		{
			exchange( value );
		}

		int operator ++( void )
		{
			return exchange_add( 1 ) + 1;
		}

		int operator ++( int )
		{
			return exchange_add( 1 );
		}

		int operator --( void )
		{
			return exchange_add( -1 ) - 1;
		}

		int operator --( int )
		{
			return exchange_add( -1 );
		}

		int operator +=( int value )
		{
			return exchange_add( value ) + value;
		}

		int operator -=( int value )
		{
			return exchange_add( -value ) - value;
		}

	private:
		volatile int atomic;

		int exchange_add( int value )
		{
#if __GNUC__ >= 4 && __GNUC_MINOR__ >= 1
			return __sync_fetch_and_add( &atomic, value );
#elif defined( __i386__ ) || defined( __x86_64 )
			__asm__ __volatile__
			(
				"lock; xadd %0, %1"
				: "=r" ( value ), "=m" ( atomic )
				: "0" ( value ), "m" ( atomic )
				: "memory", "cc"
			);

			return value;
#endif
		}

		int exchange( int value )
		{
#if defined( __i386__ ) || defined( __x86_64 )
			__asm__ __volatile__
			(
				"lock; xchg %0, %1"
				: "=r" ( value ), "=m" ( atomic )
				: "0" ( value ), "m" ( atomic )
				: "memory", "cc"
			);

			return value;
#endif
		}
	} OOE_ALIGN( sizeof( int ) );

//--- atom_ptr -----------------------------------------------------------------
	template< typename type, template< typename > class deleter = delete_ptr >
		struct atom_ptr
		: public shared_dereference< type, deleter< type >, atom >
	{
		atom_ptr( type* value = 0 )
			: shared_dereference< type, deleter< type >, atom >( value )
		{
		}
	};

//--- atom_array ---------------------------------------------------------------
	template< typename type, template< typename > class deleter = delete_array >
		struct atom_array
		: public shared_dereference< type, deleter< type >, atom >
	{
		atom_array( type* value = 0 )
			: shared_dereference< type, deleter< type >, atom >( value )
		{
		}
	};

//--- atom_free ----------------------------------------------------------------
	template< typename type, template< typename > class deleter = delete_free >
		struct atom_free
		: public shared_dereference< type, deleter< type >, atom >
	{
		atom_free( type* value = 0 )
			: shared_dereference< type, deleter< type >, atom >( value )
		{
		}
	};
}

#endif	// OOE_FOUNDATION_UTILITY_ATOMIC_HPP
