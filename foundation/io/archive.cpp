/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <zlib.h>

#include "foundation/io/archive.hpp"
#include "foundation/io/error.hpp"
#include "foundation/utility/binary.hpp"
#include "foundation/utility/miscellany.hpp"
#include "foundation/utility/scoped.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

enum method_type
{
    store,
    flate = 8
};

struct OOE_PACKED directory_header
{
    static const u32 signature = 0x06054b50;

    u32 id;
    u16 disk_number;
    u16 disk_length;    // number of disks containing the central directory
    u16 entries_number; // number of entries in the central directory of this disk
    u16 entries_length; // number of entries in the entire central directory
    u32 size;           // size of the central directory
    u32 offset;         // offset of the start of the central directory on the disk containing it
    u16 comment_length;
};

struct OOE_PACKED entry_header
{
    static const u32 signature = 0x02014b50;

    u32 id;
    u16 used;           // version used to create entry
    u16 need;           // version needed to extract entry
    u16 flags;
    u16 method;         // should be flate
    u16 time;
    u16 date;
    u32 crc;
    u32 size_deflated;
    u32 size_inflated;
    u16 name_length;
    u16 extra_length;
    u16 comment_length;
    u16 disk;           // the disk where the file begins
    u16 flags_internal;
    u32 flags_external;
    u32 offset;         // offset to file header
};

struct file_header
{
    static const u32 signature = 0x04034b50;

    u32 id;
    u16 version;
    u16 flags;
    u16 method;         // should be flate
    u16 time;
    u16 date;
    u32 crc;
    u32 size_deflated;
    u32 size_inflated;
    u16 name_length;    // length of file name field
    u16 extra_length;   // length of extra field
} OOE_PACKED;

void check_crc( const u8* data, up_t size, u32 file_crc )
{
    u32 data_crc = crc32( 0, data, size );

    if ( data_crc != file_crc )
        throw error::io( "archive_file: " ) <<
            "Cyclic redundancy check mismatch, " << hex( data_crc ) << " != " << hex( file_crc );
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- archive_file ---------------------------------------------------------------------------------
archive_file::archive_file( const u8* data_, up_t size_inflated, u32 file_crc )
    : inflated( false ), data( data_ ), size_( size_inflated )
{
    check_crc( data, size_, file_crc );
}

archive_file::
    archive_file( const u8* deflated, up_t size_deflated, up_t size_inflated, u32 file_crc )
    : inflated( true ), data(), size_( size_inflated )
{
    z_stream stream;
    std::memset( &stream, 0, sizeof( stream ) );

    // -MAX_WBITS says that there is no zlib header
    s32 status = inflateInit2( &stream, -MAX_WBITS );

    if ( status != Z_OK )
        throw error::io( "archive_file: " ) << "Unable to initialise zlib: " << zError( status );

    scoped< s32 ( z_stream* ) > scoped( inflateEnd, &stream );
    scoped_array< u8 > output( new u8[ size_inflated ] );

    stream.next_in = const_cast< u8* >( deflated );
    stream.avail_in = size_deflated;
    stream.next_out = output;
    stream.avail_out = size_inflated;
    status = inflate( &stream, Z_FINISH );

    if ( status != Z_STREAM_END )
        throw error::io( "archive_file: " ) << "Unable to inflate data: " << zError( status );

    check_crc( output, size_inflated, file_crc );
    data = output.release();
}

archive_file::~archive_file( void )
{
    if ( inflated )
        delete[] data;
}

const u8* archive_file::get( void ) const
{
    return data;
}

up_t archive_file::size( void ) const
{
    return size_;
}

//--- archive_iterator -----------------------------------------------------------------------------
archive_iterator::archive_iterator( const void* entry_ )
    : entry( entry_ )
{
}

void archive_iterator::increment( void )
{
    const entry_header* e = static_cast< const entry_header* >( entry );
    up_t size = sizeof( *e ) + little_endian( e->name_length ) + little_endian( e->extra_length ) +
        little_endian( e->comment_length );
    e = add< entry_header >( e, size );
    entry = little_endian( e->id ) == entry_header::signature ? e : 0;
}

bool archive_iterator::equal( const archive_iterator& other ) const
{
    return entry == other.entry;
}

std::string archive_iterator::dereference( void ) const
{
    const entry_header* e = static_cast< const entry_header* >( entry );
    return std::string( add< c8 >( e, sizeof( *e ) ), little_endian( e->name_length ) );
}

//--- archive --------------------------------------------------------------------------------------
archive::archive( const descriptor& desc )
    : memory( desc ), central_directory()
{
    if ( little_endian( memory.as< file_header >()->id ) != file_header::signature )
        throw error::io( "archive: " ) << "Descriptor is not a ZIP archive";

    const directory_header* sentinel = memory.as< directory_header >();
    const directory_header* directory =
        add< directory_header >( sentinel, memory.size() - sizeof( *directory ) );

    while ( directory >= sentinel && little_endian( directory->id ) != directory_header::signature )
        directory = add< directory_header >( directory, -1 );

    if ( directory == sentinel )
        throw error::io( "archive: " ) << "Unable to find central directory entry in ZIP archive";
    else if ( directory->disk_number != 0 || directory->disk_length != 0 ||
        directory->entries_number != directory->entries_length )
        throw error::io( "archive: " ) << "Multi-part ZIP archive not supported";

    const entry_header* entry =
        add< entry_header >( memory.get(), little_endian( directory->offset ) );

    if ( little_endian( entry->id ) != entry_header::signature )
        throw error::io( "archive: " ) << "Invalid central directory entry in ZIP archive";

    central_directory = entry;
}

archive::iterator archive::begin( void ) const
{
    return iterator( central_directory );
}

archive::iterator archive::end( void ) const
{
    return iterator( 0 );
}

archive::file archive::open( iterator i ) const
{
    const entry_header& entry = *static_cast< const entry_header* >( i.entry );
    const file_header& file_ = *add< file_header >( memory.get(), little_endian( entry.offset ) );
    const c8* entry_name = add< c8 >( &entry, sizeof( entry ) );
    const c8* file_name = add< c8 >( &file_, sizeof( file_ ) );
    up_t size = little_endian( file_.name_length );

    if ( little_endian( file_.id ) != file_header::signature )
        throw error::io( "archive: " ) << "Invalid file entry in ZIP archive";
    else if ( entry.name_length != file_.name_length || std::memcmp( entry_name, file_name, size ) )
        throw error::io( "archive: " ) << "Name mismatch: " << std::string( entry_name, size ) <<
            " != " << std::string( file_name, size );

    const u8* data = add< u8 >( file_name, size + little_endian( file_.extra_length ) );
    up_t inflated = little_endian( file_.size_inflated );
    u32 crc = little_endian( file_.crc );

    if ( little_endian( file_.method ) == store )
        return file( data, inflated, crc );
    else
        return file( data, little_endian( file_.size_deflated ), inflated, crc );
}

archive::file archive::open( const std::string& path ) const
{
    for ( iterator i = begin(), end_ = end(); i != end_; ++i )
    {
        if ( *i == path )
            return open( i );
    }

    throw error::io( "archive: " ) << "Unable to open \"" << path << "\", file not found";
}

OOE_NAMESPACE_END( ( ooe ) )
