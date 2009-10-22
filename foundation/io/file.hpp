/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_FILE_HPP
#define OOE_FOUNDATION_IO_FILE_HPP

#include "foundation/io/descriptor.hpp"

namespace ooe
{
	struct OOE_VISIBLE file
		: public descriptor
	{
		enum seek_type
		{
			current,
			begin,
			end
		};

		file( const descriptor& );

		void sync( void ) const;

		up_t read( void*, up_t );
		up_t write( const void*, up_t );

		void seek( sp_t, seek_type = current );
		up_t tell( void ) const;
	};
}

#endif	// OOE_FOUNDATION_IO_FILE_HPP
