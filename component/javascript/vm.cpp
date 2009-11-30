/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "component/javascript/error.hpp"
#include "component/javascript/vm.hpp"
#include "foundation/io/memory.hpp"

namespace
{
	using namespace ooe;

	class mapped
		: public v8::String::ExternalAsciiStringResource
	{
	public:
		mapped( const descriptor& );
		virtual ~mapped( void );

		virtual const c8* data( void ) const;
		virtual up_t length( void ) const;

	private:
		ooe::memory memory;
	};

	mapped::mapped( const descriptor& desc )
		: ExternalAsciiStringResource(), memory( desc )
	{
	}

	mapped::~mapped( void )
	{
	}

	const c8* mapped::data( void ) const
	{
		return memory.as< const c8 >();
	}

	up_t mapped::length( void ) const
	{
		return memory.size();
	}
}

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
		collect();
	}

	void javascript::vm::load( const std::string& name, const descriptor& desc )
	{
		v8::Context::Scope context_scope( context );
		v8::HandleScope handle_scope;
		v8::TryCatch try_catch;

		mapped mapped( desc );
		v8::Handle< v8::String > source = v8::String::NewExternal( &mapped );
		v8::Handle< v8::String > origin = v8::String::New( name.c_str(), name.size() );
		v8::Handle< v8::Script > script = v8::Script::Compile( source, origin );

		if ( try_catch.HasCaught() )
			throw error::javascript( try_catch );

		script->Run();

		if ( try_catch.HasCaught() )
			throw error::javascript( try_catch );
	}

	void javascript::vm::collect( void )
	{
		v8::V8::LowMemoryNotification();
	}

	up_t javascript::vm::size( void ) const
	{
		v8::HeapStatistics heap;
		v8::V8::GetHeapStatistics( &heap );
		return heap.used_heap_size();
	}

	std::string javascript::vm::version( void ) const
	{
		return v8::V8::GetVersion();
	}

	void javascript::vm::setup( function_type function, const std::string& path )
	{
		v8::Context::Scope context_scope( context );
		v8::HandleScope handle_scope;
		v8::TryCatch try_catch;

		function( context->Global(), path );

		if ( try_catch.HasCaught() )
			throw error::javascript( try_catch );
	}
}
