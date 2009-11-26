/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_JAVASCRIPT_VM_HPP
#define OOE_COMPONENT_JAVASCRIPT_VM_HPP

#include <v8.h>

#include "foundation/utility/macro.hpp"
#include "foundation/utility/string.hpp"

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
		vm( void );
		~vm( void );

		void collect( void );

		up_t size( void ) const;
		std::string version( void ) const;

	private:
		v8::Persistent< v8::Context > context;
	};
}

#endif	// OOE_COMPONENT_JAVASCRIPT_VM_HPP
