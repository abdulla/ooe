/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_GENERAL_VIDEO_HPP
#define OOE_FOUNDATION_GENERAL_VIDEO_HPP

#include "foundation/image/image.hpp"

namespace ooe
{
//--- bind ---------------------------------------------------------------------
	template< typename type >
		class bind
	{
	public:
		bind( type& object_ )
			: object( object_ )
		{
			object->bind();
		}

		~bind( void )
		{
			object->unbind();
		}

	private:
		type* object;
	};

//--- uniform ------------------------------------------------------------------
	struct uniform
	{
		enum type
		{
			float_1,	// float
			float_2,	// vec2
			float_3,	// vec3
			float_4,	// vec4

			int_1,		// int
			int_2,		// ivec2
			int_3,		// ivec3
			int_4,		// ivec4

			mat_2,		// mat2
			mat_3,		// mat3
			mat_4		// mat4
		};

		virtual ~uniform( void ) {}

		virtual void load( const void* ) = 0;
	};

	typedef shared_ptr< uniform > uniform_ptr;

//--- attribute ----------------------------------------------------------------
	struct attribute
	{
		enum type
		{
			float_1,	// float
			float_2,	// vec2
			float_3,	// vec3
			float_4,	// vec4

			mat_2,		// mat2
			mat_3,		// mat3
			mat_4		// mat4
		};

		virtual ~attribute( void ) {}
	};

	typedef shared_ptr< attribute > attribute_ptr;

//--- shader -------------------------------------------------------------------
	struct shader
	{
		enum type
		{
			vertex,
			fragment
		};

		virtual ~shader( void ) {}

		virtual void compile( const c8*, u32 ) = 0;
	};

//--- program ------------------------------------------------------------------
	struct program
	{
		virtual ~program( void ) {}

		virtual void bind( void ) = 0;
		virtual void unbind( void ) = 0;
		virtual void attach( const shader* ) = 0;
		virtual void detach( const shader* ) = 0;
		virtual void link( void ) = 0;

		virtual ooe::uniform* uniform( const c8*, uniform::type ) const = 0;
		virtual ooe::attribute* attribute( const c8*, attribute::type ) const = 0;
	};

	typedef shared_ptr< program > program_ptr;

//--- light --------------------------------------------------------------------
	struct light
 	{
 		enum parameter_type
 		{
			ambient,
			diffuse,
			specular,
			position
		};

		virtual ~light( void ) {}

		virtual void parameter( const f32*, parameter_type ) = 0;
	};

//--- texture ------------------------------------------------------------------
	struct texture
	{
		enum parameter_type
		{
			// filter
			nearest,
			bilinear,
			trilinear,

			// wrap
			repeat,
			clamp
		};

		enum environment_type
		{
			modulate,
			decal,
			blend,
			replace
		};

		enum
		{
			reload	= 1 << 0,
			reverse	= 1 << 1
		};

		virtual ~texture( void ) {}

		virtual void bind( void ) = 0;
		virtual void unbind( void ) = 0;
		virtual void environment( environment_type ) = 0;
		virtual void parameter( parameter_type ) = 0;
		virtual void load( const void*, u32, u32, u8, u8 = 0 ) = 0;
	};

	typedef shared_ptr< texture > texture_ptr;

//--- frame --------------------------------------------------------------------
	struct frame
	{
		virtual ~frame( void ) {}

		virtual void bind( void ) = 0;
		virtual void unbind( void ) = 0;
	};

//--- buffer -------------------------------------------------------------------
	struct buffer
	{
		enum type
		{
			point,
			index,
			pixel_pack,
			pixel_unpack
		};

		enum load_type
		{
			stream_draw,
			stream_read,
			stream_copy,

			static_draw,
			static_read,
			static_copy,

			dynamic_draw,
			dynamic_read,
			dynamic_copy,

			reload
		};

		enum map_type
		{
			read,
			write,
			read_write
		};

		virtual ~buffer( void ) {}

		virtual void bind( void ) = 0;
		virtual void unbind( void ) = 0;
		virtual void* map( map_type ) = 0;
		virtual void unmap( void ) = 0;
		virtual void load( const void*, up_t, load_type ) = 0;
	};

	typedef shared_ptr< buffer > buffer_ptr;

//--- video_data ---------------------------------------------------------------
	struct video_data
	{
		enum type
		{
			point_list,
			line_list,
			line_strip,
			triangle_list,
			triangle_strip,
			triangle_fan,
			quad_list
		};

		enum layout_type
		{
			vertex_2,
			vertex_3,
			normal,
			colour_3,
			colour_4,
			texture,
			attribute,

			index_16,
			index_32
		};

		type mode;	// vertex mode
		u32 length;	// number of points or indices
		u32 size;	// size of array
		shared_array< layout_type > layout;
		shared_array< attribute_ptr > extra;

		buffer_ptr point;
		buffer_ptr index;
	};

//--- video --------------------------------------------------------------------
	struct video
	{
		enum // clear type
		{
			depth_bit	= 1 << 0,
			stencil_bit	= 1 << 1,
			colour_bit	= 1 << 2
		};

		enum matrix_type
		{
			identity_matrix,

			model_view,
			projection,
			texture_matrix
		};

		enum operation_type
		{
			load,
			multiply
		};

		enum enable_type
		{
			blend_mode,
			alpha_test,
			depth_test,
			face_cull
		};

		enum function_type
		{
			zero,
			one,

			source,
			source_invert,
			source_alpha,
			source_alpha_invert,
			source_alpha_saturate,

			target,
			target_invert,
			target_alpha,
			target_alpha_invert
		};

		enum equation_type
		{
			equation,

			add,
			subtract,
			subtract_reverse,
			min,
			max
		};

		enum compare_type
		{
			never,
			always,
			equal,
			not_equal,
			less,
			less_equal,
			greater,
			greater_equal
		};

		enum maximum_type
		{
			units,		// maximum texture units
			lights,		// maximum lights
			vertices,	// recommended maximum vertices
			textures,	// maximum texture size
			indices		// recommended maximum indices
		};

		virtual ~video( void ) {}

		virtual void sync( bool ) = 0;
		virtual void swap( void ) = 0;
		virtual void current( void ) = 0;

		virtual void clear( u8 = depth_bit | stencil_bit | colour_bit ) = 0;
		virtual void matrix( matrix_type ) = 0;
		virtual void matrix( const f32*, operation_type ) = 0;
		virtual void viewport( u32, u32, u32 = 0, u32 = 0 ) = 0;

		virtual void unit( u8 ) = 0;
		virtual void enable( enable_type, bool = true ) = 0;
		virtual void blend( u8, u8 ) = 0;
		virtual void alpha( compare_type, f32 ) = 0;
		virtual void depth( compare_type ) = 0;

		virtual ooe::shader* shader( shader::type ) const = 0;
		virtual ooe::program* program( void ) const = 0;
		virtual ooe::light* light( void ) const = 0;
		virtual ooe::texture* texture( void ) const = 0;
		virtual ooe::frame* frame( void ) const = 0;
		virtual ooe::buffer* buffer( buffer::type ) const = 0;

		virtual void draw( const video_data& ) = 0;
		virtual u32 maximum( maximum_type ) const = 0;
	};
}

#endif	// OOE_FOUNDATION_GENERAL_VIDEO_HPP
