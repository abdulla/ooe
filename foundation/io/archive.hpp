/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_ARCHIVE_HPP
#define OOE_FOUNDATION_IO_ARCHIVE_HPP

#include <boost/iterator/iterator_facade.hpp>

#include "foundation/io/memory.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- archive_file ---------------------------------------------------------------------------------
class archive_file
{
public:
    ~archive_file( void ) OOE_VISIBLE;

    const u8* get( void ) const OOE_VISIBLE;
    up_t size( void ) const OOE_VISIBLE;

private:
    bool inflated;
    const u8* data;
    const up_t size_;

    archive_file( const u8*, up_t, u32 );
    archive_file( const u8*, up_t, up_t, u32 );

    friend class archive;
};

//--- archive_iterator -----------------------------------------------------------------------------
class archive_iterator
    : public boost::iterator_facade< archive_iterator, std::string, boost::forward_traversal_tag,
    std::string >
{
private:
    const void* entry;

    explicit archive_iterator( const void* );

    void increment( void ) OOE_VISIBLE;
    bool equal( const archive_iterator& ) const OOE_VISIBLE;
    std::string dereference( void ) const OOE_VISIBLE;

    friend class archive;
    friend class boost::iterator_core_access;
};

//--- archive --------------------------------------------------------------------------------------
class OOE_VISIBLE archive
{
public:
    typedef archive_iterator iterator;
    typedef archive_file file;

    archive( const descriptor& );

    iterator begin( void ) const;
    iterator end( void ) const;

    file open( iterator ) const;
    file open( const std::string& ) const;

private:
    ooe::memory memory;
    const void* central_directory;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_IO_ARCHIVE_HPP
