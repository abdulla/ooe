/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_FILE_HPP
#define OOE_FOUNDATION_IO_FILE_HPP

#include "foundation/io/descriptor.hpp"
#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- file -----------------------------------------------------------------------------------------
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

    up_t tell( void ) const;
    void seek( sp_t, seek_type = current );
};

template< typename t >
    typename enable_if< is_string< t >, file >::type& operator <<( file& file, t string )
{
    file.write( string_data( string ), string_size( string ) );
    return file;
}

//--- make_pipe ------------------------------------------------------------------------------------
typedef tuple< file, file > file_pair;
file_pair make_pipe( void ) OOE_VISIBLE;

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_IO_FILE_HPP
