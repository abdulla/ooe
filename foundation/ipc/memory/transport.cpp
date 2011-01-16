/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/environment.hpp"
#include "foundation/ipc/memory/transport.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( ipc )( memory ) )

//--- transport ------------------------------------------------------------------------------------
bool transport::in_guard( const void* pointer ) const
{
    const u8* begin = memory.as< u8 >() + executable::static_page_size;
    const u8* end = begin + executable::static_page_size;
    return pointer >= begin && pointer < end;
}

OOE_NAMESPACE_END( ( ooe )( ipc )( memory ) )
