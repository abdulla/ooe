/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_VFS_HPP
#define OOE_FOUNDATION_IO_VFS_HPP

#include <list>

#include "foundation/io/descriptor.hpp"

namespace ooe
{
	class OOE_VISIBLE vfs
		: private noncopyable
	{
	public:
		typedef tuple< const std::string, const std::string > tuple_type;
		typedef std::list< tuple_type > list_type;

		vfs( void );

		void insert( const std::string&, const std::string& );
		bool empty( void ) const;

		descriptor operator []( const std::string& ) const;

	private:
		mutable list_type list;
	};
}

#endif	// OOE_FOUNDATION_IO_VFS_HPP
