/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <objc/message.h>
#include <QTKit/QTKit.h>

#include "foundation/general/sight.hpp"
#include "foundation/utility/error.hpp"
#include "foundation/utility/scoped.hpp"

using namespace ooe;

//--- Sight --------------------------------------------------------------------
OOE_HIDDEN
@interface Sight
	: NSObject
{
@private
	sight::call_type* call;
	QTCaptureSession* session;
	QTCaptureDeviceInput* input;
	QTCaptureVideoPreviewOutput* output;
}
@end

@implementation Sight
- ( id ) initWithCall: ( sight::call_type* )call_ width: ( u16 )width height: ( u16 )height
{
	if ( ![ super init ] )
		throw error::runtime( "sight: " ) << "Unable to initialise super";

	typedef id ( send_type )( id, SEL );
	scoped< send_type >
		scoped( reinterpret_cast< send_type* >( objc_msgSend ), self, @selector( release ) );

	call = call_;
	session = [ [ QTCaptureSession alloc ] init ];

	if ( !session )
		throw error::runtime( "sight: " ) << "Unable to initialise capture session";

	QTCaptureDevice* device = [ QTCaptureDevice defaultInputDeviceWithMediaType: QTMediaTypeVideo ];

	if ( !device )
		throw error::runtime( "sight: " ) << "Unable to initialise device";

	NSError* error;

	if ( ![ device open: &error ] )
		throw error::runtime( "sight: " ) << "Unable to open device: " <<
			error.localizedDescription.UTF8String;

	input = [ [ QTCaptureDeviceInput alloc ] initWithDevice: device ];

	if ( !input )
		throw error::runtime( "sight: " ) << "Unable to link device to input";

	if ( ![ session addInput: input error: &error ] )
		throw error::runtime( "sight: " ) << "Unable to add input to capture session: " <<
			error.localizedDescription.UTF8String;

	output = [ [ QTCaptureVideoPreviewOutput alloc ] init ];

	if ( !output )
		throw error::runtime( "sight: " ) << "Unable to initialise output";

	// refer to: http://developer.apple.com/qa/qa2008/qa1582.html
	NSDictionary* attributes = [ NSDictionary dictionaryWithObjectsAndKeys:
		[ NSNumber numberWithDouble: width ], kCVPixelBufferWidthKey,
		[ NSNumber numberWithDouble: height ], kCVPixelBufferHeightKey,
		[ NSNumber numberWithUnsignedInt: kCVPixelFormatType_24RGB ],
		kCVPixelBufferPixelFormatTypeKey, nil ];

	if ( !attributes )
		throw error::runtime( "sight: " ) << "Unable to allocate attribute dictionary";

	output.pixelBufferAttributes = attributes;
	output.delegate = self;

	if ( ![ session addOutput: output error: &error ] )
		throw error::runtime( "sight: " ) << "Unable to add output to capture session: " <<
			error.localizedDescription.UTF8String;

	[ session startRunning ];
	scoped.clear();
	return self;
}

- ( void ) dealloc
{
	[ session stopRunning ];
	[ input.device close ];

	[ input release ];
	[ output release ];
	[ session release ];

	[ super dealloc ];
}
@end

//--- Sight( Delegate ) --------------------------------------------------------
@interface Sight( Delegate )
- ( void ) captureOutput: ( QTCaptureOutput* )output_ didOutputVideoFrame: ( CVImageBufferRef )image
	withSampleBuffer: ( QTSampleBuffer* )sample fromConnection: ( QTCaptureConnection* )connection;
@end

@implementation Sight( Delegate )
- ( void ) captureOutput: ( QTCaptureOutput* )output_ didOutputVideoFrame: ( CVImageBufferRef )image
	withSampleBuffer: ( QTSampleBuffer* )sample fromConnection: ( QTCaptureConnection* )connection
{
	( void )output_;
	( void )sample;
	( void )connection;

	CVPixelBufferLockBaseAddress( image, 0 );
	( *call )( static_cast< u8* >( CVPixelBufferGetBaseAddress( image ) ) );
	CVPixelBufferUnlockBaseAddress( image, 0 );
}
@end

namespace ooe
{
//--- platform::sight ----------------------------------------------------------
	platform::sight::sight( const call_type& call_, u16 width, u16 height )
		: call( call_ ), id( [ [ Sight alloc ] initWithCall: &call width: width height: height ] )
	{
		if ( !id )
			throw error::runtime( "sight: " ) << "Unable to initialise";
	}

	platform::sight::~sight( void )
	{
		[ id release ];
	}

//--- sight --------------------------------------------------------------------
	sight::sight( const call_type& call_, u16 width, u16 height )
		: platform::sight( call_, width, height )
	{
	}

	uncompressed_image::type sight::format( void )
	{
		return uncompressed_image::rgb_u8;
	}
}
