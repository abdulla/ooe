/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_DIRECTORY_HPP
#define OOE_FOUNDATION_IO_DIRECTORY_HPP

#include <boost/iterator/iterator_facade.hpp>

#include "foundation/executable/environment.hpp"
#include "foundation/io/descriptor.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- directory ------------------------------------------------------------------------------------
class OOE_VISIBLE directory
	: private noncopyable, public descriptor
{
public:
	class iterator;

	directory( const descriptor& );

	iterator begin( void );
	iterator end( void );

private:
	up_t prev;
	up_t next;
	up_t limit;
	c8 data[ executable::static_page_size ];
};

//--- directory::iterator --------------------------------------------------------------------------
class directory::iterator
	: public boost::iterator_facade< iterator, std::string, boost::forward_traversal_tag,
	std::string >
{
private:
	directory* dir;

	iterator( directory* );

	void increment( void ) OOE_VISIBLE;
	bool equal( const iterator& ) const OOE_VISIBLE;
	std::string dereference( void ) const OOE_VISIBLE;

	friend class directory;
	friend class boost::iterator_core_access;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_FOUNDATION_IO_DIRECTORY_HPP
