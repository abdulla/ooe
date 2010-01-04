/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/io/directory.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

//--- directory_open -------------------------------------------------------------------------------
DIR* directory_open( const ooe::descriptor& desc )
{
	return fdopendir( dup( desc.get() ) );
}

OOE_NAMESPACE_END( ( ooe )( platform ) )
