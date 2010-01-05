/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_IO_BUFFER_HPP
#define OOE_FOUNDATION_IPC_IO_BUFFER_HPP

#include "foundation/ipc/io_buffer_forward.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/shared_memory.hpp"
#include "foundation/utility/align.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc ) )

//--- buffer_allocator -----------------------------------------------------------------------------
struct buffer_allocator
{
	virtual ~buffer_allocator( void ) {}
	virtual bool empty( void ) = 0;
	virtual void allocate( up_t ) = 0;
	virtual u8* get( up_t ) const = 0;
};

//--- heap_allocator -------------------------------------------------------------------------------
class heap_allocator
	: public buffer_allocator
{
public:
	heap_allocator( void )
		: memory( 0 )
	{
	}

	virtual bool empty( void )
	{
		return !memory.as< u8 >();
	}

	// TODO: round allocations to page-sizes and reuse previous allocations
	// TODO: better yet, use vmsplice to gift pages and accelerate transfer
	virtual void allocate( up_t size )
	{
		aligned_ptr< static_alignment >( aligned< static_alignment >( size ) ).swap( memory );
	}

	virtual u8* get( up_t preserved ) const
	{
		return memory.as< u8 >() + preserved;
	}

	u8* release( void )
	{
		return memory.release< u8 >();
	}

private:
	aligned_ptr< static_alignment > memory;
};

//--- shared_allocator -----------------------------------------------------------------------------
class shared_allocator
	: public buffer_allocator
{
public:
	shared_allocator( void )
		: memory( 0 )
	{
	}

	virtual bool empty( void )
	{
		return !memory;
	}

	virtual void allocate( up_t size )
	{
		scoped_ptr< shared_memory >
			( new shared_memory( unique_name(), shared_memory::create, size ) ).swap( memory );
	}

	virtual u8* get( up_t ) const
	{
		return memory->as< u8 >();
	}

	void deallocate( void )
	{
		scoped_ptr< shared_memory >( 0 ).swap( memory );
	}

	void set( const std::string& name_ )
	{
		scoped_ptr< shared_memory >( new shared_memory( name_ ) ).swap( memory );
	}

	std::string name( void ) const
	{
		return memory->name();
	}

private:
	scoped_ptr< shared_memory > memory;
};

//--- io_buffer ------------------------------------------------------------------------------------
class io_buffer
	: private noncopyable
{
public:
	io_buffer( u8* data_, up_t size_, buffer_allocator& allocator_ )
		: preserved( 0 ), internal( true ), data( data_ ), size( size_ ), allocator( allocator_ )
	{
	}

	void preserve( up_t request )
	{
		preserved = request;
	}

	void external( void )
	{
		internal = false;
	}

	void reset( void )
	{
		preserved = 0;
		internal = true;
	}

	void allocate( up_t request )
	{
		if ( request + preserved <= size )
			internal = true;
		else
		{
			allocator.allocate( request );
			internal = false;
		}
	}

	u8* get( void ) const
	{
		return internal ? data + preserved : allocator.get( preserved );
	}

	bool is_internal( void ) const
	{
		return internal;
	}

private:
	up_t preserved;
	bool internal;

	u8* data;
	up_t size;

	buffer_allocator& allocator;
};

OOE_NAMESPACE_END( ( ooe )( ipc ) )

#endif	// OOE_FOUNDATION_IPC_IO_BUFFER_HPP
