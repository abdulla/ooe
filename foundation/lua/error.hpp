/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_LUA_ERROR_HPP
#define OOE_FOUNDATION_LUA_ERROR_HPP

#include "foundation/lua/stack.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
	namespace error
	{
		struct lua;
	}

	struct OOE_VISIBLE error::lua
		: virtual public runtime
	{
		lua( ooe::lua::stack& stack )
			: runtime( "lua: " )
		{
			const c8* string_ = stack.tolstring( -1, 0 );

			// remove lua prefix from exception translation
			if ( std::equal( string_, string_ + 5, "lua: " ) )
				string_ += 5;

			*this << string_;
			stack.pop( 1 );
		}

		lua( const std::string& string_ )
			: runtime( "lua: " )
		{
			*this << string_;
		}

		virtual ~lua( void ) throw()
		{
		}
	};
}

#define OOE_ERROR( statement )\
do\
{\
	try\
	{\
		statement;\
	}\
	catch ( error::runtime& error )\
	{\
		std::string string;\
		string << error.what() << "\n\nVM stack trace:" << error.where();\
		traits< const std::string& >::push( stack, string );\
		stack.error();\
	}\
	catch ( std::exception& error )\
	{\
		traits< const c8* >::push( stack, error.what() );\
		stack.error();\
	}\
	catch ( ... )\
	{\
		traits< const c8* >::push( stack, "An unknown exception was thrown" );\
		stack.error();\
	}\
}\
while ( false )

#endif	// OOE_FOUNDATION_LUA_ERROR_HPP
