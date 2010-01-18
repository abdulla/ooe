/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/executable/checkpoint.hpp"
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
	: desc( 0 ), vector()
{
}

void checkpoint::insert( const function_type& function )
{
	vector.push_back( function );
}

void checkpoint::update( void )
{
	file pipe = link( desc );
	fork_io fork;

	// not child, continue
	if ( !fork.is_child() )
		return;

	desc_ptr( 0 ).swap( desc );
	pid_t parent_pid = getppid();
	u8 dummy;
	pipe.read( &dummy, sizeof( dummy ) );

	// parent is still alive, exit
	if ( getppid() == parent_pid )
		fork_io::exit( true );

	// run all post-recovery actions
	for ( vector_type::const_iterator i = vector.begin(), end = vector.end(); i != end; ++i )
		( *i )();

	// create checkpoint for this process
	update();
}

OOE_NAMESPACE_END( ( ooe ) )
