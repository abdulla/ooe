/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_DIRECTORY_HPP
#define OOE_FOUNDATION_IO_DIRECTORY_HPP

#include "foundation/executable/environment.hpp"
#include "foundation/io/descriptor.hpp"

namespace ooe
{
	class OOE_VISIBLE directory
		: private descriptor
	{
	public:
		directory( const descriptor& );

		bool operator ++( void );
		std::string operator *( void ) const;

	private:
		up_t last;
		up_t next;
		up_t limit;
		c8 buffer[ executable::static_page_size ];
	};
}

#endif	// OOE_FOUNDATION_IO_DIRECTORY_HPP
