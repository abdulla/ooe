/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl/attribute.hpp"
#include "foundation/opengl/buffer.hpp"
#include "foundation/opengl/context.hpp"
#include "foundation/opengl/light.hpp"
#include "foundation/opengl/program.hpp"
#include "foundation/opengl/shader.hpp"
#include "foundation/opengl/symbol.hpp"
#include "foundation/opengl/texture.hpp"
#include "foundation/utility/error.hpp"

#include "external/engine/dynamic.hpp"

namespace
{
	using namespace ooe;
	using namespace opengl;

	class video
		: private noncopyable, public ooe::video
	{
	public:
		video( const view_data&, platform::context_type );
		virtual ~video( void );

		virtual void sync( bool );
		virtual void swap( void );
		virtual void current( void );

		virtual void clear( u8 );
		virtual void matrix( matrix_type );
		virtual void matrix( const f32*, operation_type );
		virtual void viewport( u32, u32, u32, u32 );

		virtual void unit( u8 );
		virtual void enable( enable_type, bool );
		virtual void blend( u8, u8 );
		virtual void alpha( compare_type, f32 );
		virtual void depth( compare_type );

		virtual ooe::shader* shader( ooe::shader::type ) const;
		virtual ooe::program* program( void ) const;
		virtual ooe::light* light( void ) const;
		virtual ooe::texture* texture( void ) const;
		virtual ooe::frame* frame( void ) const OOE_NORETURN;
		virtual ooe::buffer* buffer( ooe::buffer::type ) const;

		virtual void draw( const video_data& );
		virtual u32 maximum( maximum_type ) const;

	private:
		const view_data& view;
		platform::context_type context;

		s32 units;
		s32 lights;

		scoped_array< bool > unit_array;
		scoped_array< bool > light_array;

		bool normal_state;
		bool colour_state;
	};

	typedef video video_type;

	platform::context_type context_f( const view_data& );
	bool* array_f( u32, s32& );
	u32 function_f( u8 );
	u32 equation_f( u8 );
	u32 compare_f( video::compare_type );
	u32 mode_f( u8 );
	u32 size_f( const video_data&, bool&, bool& );
	void state_f( bool&, bool, u32 );
	void unit_f( bool*, u32, bool );

//--- video --------------------------------------------------------------------
	video::video( const view_data& view_, platform::context_type context_ )
	try
		: view( view_ ), context( context_ ), units(), lights(),
		unit_array( array_f( MAX_TEXTURE_COORDS, units ) ),
		light_array( array_f( MAX_LIGHTS, lights ) ), normal_state( false ), colour_state( false )
	{
		EnableClientState( VERTEX_ARRAY );
		ClearColor( 0, 0, 0, 0 );
		ClearDepth( 1 );
		LineWidth( 2 );
		Hint( PERSPECTIVE_CORRECTION_HINT, NICEST );
		Hint( GENERATE_MIPMAP_HINT, NICEST );
	}
	catch ( ... )
	{
		this->~video();
		// standard states that the exception will be automatically rethrown after this point, see:
		// http://www.gotw.ca/gotw/066.htm
	}

	video::~video( void )
	{
		context_current( view, 0 );
		context_destruct( view, context );
	}

	void video::sync( bool vsync )
	{
		if ( !context_sync( view, context, vsync ) )
			throw error::runtime( "opengl: " ) << "Unable to set vertical sync";
	}

	void video::swap( void )
	{
		context_swap( view, context );
	}

	void video::current( void )
	{
		if ( !context_current( view, context ) )
			throw error::runtime( "opengl: " ) << "Unable to set context";
	}

	void video::clear( u8 target_ )
	{
		u32 mask = 0;

		if ( target_ & depth_bit )
			mask |= DEPTH_BUFFER_BIT;

		if ( target_ & stencil_bit )
			mask |= STENCIL_BUFFER_BIT;

		if ( target_ & colour_bit )
			mask |= COLOR_BUFFER_BIT;

		Clear( mask );
	}

	void video::matrix( matrix_type type )
	{
		u32 mode;

		switch ( type )
		{
		case model_view:
			mode = MODELVIEW;
			break;

		case projection:
			mode = PROJECTION;
			break;

		case texture_matrix:
			mode = TEXTURE;
			break;

		default:
			throw error::runtime( "opengl: " ) << "Unknown matrix type";
		}

		MatrixMode( mode );
	}

	void video::matrix( const f32* data, operation_type mode )
	{
		if ( mode != load )
			MultMatrixf( data );
		else if ( data )
			LoadMatrixf( data );
		else
			LoadIdentity();
	}

	void video::viewport( u32 width, u32 height, u32 left, u32 top )
	{
		Viewport( left, top, width, height );
	}

	void video::unit( u8 unit_ )
	{
		ActiveTexture( TEXTURE0 + unit_ );
	}

	void video::enable( enable_type type, bool enable_ )
	{
		u32 capability;

		switch ( type )
		{
		case blend_mode:
			capability = BLEND;
			break;

		case alpha_test:
			capability = ALPHA_TEST;
			break;

		case depth_test:
			capability = DEPTH_TEST;
			break;

		case face_cull:
			capability = CULL_FACE;
			break;

		default:
			throw error::runtime( "opengl: " ) << "Unknown enable type";
		}

		if ( enable_ )
			Enable( capability );
		else
			Disable( capability );
	}

	void video::blend( u8 source_, u8 target_ )
	{
		if ( source_ == video::equation )
			BlendEquation( equation_f( target_ ) );
		else
			BlendFunc( function_f( source_ ), function_f( target_ ) );
	}

	void video::alpha( compare_type compare, f32 reference )
	{
		AlphaFunc( compare_f( compare ), reference );
	}

	void video::depth( compare_type compare )
	{
		DepthFunc( compare_f( compare ) );
	}

	ooe::shader* video::shader( ooe::shader::type type ) const
	{
		return new opengl::shader( type );
	}

	ooe::program* video::program( void ) const
	{
		return new opengl::program;
	}

	ooe::light* video::light( void ) const
	{
		s32 light_ = 0;

		while ( light_ != lights )
		{
			if ( !light_array[ light_ ] )
				break;
			else
				++light_;
		}

		if ( light_ != lights )
			return new opengl::light( light_, light_array[ light_ ] );
		else
			return 0;
	}

	ooe::texture* video::texture( void ) const
	{
		return new opengl::texture;
	}

	ooe::frame* video::frame( void ) const
	{
		throw error::runtime( "opengl: " ) << "Frame unimplemented";
	}

	ooe::buffer* video::buffer( ooe::buffer::type type ) const
	{
		return new opengl::buffer( type );
	}

	void video::draw( const video_data& data )
	{
		using ooe::add;
		u32 size = size_f( data, normal_state, colour_state );
		s32 coord = 0;
		s32 index = -1;
		u32 offset = 0;
		u32 attr = 0;
		u32 type = UNSIGNED_SHORT;

		opengl::attribute* a;
		const void* null = 0;
		data.point->bind();

		for ( u32 i = 0; i != data.size; ++i )
		{
			switch ( data.layout[ i ] )
			{
			case video_data::vertex_2:
				VertexPointer( 2, FLOAT, size, add< void >( null, offset ) );
				offset += 2 * sizeof( f32 );
				break;

			case video_data::vertex_3:
				VertexPointer( 3, FLOAT, size, add< void >( null, offset ) );
				offset += 3 * sizeof( f32 );
				break;

			case video_data::normal:
				NormalPointer( FLOAT, size, add< void >( null, offset ) );
				offset += 3 * sizeof( f32 );
				break;

			case video_data::colour_3:
				ColorPointer( 3, FLOAT, size, add< void >( null, offset ) );
				offset += 3 * sizeof( f32 );
				break;

			case video_data::colour_4:
				ColorPointer( 4, FLOAT, size, add< void >( null, offset ) );
				offset += 4 * sizeof( f32 );
				break;

			case video_data::texture:
				unit_f( unit_array, coord++, true );
				TexCoordPointer( 2, FLOAT, size, add< void >( null, offset ) );
				offset += 2 * sizeof( f32 );
				break;

			case video_data::attribute:
				a = data.extra[ attr++ ].as< opengl::attribute >();
				EnableVertexAttribArray( a->id );
				VertexAttribPointer( a->id, a->size, FLOAT, 0, size, add< void >( null, offset ) );
				offset += a->size * sizeof( f32 );
				break;

			case video_data::index_16:
				index = i;
				break;

			case video_data::index_32:
				index = i;
				type = UNSIGNED_INT;
				break;

			default:
				throw error::runtime( "opengl: " ) << "Unknown layout type";
			}
		}

		while ( coord != units )
			unit_f( unit_array, coord++, false );

		u32 mode = mode_f( data.mode );

		if ( index == -1 )
			DrawArrays( mode, 0, data.length );
		else
		{
			data.index->bind();
			DrawElements( mode, data.length, type, 0 );
		}

		while ( attr-- )
		{
			a = data.extra[ attr ].as< opengl::attribute >();
			DisableVertexAttribArray( a->id );
		}
	}

	u32 video::maximum( maximum_type type ) const
	{
		s32 maximum_;

		switch ( type )
		{
		case ooe::video::units:
			return units;

		case ooe::video::lights:
			return lights;

		case vertices:
			GetIntegerv( MAX_ELEMENTS_VERTICES, &maximum_ );
			break;

		case textures:
			GetIntegerv( MAX_TEXTURE_SIZE, &maximum_ );
			break;

		case indices:
			GetIntegerv( MAX_ELEMENTS_INDICES, &maximum_ );
			break;

		default:
			throw error::runtime( "opengl: " ) << "Unknown maximum type";
		}

		return maximum_;
	}

//--- utility ------------------------------------------------------------------
	platform::context_type context_f( const view_data& view )
	{
		platform::context_type context = context_construct( view );
		setup_context( view, context );
		opengl::symbol();
		return context;
	}

	bool* array_f( u32 type, s32& size )
	{
		GetIntegerv( type, &size );
		bool* array = new bool[ size ];
		std::fill_n( array, sizeof( bool ) * size, false );
		return array;
	}

	u32 function_f( u8 mode )
	{
		switch ( mode )
		{
		case video::zero:
			return ZERO;

		case video::one:
			return ONE;

		case video::source:
			return SRC_COLOR;

		case video::source_invert:
			return ONE_MINUS_SRC_COLOR;

		case video::source_alpha:
			return SRC_ALPHA;

		case video::source_alpha_invert:
			return ONE_MINUS_SRC_ALPHA;

		case video::source_alpha_saturate:
			return SRC_ALPHA_SATURATE;

		case video::target:
			return DST_COLOR;

		case video::target_invert:
			return ONE_MINUS_DST_COLOR;

		case video::target_alpha:
			return DST_ALPHA;

		case video::target_alpha_invert:
			return ONE_MINUS_DST_ALPHA;

		default:
			throw error::runtime( "opengl: " ) << "Unknown function type";
		}
	}

	u32 equation_f( u8 mode )
	{
		switch ( mode )
		{
		case video::add:
			return FUNC_ADD;

		case video::subtract:
			return FUNC_SUBTRACT;

		case video::subtract_reverse:
			return FUNC_REVERSE_SUBTRACT;

		case video::min:
			return MIN;

		case video::max:
			return MAX;

		default:
			throw error::runtime( "opengl: " ) << "Unknown equation type";
		}
	}

	u32 compare_f( video::compare_type mode )
	{
		switch ( mode )
		{
		case video::always:
			return ALWAYS;

		case video::never:
			return NEVER;

		case video::equal:
			return EQUAL;

		case video::not_equal:
			return NOTEQUAL;

		case video::less:
			return LESS;

		case video::less_equal:
			return LEQUAL;

		case video::greater:
			return GREATER;

		case video::greater_equal:
			return GEQUAL;

		default:
			throw error::runtime( "opengl: " ) << "Unknown compare type";
		}
	}

	u32 mode_f( u8 mode )
	{
		switch ( mode )
		{
		case video_data::triangle_list:
			return TRIANGLES;

		case video_data::triangle_strip:
			return TRIANGLE_STRIP;

		case video_data::triangle_fan:
			return TRIANGLE_FAN;

		case video_data::point_list:
			return POINTS;

		case video_data::line_list:
			return LINES;

		case video_data::line_strip:
			return LINE_STRIP;

		case video_data::quad_list:
			return QUADS;

		default:
			throw error::runtime( "opengl: " ) << "Unknown mode type";
		}
	}

	u32 size_f( const video_data& data, bool& normal_state, bool& colour_state )
	{
		u32 size = 0;
		u32 attr = 0;
		bool vertex = false;
		bool normal = false;
		bool colour = false;
		opengl::attribute* a;

		for ( u32 i = 0; i != data.size; ++i )
		{
			switch ( data.layout[ i ] )
			{
			case video_data::vertex_2:
				vertex = true;
				size += 2 * sizeof( f32 );
				break;

			case video_data::vertex_3:
				vertex = true;
				size += 3 * sizeof( f32 );
				break;

			case video_data::normal:
				normal = true;
				size += 3 * sizeof( f32 );
				break;

			case video_data::colour_3:
				colour = true;
				size += 3 * sizeof( f32 );
				break;

			case video_data::colour_4:
				colour = true;
				size += 4 * sizeof( f32 );
				break;

			case video_data::texture:
				size += 2 * sizeof( f32 );
				break;

			case video_data::attribute:
				a = data.extra[ attr++ ].as< opengl::attribute >();
				size += a->size * sizeof( f32 );
				break;

			case video_data::index_16:
			case video_data::index_32:
				break;

			default:
				throw error::runtime( "opengl: " ) << "Unknown layout type";
			}
		}

		if ( !vertex )
			throw error::runtime( "opengl: " ) << "Vertex data missing";

		state_f( normal_state, normal, NORMAL_ARRAY );
		state_f( colour_state, colour, COLOR_ARRAY );
		return size;
	}

	void state_f( bool& state, bool enable, u32 type )
	{
		if ( state == enable )
			return;
		else
			state = enable;

		if ( enable )
			EnableClientState( type );
		else
			DisableClientState( type );
	}

	void unit_f( bool* unit, u32 index, bool enable )
	{
		if ( enable )
			ClientActiveTexture( TEXTURE0 + index );

		if ( unit[ index ] == enable )
			return;
		else
			unit[ index ] = enable;

		if ( enable )
			EnableClientState( TEXTURE_COORD_ARRAY );
		else
		{
			ClientActiveTexture( TEXTURE0 + index );
			DisableClientState( TEXTURE_COORD_ARRAY );
		}
	}

	void close( void* pointer )
	{
		delete static_cast< video_type* >( pointer );
	}
}

extern "C" dynamic::close_type ooe_open( const view_data& ) OOE_VISIBLE;
extern "C" dynamic::close_type ooe_open( const view_data& view )
{
	return dynamic::close_type( new video_type( view, context_f( view ) ), close );
}
