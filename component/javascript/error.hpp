/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_JAVASCRIPT_ERROR_HPP
#define OOE_COMPONENT_JAVASCRIPT_ERROR_HPP

#include <v8.h>

#include "foundation/utility/convert.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( error ) )

struct OOE_VISIBLE javascript
    : virtual public runtime
{
    javascript( const v8::TryCatch& try_catch )
        : runtime( "javascript: " )
    {
        v8::Handle< v8::Message > message = try_catch.Message();

        v8::Handle< v8::String > description = message->Get();
        v8::Handle< v8::String > source = message->GetSourceLine();
        v8::Handle< v8::Value > name = message->GetScriptResourceName();
        v8::String::Utf8Value utf8_description( description );
        v8::String::Utf8Value utf8_source( source );
        v8::String::Utf8Value utf8_name( name );

        s32 line = message->GetLineNumber();
        s32 i = message->GetStartColumn();
        s32 extent = message->GetEndColumn() - i - 1;

        using ooe::operator <<;
        *this << *utf8_name << ':' << line << ": " << *utf8_description <<
            "\n    " << *utf8_source <<
            "\n    " << rep( ' ', i ) << '^' << rep( '-', extent ) << '^';

        v8::Handle< v8::Value > trace = try_catch.StackTrace();

        if ( !trace.IsEmpty() )
        {
            v8::String::Utf8Value utf8_trace( trace );
            *this << "\n\nScript stack trace: " << *utf8_trace;
        }
    }

    javascript( void )
        : runtime( "javascript: " )
    {
    }

    virtual ~javascript( void ) throw()
    {
    }
};

OOE_NAMESPACE_END( ( ooe )( error ) )

#endif  // OOE_COMPONENT_JAVASCRIPT_ERROR_HPP
