/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_GENERAL_SIGHT_FORWARD_HPP
#define OOE_FOUNDATION_GENERAL_SIGHT_FORWARD_HPP

#include "foundation/io/poll.hpp"
#include "foundation/parallel/task.hpp"
#include "foundation/utility/atom.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

class sight
{
protected:
	typedef function< void ( const u8* ) > call_type;

	sight( const call_type&, u16 width, u16 height );
	~sight( void ) OOE_VISIBLE;

private:
	atom< u32 > state;
	unique_task< void ( call_type, u16, u16 ) > task;

	void status( const descriptor&, poll::type );
	void main( call_type, u16, u16 );
};

OOE_NAMESPACE_END( ( ooe )( platform ) )

#endif	// OOE_FOUNDATION_GENERAL_SIGHT_FORWARD_HPP
