/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <cerrno>
#include <vector>

#include <libv4lconvert.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>

#include "foundation/io/memory.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/scoped.hpp"
#include "foundation/visual/sight.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

void control( const descriptor& desc, up_t request, void* pointer )
{
    if ( ioctl( desc.get(), request, pointer ) == -1 )
        throw error::runtime( "sight: " ) << "Unable control device: " << error::number( errno );
}

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- sight ----------------------------------------------------------------------------------------
sight::sight( const call_type& call_, u16 width_, u16 height_ )
    : platform::sight( call_, width_, height_ )
{
}

image_format::type sight::format( void )
{
    return image_format::rgb_u8;
}

OOE_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

//--- sight ----------------------------------------------------------------------------------------
sight::sight( const call_type& call_, u16 width_, u16 height_ )
    : call( call_ ), width( width_ ), height( height_ ), state( true ),
    thread( "sight", make_function( *this, &sight::main ), 0 )
{
}

sight::~sight( void )
{
    state.exchange( false );
}

void sight::status( const descriptor&, poll::type type )
{
    if ( type != poll::input )
        state.exchange( false );
}

void sight::main( void* )
{
    descriptor desc( "/dev/video0", descriptor::read_write );

    if ( desc.type() != descriptor::character )
        throw error::runtime( "sight: " ) << "Descriptor is not a character device";

    v4l2_capability capability;
    control( desc, VIDIOC_QUERYCAP, &capability );

    if ( ~capability.capabilities & V4L2_CAP_VIDEO_CAPTURE )
        throw error::runtime( "sight: " ) << "Device is not capable of video capture";
    else if ( ~capability.capabilities & V4L2_CAP_STREAMING )
        throw error::runtime( "sight: " ) << "Device is not capable of streaming IO";

    /*v4l2_cropcap cropcap;
    std::memset( &cropcap, 0, sizeof( cropcap ) );
    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    control( desc, VIDIOC_CROPCAP, &cropcap );

    v4l2_crop crop;
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c = cropcap.defrect;
    control( desc, VIDIOC_S_CROP, &crop );*/

    v4l2_format source_format;
    source_format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    control( desc, VIDIOC_G_FMT, &source_format );

    source_format.fmt.pix.width = width;
    source_format.fmt.pix.height = height;
    control( desc, VIDIOC_S_FMT, &source_format );

    v4lconvert_data* data = v4lconvert_create( desc.get() );
    scoped< void ( v4lconvert_data* ) > scoped( v4lconvert_destroy, data );
    v4l2_format target_format = source_format;
    target_format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;

    up_t target_size = width * height * 3;
    scoped_array< u8 > target_data( new u8[ target_size ] );

    v4l2_requestbuffers requestbuffers;
    std::memset( &requestbuffers, 0, sizeof( requestbuffers ) );
    requestbuffers.count = 4;
    requestbuffers.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    requestbuffers.memory = V4L2_MEMORY_MMAP;
    control( desc, VIDIOC_REQBUFS, &requestbuffers );

    if ( requestbuffers.count < 2 )
        throw error::runtime( "sight: " ) << "Device does not have enough buffer memory";

    std::vector< memory > vector;
    vector.reserve( requestbuffers.count );

    for ( up_t i = 0; i != requestbuffers.count; ++i )
    {
        v4l2_buffer buffer;
        std::memset( &buffer, 0, sizeof( buffer ) );
        buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buffer.memory = V4L2_MEMORY_MMAP;
        buffer.index = i;
        control( desc, VIDIOC_QUERYBUF, &buffer );

        memory::region region( buffer.m.offset, buffer.length );
        vector.push_back( memory( desc, memory::read_write, region ) );
        control( desc, VIDIOC_QBUF, &buffer );
    }

    v4l2_buf_type buf_type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    control( desc, VIDIOC_STREAMON, &buf_type );

    v4l2_buffer buffer;
    std::memset( &buffer, 0, sizeof( buffer ) );
    buffer.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buffer.memory = V4L2_MEMORY_MMAP;

    poll poll;
    poll.insert( desc, make_function( *this, &sight::status ) );

    while ( state )
    {
        poll.wait();
        control( desc, VIDIOC_DQBUF, &buffer );

        memory& memory = vector[ buffer.index ];
        up_t source_size = memory.size();
        u8* source_data = memory.as< u8 >();
        v4lconvert_convert( data, &source_format, &target_format,
            source_data, source_size, target_data, target_size );
        call( target_data );

        control( desc, VIDIOC_QBUF, &buffer );
    }

    control( desc, VIDIOC_STREAMOFF, &buf_type );
}

OOE_NAMESPACE_END( ( ooe )( platform ) )
