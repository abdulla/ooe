/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IMAGE_IMAGE_HPP
#define OOE_FOUNDATION_IMAGE_IMAGE_HPP

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"
#include "foundation/utility/pointer.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( image_format ) )

enum
{
    uncompressed = 0,
    compressed = 128
};

enum type
{
    bgr_u8      = uncompressed,
    bgra_u8,

    rgb_u8,
    rgba_u8,
    y_u8,
    ya_u8,
    a_u8,
    r_u8,
    rg_u8,

    rgb_s16,
    rgba_s16,
    y_s16,
    ya_s16,
    a_s16,
    r_s16,
    rg_s16,

    rgb_f16,
    rgba_f16,
    y_f16,
    ya_f16,
    a_f16,
    r_f16,
    rg_f16,

    rgb_f32,
    rgba_f32,
    y_f32,
    ya_f32,
    a_f32,
    r_f32,
    rg_f32,

    rgba_dxt1   = compressed,
    rgba_dxt3,
    rgba_dxt5
};

OOE_NAMESPACE_END( ( ooe )( image_format ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- image_metadata -------------------------------------------------------------------------------
struct image_metadata
{
    const u32 width;
    const u32 height;
    const image_format::type format;

    image_metadata( u32, u32, image_format::type );
};

//--- image ----------------------------------------------------------------------------------------
class image
    : public image_metadata
{
public:
    typedef shared_free< void > data_ptr;

    image( u32, u32, image_format::type );

    template< typename to >
        to* as( void ) const
    {
        return data.as< to >();
    }

    void* get( void ) const
    {
        return data.get();
    }

    data_ptr ptr( void ) const
    {
        return data;
    }

private:
    const data_ptr data;

    image( u32, u32, image_format::type, const data_ptr& ) OOE_VISIBLE;

    friend class image_pyramid;
};

//--- image_reader ---------------------------------------------------------------------------------
class image_reader
    : public image_metadata
{
public:
    virtual ~image_reader( void ) {}
    virtual bool decode_row( void ) = 0;
    virtual u32 read_pixels( void*, u32 ) = 0;

protected:
    image_reader( const image_metadata& );
};

typedef shared_ptr< image_reader > reader_ptr;

//--------------------------------------------------------------------------------------------------
bool is_compressed( image_format::type ) OOE_VISIBLE;
u8 subpixels( image_format::type ) OOE_VISIBLE;
u8 subpixel_size( image_format::type ) OOE_VISIBLE;
u8 pixel_size( image_format::type ) OOE_VISIBLE;
u8 block_size( image_format::type ) OOE_VISIBLE;

u8 subpixels( const image_metadata& ) OOE_VISIBLE;
u8 subpixel_size( const image_metadata& ) OOE_VISIBLE;
up_t pixels( const image_metadata& ) OOE_VISIBLE;
u8 pixel_size( const image_metadata& ) OOE_VISIBLE;
u8 block_size( const image_metadata& ) OOE_VISIBLE;
up_t row_size( const image_metadata& ) OOE_VISIBLE;
up_t byte_size( const image_metadata& ) OOE_VISIBLE;

class descriptor;
typedef image ( * decoder_type )( const descriptor& );
typedef void ( * encoder_type )( const image&, const descriptor& );

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_IMAGE_IMAGE_HPP
