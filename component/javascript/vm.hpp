/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_JAVASCRIPT_VM_HPP
#define OOE_COMPONENT_JAVASCRIPT_VM_HPP

#include <v8.h>

#include "foundation/io/descriptor.hpp"

namespace ooe
{
	namespace javascript
	{
		class vm;
	}

//--- javascript::vm -----------------------------------------------------------
	class OOE_VISIBLE javascript::vm
	{
	public:
		typedef void ( * function_type )( v8::Handle< v8::Object >, const std::string& );

		vm( void );
		~vm( void );

		void load( const std::string&, const descriptor& );
		void collect( void );

		up_t size( void ) const;
		std::string version( void ) const;

		void setup( function_type, const std::string& );

	private:
		v8::Persistent< v8::Context > context;
	};
}

#endif	// OOE_COMPONENT_JAVASCRIPT_VM_HPP
