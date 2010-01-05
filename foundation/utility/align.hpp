/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_ALIGN_HPP
#define OOE_FOUNDATION_UTILITY_ALIGN_HPP

#include <new>

#include <cstdlib>

#include "foundation/utility/pointer.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

#ifdef __GNUC__
	#define OOE_ALIGN( size ) __attribute__( ( __aligned__( size ) ) )
#else
	#define OOE_ALIGN( size )
#endif

template< std::size_t alignment >
	std::size_t align_by( std::size_t size )
{
	std::size_t quotient = size / alignment;
	return ( quotient + 1 ) * alignment;
}

struct align_to
{
	std::size_t alignment;

	align_to( std::size_t alignment_ = sizeof( void* ) )
		: alignment( alignment_ )
	{
	}
};

inline void* aligned_allocate( std::size_t size, std::size_t alignment )
{
	void* pointer;

	if ( posix_memalign( &pointer, alignment, size ) )
		throw std::bad_alloc();

	return pointer;
}

OOE_NAMESPACE_END( ( ooe ) )

inline void* operator new( std::size_t size, const ooe::align_to& align_to )
{
	return ooe::aligned_allocate( size, align_to.alignment );
}

inline void* operator new[]( std::size_t size, const ooe::align_to& align_to )
{
	return ooe::aligned_allocate( size, align_to.alignment );
}

inline void operator delete( void* pointer, const ooe::align_to& )
{
	std::free( pointer );
}

inline void operator delete[]( void* pointer, const ooe::align_to& )
{
	std::free( pointer );
}

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- deallocate_aligned ---------------------------------------------------------------------------
template< typename type >
	void deallocate_aligned( type* value )
{
	value->~type();
	std::free( value );
}

//--- scoped_aligned -------------------------------------------------------------------------------
template< typename type >
	struct scoped_aligned
	: public scoped_dereference< type, deallocate_aligned< type > >
{
	scoped_aligned( type* value_ )
		: scoped_dereference< type, deallocate_aligned< type > >( value_ )
	{
	}
};

//--- shared_aligned -------------------------------------------------------------------------------
template< typename type >
	struct shared_aligned
	: public shared_dereference< type, deallocate_aligned< type >, unsigned >
{
	shared_aligned( type* value = 0 )
		: shared_dereference< type, deallocate_aligned< type >, unsigned >( value )
	{
	}
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_FOUNDATION_UTILITY_ALIGN_HPP
