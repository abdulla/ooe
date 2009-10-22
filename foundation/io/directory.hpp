/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_DIRECTORY_HPP
#define OOE_FOUNDATION_IO_DIRECTORY_HPP

#include "foundation/executable/environment.hpp"
#include "foundation/io/descriptor.hpp"

namespace ooe
{
	struct directory_id
	{
		up_t last;
		up_t next;
		up_t limit;
		c8 buffer[ executable::static_page_size ];

		directory_id( void );
	};

	class OOE_VISIBLE directory
		: public descriptor
	{
	public:
		directory( const descriptor& );

		bool operator ++( void );
		std::string operator *( void ) const;

	private:
		shared_ptr< directory_id > id;
	};
}

#endif	// OOE_FOUNDATION_IO_DIRECTORY_HPP
