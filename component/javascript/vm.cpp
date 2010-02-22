/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/javascript/error.hpp"
#include "component/javascript/vm.hpp"
#include "foundation/io/memory.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

//--- mapped ---------------------------------------------------------------------------------------
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

//--- object_template ------------------------------------------------------------------------------
v8::Handle< v8::ObjectTemplate >* object_template;

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( javascript ) )

//--- vm -------------------------------------------------------------------------------------------
vm::vm( void )
	: context( v8::Context::New() )
{
}

vm::~vm( void )
{
	context.Dispose();
	collect();
}

void vm::load( const std::string& name, const descriptor& desc )
{
	v8::Context::Scope context_scope( context );
	v8::HandleScope handle_scope;
	v8::TryCatch try_catch;

	v8::Handle< v8::ObjectTemplate > local_template = v8::ObjectTemplate::New();
	local_template->SetInternalFieldCount( 2 );
	object_template = &local_template;

	v8::Handle< v8::String > source = v8::String::NewExternal( new mapped( desc ) );
	v8::Handle< v8::String > origin = v8::String::New( name.data(), name.size() );
	v8::Handle< v8::Script > script = v8::Script::Compile( source, origin );

	if ( try_catch.HasCaught() )
		throw error::javascript( try_catch );

	script->Run();

	if ( try_catch.HasCaught() )
		throw error::javascript( try_catch );
}

void vm::collect( void )
{
	v8::V8::LowMemoryNotification();
	while ( !v8::V8::IdleNotification() ) {}
}

up_t vm::size( void ) const
{
	v8::HeapStatistics heap;
	v8::V8::GetHeapStatistics( &heap );
	return heap.used_heap_size();
}

std::string vm::version( void ) const
{
	return v8::V8::GetVersion();
}

void vm::setup( function_type function )
{
	v8::Context::Scope context_scope( context );
	v8::HandleScope handle_scope;
	v8::TryCatch try_catch;

	function( context->Global() );

	if ( try_catch.HasCaught() )
		throw error::javascript( try_catch );
}

v8::Handle< v8::Object > make_object( void* pointer, const std::type_info& type_info )
{
	v8::Handle< v8::Object > object = ( *object_template )->NewInstance();
	object->SetPointerInInternalField( 0, pointer );
	object->SetPointerInInternalField( 1, const_cast< std::type_info* >( &type_info ) );
	return object;
}

OOE_NAMESPACE_END( ( ooe )( javascript ) )
