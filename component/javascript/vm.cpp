/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "component/javascript/vm.hpp"

namespace ooe
{
//--- javascript::vm -----------------------------------------------------------
	javascript::vm::vm( void )
		: context( v8::Context::New() )
	{
	}

	javascript::vm::~vm( void )
	{
		context.Dispose();
	}

	void javascript::vm::collect( void )
	{
		v8::V8::LowMemoryNotification();
	}

	up_t javascript::vm::size( void ) const
	{
		v8::HeapStatistics heap;
		v8::V8::GetHeapStatistics( &heap );
		return heap.total_heap_size();
	}

	std::string javascript::vm::version( void ) const
	{
		return v8::V8::GetVersion();
	}
}
