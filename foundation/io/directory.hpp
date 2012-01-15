/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_DIRECTORY_HPP
#define OOE_FOUNDATION_IO_DIRECTORY_HPP

#include <dirent.h>

#include <boost/iterator/iterator_facade.hpp>

#include "foundation/io/descriptor.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

DIR* directory_open( const ooe::descriptor& );

OOE_NAMESPACE_END( ( ooe )( platform ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- directory_iterator ---------------------------------------------------------------------------
class directory_iterator
    : public boost::iterator_facade< directory_iterator, std::string,
    boost::single_pass_traversal_tag, std::string >
{
private:
    DIR* dir;
    dirent* entry;

    explicit directory_iterator( DIR*, dirent* );

    void increment( void ) OOE_VISIBLE;
    bool equal( const directory_iterator& ) const OOE_VISIBLE;
    std::string dereference( void ) const OOE_VISIBLE;

    friend class directory;
    friend class boost::iterator_core_access;
};

//--- directory ------------------------------------------------------------------------------------
class OOE_VISIBLE directory
    : private noncopyable
{
public:
    typedef directory_iterator iterator;

    directory( const descriptor& );
    ~directory( void );

    iterator begin( void ) const;
    iterator end( void ) const;

private:
    DIR* dir;
};

//--------------------------------------------------------------------------------------------------
bool exists( const std::string& ) OOE_VISIBLE;
void erase( const std::string& ) OOE_VISIBLE;
std::string canonical_path( const std::string& ) OOE_VISIBLE;
void make_directory( const std::string& ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_IO_DIRECTORY_HPP
