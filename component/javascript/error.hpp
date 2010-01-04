/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_JAVASCRIPT_ERROR_HPP
#define OOE_COMPONENT_JAVASCRIPT_ERROR_HPP

#include <v8.h>

#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"

namespace ooe
{
	namespace error
	{
		struct javascript;
	}

	struct OOE_VISIBLE error::javascript
		: virtual public runtime
	{
		javascript( const v8::TryCatch& try_catch )
			: runtime( "javascript: " )
		{
			v8::Handle< v8::Message > message = try_catch.Message();

			v8::Handle< v8::String > description = message->Get();
			v8::Handle< v8::String > source = message->GetSourceLine();
			v8::Handle< v8::Value > name = message->GetScriptResourceName();
			v8::String::AsciiValue ascii_description( description );
			v8::String::AsciiValue ascii_source( source );
			v8::String::AsciiValue ascii_name( name );

			s32 line = message->GetLineNumber();
			s32 i = message->GetStartColumn();
			s32 extent = message->GetEndColumn() - i - 1;

			using ooe::operator <<;
			*this << *ascii_name << ':' << line << ": " << *ascii_description << "\n    " <<
				*ascii_source << "\n    " << rep( ' ', i ) << '^' << rep( '-', extent ) << '^';
		}

		javascript( void )
			: runtime( "javascript: " )
		{
		}

		virtual ~javascript( void ) throw()
		{
		}
	};
}

#endif	// OOE_COMPONENT_JAVASCRIPT_ERROR_HPP
