/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <vector>

#include <cerrno>

#include <linux/videodev2.h>
#include <sys/ioctl.h>

#include "foundation/general/sight.hpp"
#include "foundation/io/memory.hpp"
#include "foundation/utility/error.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

void control( const descriptor& desc, up_t request, void* pointer )
{
	if ( ioctl( desc.get(), request, pointer ) == -1 )
		throw error::runtime( "sight: " ) << "Unable control device: " << error::number( errno );
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- sight ----------------------------------------------------------------------------------------
sight::sight( const call_type& call_, u16 width, u16 height )
	: platform::sight( call_, width, height )
{
}

uncompressed_image::type sight::format( void )
{
	return uncompressed_image::bgr_u8;
}

OOE_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( platform ) )

//--- sight ----------------------------------------------------------------------------------------
sight::sight( const call_type& call, u16 width, u16 height )
	: state( true ), task( make_function( *this, &sight::main ), call, width, height )
{
}

sight::~sight( void )
{
	state = false;
	task();
}

void sight::status( const descriptor&, poll::type type )
{
	if ( type != poll::input )
		state = false;
}

void sight::main( call_type call, u16 width, u16 height )
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

	v4l2_format format;
	std::memset( &format, 0, sizeof( format ) );
	format.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	format.fmt.pix.width = width;
	format.fmt.pix.height = height;
	format.fmt.pix.pixelformat = V4L2_PIX_FMT_RGB24;
	format.fmt.pix.field = V4L2_FIELD_INTERLACED;
	control( desc, VIDIOC_S_FMT, &format );

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
		call( vector[ buffer.index ].as< u8 >() );
		control( desc, VIDIOC_QBUF, &buffer );
	}

	control( desc, VIDIOC_STREAMOFF, &buf_type );
}

OOE_NAMESPACE_END( ( ooe )( platform ) )
