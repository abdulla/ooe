/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "component/registry/checkpoint.hpp"
#include "foundation/executable/fork_io.hpp"
#include "foundation/io/file.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

file link( checkpoint::desc_ptr& desc )
{
	file_pair pair = make_pipe();
	checkpoint::desc_ptr( new descriptor( pair._1 ) ).swap( desc );
	return pair._0;
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- checkpoint -----------------------------------------------------------------------------------
checkpoint::checkpoint( void )
	: desc( 0 )
{
}

bool checkpoint::update( void )
{
	file pipe = link( desc );
	fork_io fork;

	// not child, continue
	if ( !fork.is_child() )
		return true;

	desc_ptr( 0 ).swap( desc );
	pid_t parent_pid = getppid();
	u8 dummy;
	pipe.read( &dummy, sizeof( dummy ) );

	// parent is still alive, exit
	if ( getppid() == parent_pid )
		fork_io::exit( true );

	return false;
}

OOE_NAMESPACE_END( ( ooe ) )
