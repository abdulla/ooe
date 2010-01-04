/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/opengl/symbol.hpp"
#include "foundation/opengl/uniform.hpp"
#include "foundation/utility/error.hpp"

namespace
{
	using namespace ooe;

	template< u8 >
		void load_f( s32, const void* );

	template<>
		void load_f< ooe::uniform::float_1 >( s32 id, const void* pointer )
	{
		opengl::Uniform1fv( id, 1, static_cast< const f32* >( pointer ) );
	}

	template<>
		void load_f< ooe::uniform::float_2 >( s32 id, const void* pointer )
	{
		opengl::Uniform2fv( id, 1, static_cast< const f32* >( pointer ) );
	}

	template<>
		void load_f< ooe::uniform::float_3 >( s32 id, const void* pointer )
	{
		opengl::Uniform3fv( id, 1, static_cast< const f32* >( pointer ) );
	}

	template<>
		void load_f< ooe::uniform::float_4 >( s32 id, const void* pointer )
	{
		opengl::Uniform4fv( id, 1, static_cast< const f32* >( pointer ) );
	}

	template<>
		void load_f< ooe::uniform::int_1 >( s32 id, const void* pointer )
	{
		opengl::Uniform1iv( id, 1, static_cast< const s32* >( pointer ) );
	}

	template<>
		void load_f< ooe::uniform::int_2 >( s32 id, const void* pointer )
	{
		opengl::Uniform2iv( id, 1, static_cast< const s32* >( pointer ) );
	}

	template<>
		void load_f< ooe::uniform::int_3 >( s32 id, const void* pointer )
	{
		opengl::Uniform3iv( id, 1, static_cast< const s32* >( pointer ) );
	}

	template<>
		void load_f< ooe::uniform::int_4 >( s32 id, const void* pointer )
	{
		opengl::Uniform4iv( id, 1, static_cast< const s32* >( pointer ) );
	}

	template<>
		void load_f< ooe::uniform::mat_2 >( s32 id, const void* pointer )
	{
		opengl::UniformMatrix2fv( id, 1, false, static_cast< const f32* >( pointer ) );
	}

	template<>
		void load_f< ooe::uniform::mat_3 >( s32 id, const void* pointer )
	{
		opengl::UniformMatrix3fv( id, 1, false, static_cast< const f32* >( pointer ) );
	}

	template<>
		void load_f< ooe::uniform::mat_4 >( s32 id, const void* pointer )
	{
		opengl::UniformMatrix4fv( id, 1, false, static_cast< const f32* >( pointer ) );
	}

	opengl::uniform::function_type function_f( ooe::uniform::type type )
	{
		switch ( type )
		{
		case ooe::uniform::float_1:
			return load_f< ooe::uniform::float_1 >;

		case ooe::uniform::float_2:
			return load_f< ooe::uniform::float_2 >;

		case ooe::uniform::float_3:
			return load_f< ooe::uniform::float_3 >;

		case ooe::uniform::float_4:
			return load_f< ooe::uniform::float_4 >;

		case ooe::uniform::int_1:
			return load_f< ooe::uniform::int_1 >;

		case ooe::uniform::int_2:
			return load_f< ooe::uniform::int_2 >;

		case ooe::uniform::int_3:
			return load_f< ooe::uniform::int_3 >;

		case ooe::uniform::int_4:
			return load_f< ooe::uniform::int_4 >;

		case ooe::uniform::mat_2:
			return load_f< ooe::uniform::mat_2 >;

		case ooe::uniform::mat_3:
			return load_f< ooe::uniform::mat_3 >;

		case ooe::uniform::mat_4:
			return load_f< ooe::uniform::mat_4 >;

		default:
			throw error::runtime( "opengl: " ) << "Unknown uniform type: " << type;
		}
	}
}

namespace ooe
{
//--- uniform ------------------------------------------------------------------
	opengl::uniform::uniform( u32 program, const c8* name, type value )
		: id( GetUniformLocation( program, name ) ), function( function_f( value ) )
	{
		if ( id == -1 )
			throw error::runtime( "opengl: " ) << "Invalid uniform \"" << name << '\"';
	}

	opengl::uniform::~uniform( void )
	{
	}

	void opengl::uniform::load( const void* pointer )
	{
		function( id, pointer );
	}
}
