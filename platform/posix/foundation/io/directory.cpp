/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/io/directory.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

//--- directory_open -------------------------------------------------------------------------------
DIR* directory_open( const ooe::descriptor& desc )
{
	return fdopendir( dup( desc.get() ) );
}
