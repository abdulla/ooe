/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_IO_BUFFER_HPP
#define OOE_FOUNDATION_IPC_IO_BUFFER_HPP

#include "foundation/executable/environment.hpp"
#include "foundation/ipc/name.hpp"
#include "foundation/ipc/shared_memory.hpp"
#include "foundation/utility/arithmetic.hpp"

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
		return !memory;
	}

	virtual void allocate( up_t size )
	{
		scoped_array< u8 >( new u8[ size ] ).swap( memory );
	}

	virtual u8* get( up_t preserved ) const
	{
		return memory + preserved;
	}

	u8* release( void )
	{
		return memory.release();
	}

private:
	scoped_array< u8 > memory;
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
		size = round_up< executable::static_page_size >( size + executable::static_page_size );
		scoped_ptr< shared_memory >
			( new shared_memory( unique_name(), shared_memory::create, size ) ).swap( memory );

		ooe::memory::region
			window( size - executable::static_page_size, executable::static_page_size );
		memory->protect( ooe::memory::none, window );
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

		ooe::memory::region
			window( memory->size() - executable::static_page_size, executable::static_page_size );
		memory->protect( ooe::memory::none, window );
	}

	std::string name( void ) const
	{
		return memory->name();
	}

	bool in_canary( const void* pointer ) const
	{
		const u8* end = memory->as< u8 >() + memory->size();
		const u8* begin = end - executable::static_page_size;
		return pointer >= begin && pointer < end;
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
