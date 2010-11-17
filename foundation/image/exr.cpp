/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <ImfChannelList.h>
#include <ImfIO.h>
#include <ImfInputFile.h>

#include "foundation/image/exr.hpp"
#include "foundation/io/memory.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( Imf ) )

//--- mapped ---------------------------------------------------------------------------------------
class mapped
    : public IStream
{
public:
    mapped( void*, up_t );
    virtual ~mapped( void ) {}

    virtual bool isMemoryMapped( void ) const;
    virtual c8* readMemoryMapped( s32 );
    virtual bool read( c8*, s32 );
    virtual u64 tellg( void );
    virtual void seekg( u64 );

private:
    c8* const data;
    const up_t size;
    u64 position;
};

mapped::mapped( void* data_, up_t size_ )
    : IStream( "" ), data( static_cast< c8* >( data_ ) ), size( size_ ), position( 0 )
{
}

bool mapped::isMemoryMapped( void ) const
{
    return true;
}

c8* mapped::readMemoryMapped( s32 length )
{
    if ( position >= size )
        throw error::runtime( "exr: " ) << "Unexpected end of file";
    else if ( position + length > size )
        throw error::runtime( "exr: " ) << "Reading past end of file";

    c8* pointer = data + position;
    position += length;
    return pointer;
}

bool mapped::read( c8* buffer, s32 length )
{
    if ( position >= size )
        throw error::runtime( "exr: " ) << "Unexpected end of file";
    else if ( position + length > size )
        throw error::runtime( "exr: " ) << "Reading past end of file";

    c8* offset = data + position;
    std::memcpy( buffer, offset, length );
    position += length;
    return position < size;
}

u64 mapped::tellg( void )
{
    return position;
}

void mapped::seekg( u64 position_ )
{
    position = position_;
}

//--- format ---------------------------------------------------------------------------------------
image::type exr_image_type( const ChannelList& channels, PixelType type )
{
    std::string format;
    format.reserve( 4 );

    for ( ChannelList::ConstIterator i = channels.begin(), end = channels.end(); i != end; ++i )
    {
        if ( i.channel().type != type )
            throw error::runtime( "exr: " ) << "Channels must be the same type";

        format += i.name();

        if ( format.size() > 4 )
            throw error::runtime( "exr: " ) << "Too many channels";
    }

    if ( format == "BGR" )
        return type == HALF ? image::rgb_f16 : image::rgb_f32;
    else if ( format == "ABGR" )
        return type == HALF ? image::rgba_f16 : image::rgba_f32;
    else if ( format == "A" )
        return type == HALF ? image::a_f16 : image::a_f32;
    else if ( format == "Y" )
        return type == HALF ? image::y_f16 : image::y_f32;
    else if ( format == "AY" )
        return type == HALF ? image::ya_f16 : image::ya_f32;
    else
        throw error::runtime( "exr: " ) << "Unknown format: " << format;
}

OOE_ANONYMOUS_END( ( ooe )( Imf ) )

OOE_NAMESPACE_BEGIN( ( ooe )( exr ) )

//--- exr ------------------------------------------------------------------------------------------
uncompressed_image decode( const descriptor& desc )
{
    memory memory( desc );
    return decode( memory.get(), memory.size() );
}

uncompressed_image decode( const void* data, up_t size )
{
    mapped mapped( const_cast< void* >( data ), size );
    InputFile file( mapped );
    const Header& header = file.header();
    const Imath::Box2i window = header.dataWindow();
    const ChannelList& channels = header.channels();
    const PixelType type = channels.begin().channel().type;

    if ( type != HALF || type != FLOAT )
        throw error::runtime( "exr: " ) << "Unsupported channel type";

    uncompressed_image image( window.max.x - window.min.x + 1, window.max.y - window.min.y + 1,
        exr_image_type( channels, type ) );

    up_t x_stride = image.pixel_size();
    up_t y_stride = image.row_size();
    c8* pointer = image.as< c8 >() -
        ( window.min.x + window.min.y * image.width ) * x_stride;

    FrameBuffer frame;
    up_t offset = 0;

    for ( ChannelList::ConstIterator i = channels.begin(), end = channels.end(); i != end; ++i )
    {
        frame.insert( i.name(), Slice( type, pointer + offset, x_stride, y_stride ) );
        offset += image.channel_size();
    }

    file.setFrameBuffer( frame );
    file.readPixels( window.min.y, window.max.y );
    return image;
}

OOE_NAMESPACE_END( ( ooe )( exr ) )
