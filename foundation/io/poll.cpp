/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <cerrno>

#include "foundation/io/error.hpp"
#include "foundation/io/poll.hpp"

namespace ooe
{
	void poll::insert( const descriptor& desc, const function_type& function )
	{
		data.push_back( datum_tuple( desc, function ) );
		pollfd event = { desc.get(), POLLIN, 0 };
		fds.push_back( event );
	}

	void poll::wait( void ) const
	{
		if ( ::poll( &fds[ 0 ], fds.size(), -1 ) == -1 )
			throw error::io( "poll: " ) << "Unable to poll descriptors: " << error::number( errno );

		for ( up_t i = 0, end = fds.size(); i != end; ++i )
		{
			u16 returned = fds[ i ].revents;
			const datum_tuple& datum = data[ i ];

			if ( !returned )
				continue;
			else if ( returned & POLLERR )
				datum._1( datum._0, error );
			else if ( returned & POLLHUP )
				datum._1( datum._0, disconnect );
			else if ( returned & POLLIN )
				datum._1( datum._0, input );
			else
				throw error::io( "poll: " ) <<
					"Unknown poll event 0x" << hex( returned ) << " on descriptor " << fds[ i ].fd;
		}
	}
}
