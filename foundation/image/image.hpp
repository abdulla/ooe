/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IMAGE_IMAGE_HPP
#define OOE_FOUNDATION_IMAGE_IMAGE_HPP

#include "foundation/utility/fundamental.hpp"
#include "foundation/utility/macro.hpp"
#include "foundation/utility/pointer.hpp"

namespace ooe
{
	class descriptor;

//--- image --------------------------------------------------------------------
	class image
	{
	public:
		enum
		{
			uncompressed = 127,
			compressed = 128
		};

		const u32 width;
		const u32 height;
		const u8 format;

		template< typename to >
			to* as( void ) const
		{
			return data.as< to >();
		}

		void* get( void ) const
		{
			return data.get();
		}

	protected:
		typedef up_t ( image::* function_type )( void );

		image( u32, u32, u8, function_type );

	private:
		const shared_free< void > data;
	};

//--- uncompressed_image -------------------------------------------------------
	struct OOE_VISIBLE uncompressed_image
		: public image
	{
		enum type
		{
			bgr_u8,
			bgra_u8,

			rgb_u8,
			rgba_u8,
			a_u8,
			y_u8,
			ya_u8,

			rgb_f16,
			rgba_f16,
			a_f16,
			y_f16,
			ya_f16,

			rgb_f32,
			rgba_f32,
			a_f32,
			y_f32,
			ya_f32
		};

		uncompressed_image( u32, u32, type );
		uncompressed_image( const image& );

		u8 channels( void ) const;
		u8 channel_size( void ) const;
		u8 pixel_size( void ) const;
		up_t row_size( void ) const;
		up_t byte_size( void ) const;
	};

//--- compressed_image ---------------------------------------------------------
	struct OOE_VISIBLE compressed_image
		: public image
	{
		enum type
		{
			rgba_dxt1 = compressed,
			rgba_dxt3,
			rgba_dxt5
		};

		compressed_image( u32, u32, type );
		compressed_image( const image& );

		u8 channels( void ) const;
		u8 block_size( void ) const;
		up_t byte_size( void ) const;
	};

//--- decode -------------------------------------------------------------------
	template< uncompressed_image ( * decode )( const descriptor& ) >
		image uncompressed_decode( const descriptor& desc )
	{
		return decode( desc );
	}

	template< compressed_image ( * decode )( const descriptor& ) >
		image compressed_decode( const descriptor& desc )
	{
		return decode( desc );
	}

//--- encode -------------------------------------------------------------------
	template< void ( * encode )( const uncompressed_image&, const descriptor& ) >
		void uncompressed_encode( const image& image, const descriptor& desc )
	{
		encode( image, desc );
	}

	template< void ( * encode )( const compressed_image&, const descriptor& ) >
		void compressed_encode( const image& image, const descriptor& desc )
	{
		encode( image, desc );
	}
}

#endif	// OOE_FOUNDATION_IMAGE_IMAGE_HPP
