/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <ImfChannelList.h>
#include <ImfIO.h>
#include <ImfInputFile.h>

#include "foundation/image/exr.hpp"
#include "foundation/io/file.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe )( Imf ) )

//--- ExrFile --------------------------------------------------------------------------------------
class ExrFile
    : public IStream
{
public:
    ExrFile( const descriptor& desc )
        : IStream( "" ), file( desc )
    {
    }

    virtual bool read( c8* buffer, s32 bytes )
    {
        return file.read( buffer, bytes ) == up_t( bytes );
    }

    virtual u64 tellg( void )
    {
        return file.tell();
    }

    virtual void seekg( u64 position )
    {
        file.seek( position, file::begin );
    }

private:
    ooe::file file;
};

//--- format ---------------------------------------------------------------------------------------
image_format::type exr_image_format( const ChannelList& channels, PixelType type )
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
        return type == HALF ? image_format::rgb_f16 : image_format::rgb_f32;
    else if ( format == "ABGR" )
        return type == HALF ? image_format::rgba_f16 : image_format::rgba_f32;
    else if ( format == "A" )
        return type == HALF ? image_format::a_f16 : image_format::a_f32;
    else if ( format == "Y" )
        return type == HALF ? image_format::y_f16 : image_format::y_f32;
    else if ( format == "AY" )
        return type == HALF ? image_format::ya_f16 : image_format::ya_f32;
    else
        throw error::runtime( "exr: " ) << "Unknown format: " << format;
}

OOE_ANONYMOUS_END( ( ooe )( Imf ) )

OOE_NAMESPACE_BEGIN( ( ooe )( exr ) )

//--- exr ------------------------------------------------------------------------------------------
image decode( const descriptor& desc )
{
    ExrFile exr_file( desc );
    InputFile input_file( exr_file );
    const Header& header = input_file.header();
    const Imath::Box2i window = header.dataWindow();
    const ChannelList& channels = header.channels();
    const PixelType type = channels.begin().channel().type;

    if ( type != HALF || type != FLOAT )
        throw error::runtime( "exr: " ) << "Unsupported subpixel type";

    image image( window.max.x - window.min.x + 1, window.max.y - window.min.y + 1,
        exr_image_format( channels, type ) );
    up_t x_stride = pixel_size( image );
    up_t y_stride = row_size( image );
    c8* pointer = image.as< c8 >() - ( window.min.x + window.min.y * image.width ) * x_stride;
    FrameBuffer frame;
    up_t offset = 0;

    for ( ChannelList::ConstIterator i = channels.begin(), end = channels.end(); i != end; ++i )
    {
        frame.insert( i.name(), Slice( type, pointer + offset, x_stride, y_stride ) );
        offset += subpixel_size( image );
    }

    input_file.setFrameBuffer( frame );
    input_file.readPixels( window.min.y, window.max.y );
    return image;
}

OOE_NAMESPACE_END( ( ooe )( exr ) )
