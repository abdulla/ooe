/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <Foundation/Foundation.h>
#include <mach-o/dyld.h>

#include "foundation/executable/program.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

class autorelease
{
public:
    autorelease( void )
        : pool( [ [ NSAutoreleasePool alloc ] init ] )
    {
        if ( !pool )
            throw error::runtime( "autorelease: " ) << "Unable to initialise";
    }

    ~autorelease( void )
    {
        [ pool release ];
    }

private:
    NSAutoreleasePool* pool;
};

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- platform -------------------------------------------------------------------------------------
bool platform::launch( executable::launch_type launch,
    const std::string& root, const std::string& name, s32 argc, c8** argv )
{
    autorelease autorelease;
    bool status = false;

    @try
    {
        status = launch( root, name, argc, argv );
    }
    @catch ( NSException* error )
    {
        throw error::runtime( error.name.UTF8String ) << ": " << error.reason.UTF8String;
    }

    return status;
}

//--- executable -----------------------------------------------------------------------------------
bool executable::path( c8* buffer, up_t size )
{
    u32 length = size;
    return !_NSGetExecutablePath( buffer, &length );
}

OOE_NAMESPACE_END( ( ooe ) )
