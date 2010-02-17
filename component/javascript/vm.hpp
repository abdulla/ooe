/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_JAVASCRIPT_VM_HPP
#define OOE_COMPONENT_JAVASCRIPT_VM_HPP

#include <v8.h>

#include "foundation/io/descriptor.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( javascript ) )

//--- vm -------------------------------------------------------------------------------------------
class OOE_VISIBLE vm
{
public:
	typedef void ( * function_type )( v8::Handle< v8::Object > );

	vm( void );
	~vm( void );

	void load( const std::string&, const descriptor& );
	void collect( void );

	up_t size( void ) const;
	std::string version( void ) const;

	void setup( function_type );

private:
	v8::Persistent< v8::Context > context;
};

OOE_NAMESPACE_END( ( ooe )( javascript ) )

#endif	// OOE_COMPONENT_JAVASCRIPT_VM_HPP
