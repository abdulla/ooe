/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <vector>

#include <cmath>

#include "foundation/executable/program.hpp"
#include "foundation/general/camera.hpp"
#include "foundation/general/text.hpp"
#include "foundation/image/dds.hpp"
#include "foundation/image/exr.hpp"
#include "foundation/image/jpeg.hpp"
#include "foundation/image/jpeg2000.hpp"
#include "foundation/image/png.hpp"
#include "foundation/io/directory.hpp"
#include "foundation/lua/signal.hpp"
#include "foundation/lua/table.hpp"
#include "foundation/lua/type.hpp"
#include "foundation/lua/vm.hpp"
#include "foundation/scene/effect.hpp"

#include "external/engine/dynamic.hpp"
#include "external/engine/interface.hpp"
#include "external/engine/setup.hpp"
#include "external/graph/flat.hpp"
#include "external/graph/node.hpp"
#include "external/graph/simple.hpp"
#include "external/service/service.hpp"
#include "external/service/input.hpp"
#include "external/service/sight.hpp"
#include "external/show/expose.hpp"

namespace
{
	using namespace ooe;

	typedef std::vector< std::string > vector_type;
	typedef std::vector< scene::node* > node_vector;

//--- utility ------------------------------------------------------------------
	void find_slides( vfs& vfs, vector_type& vector )
	{
		directory slides( vfs[ "slide" ] );

		while ( ++slides )
		{
			const std::string& slide = *slides;
			up_t size = slide.size();

			if ( slide.rfind( ".lua" ) == size - 4 )
				vector.push_back( std::string( "slide/" ) + slide );
		}

		std::sort( vector.begin(), vector.end() );
	}

	void lua_local( lua::stack& stack, scene::graph* perspective, scene::graph* orthographic,
		const service& service, const settings& set )
	{
		lua::type< node_vector >( stack )
			.insert( "push_back", &node_vector::push_back );

		lua::table global( stack, lua::global );
		lua::table signal( stack );
		global[ "perspective" ] = perspective;
		global[ "orthographic" ] = orthographic;
		global[ "signal" ] = signal;
		signal_base* sight = service[ "sight" ];

		if ( sight )
			signal[ "sight" ] = static_cast< sight_signal& >( *sight );

		lua::table scene = global[ "scene" ];
		scene[ "video_size" ] = make_tuple( set.view.width, set.view.width );
		scene[ "sight_size" ] = make_tuple( set.sight.width, set.sight.width );
	}

//--- slots --------------------------------------------------------------------
	struct slots
	{
		frame_slot frame;
		key_slot key;
	};

//--- driver -------------------------------------------------------------------
	class driver
	{
	public:
		driver( core_type&, const settings&, service&, const resource< image_decode_t >& );

		void frame( f32 );
		void key( u32, bool );

		void slide( f32 );

	private:
		slots slot;
		cache_DEPRECATED< ooe::texture > texture;
		cache_DEPRECATED< ooe::mesh > mesh;

		node_vector titles;
		s8 move;
		f32 time_left;

		u32 slides;
		f32 radius;
		u32 width;

		scoped_ptr< scene::graph > perspective;
		scoped_ptr< scene::graph > orthographic;
		ooe::camera camera;

		lua::vm vm;
		ooe::effect effect;
	};

	driver::driver( core_type& core, const settings& set, service& service,
		const resource< image_decode_t >& resource )
		: slot(), texture( core.vfs, core.video, resource ), mesh( core.vfs, core.video ), titles(),
		move( 0 ), time_left( 0 ), slides(), radius(), width( set.view.width ),
		perspective( simple_create( core.video, set.view.width, set.view.height ) ),
		orthographic( flat_create( core.video, set.view.width, set.view.height ) ),
		camera( degree( 45 ), divide( width, set.view.height ), 1, 100 ), vm(),
		effect( core.vfs[ "effect/fixed.lua" ], core.vfs, core.video )
	{
		effect.get().bind();

		slot.frame = make_function( *this, &driver::frame );
		slot.key = make_function( *this, &driver::key );

		connect( service[ "frame" ], slot.frame );
		connect( service[ "key" ], slot.key );

		vector_type vector;
		find_slides( core.vfs, vector );

		slides = vector.size() * 2;
		f32 phi = 2.f * maths::pi / static_cast< f32 >( slides );
		radius = -1.f / std::tan( phi * .5f );

		font::library library;
		font::face face( library, core.vfs[ "font/vera.ttf" ] );
		text text( core.video, face, 32 );

		lua::stack stack = vm.stack();
		lua::table global( stack, lua::global );
		expose( stack, texture, mesh );
		lua_local( stack, perspective, orthographic, service, set );
		global[ "text" ] = &text;
		global[ "titles" ] = &titles;
		global[ "video" ] = &core.video;
		global[ "driver" ] = this;
		global[ "radius" ] = radius;

		typedef vector_type::const_iterator iterator;
		u32 j = 0;

		for ( iterator i = vector.begin(), end = vector.end(); i != end; ++i, ++j )
		{
			global[ "rotation" ] = static_cast< f32 >( j ) * phi;
			global[ "position" ] = static_cast< f32 >( j * set.view.width );
			vm.load( *i, core.vfs[ *i ] );
		}

		vm.collect();
		camera.rotate( vec3( 0, maths::pi * -.5, 0 ) );
		camera.translate( vec3( -radius, 0, 0 ) );
		camera.rotate( vec3( 0, maths::pi, 0 ) );
	}

	void driver::frame( f32 frame_time )
	{
		if ( move )
			slide( frame_time );

		perspective->draw( camera );
		orthographic->draw( camera );
	}

	void driver::key( u32 key_, bool press )
	{
		if ( !press )
		{
			if ( key_ == 'q' )
				executable::quit();

			return;
		}

		switch ( key_ )
		{
		case '.':
			if ( move == 1 )
				break;

			time_left += 1;
			move = -1;
			break;

		case ',':
			if ( move == -1 )
				break;

			time_left += 1;
			move = 1;
		default:
			break;
		}
	}

	void driver::slide( f32 frame_time )
	{
		time_left -= frame_time;
		u8 stop;

		if ( time_left < 0 )
		{
			frame_time += time_left;
			time_left = 0;
			stop = 0;
		}
		else
			stop = move;

		f32 z = static_cast< f32 >( width ) * frame_time;
		typedef node_vector::const_iterator iterator;

		for ( iterator i = titles.begin(), end = titles.end(); i != end; ++i )
			( *i )->matrix[ 3 ][ 0 ] += static_cast< f32 >( move ) * z;

		f32 x = 2.f * maths::pi * radius * frame_time / static_cast< f32 >( slides );
		f32 y = 2.f * maths::pi * frame_time / static_cast< f32 >( slides );
		camera.translate( vec3( 0, 0, x * static_cast< f32 >( -move ) ) );
		camera.rotate( vec3( 0, y * static_cast< f32 >( -move ), 0 ) );
		move = stop;
	}

	void close( void* pointer )
	{
		delete static_cast< driver* >( pointer );
	}
}

extern "C" dynamic::close_type ooe_open( core_type&, const settings&, service& ) OOE_VISIBLE;
extern "C" dynamic::close_type ooe_open( core_type& core, const settings& set, service& service )
{
	resource< image_decode_t > resource;
	resource.insert( "dds", compressed_decode< dds::decode > );
	resource.insert( "exr", uncompressed_decode< exr::decode > );
	resource.insert( "jpg", uncompressed_decode< jpeg::decode > );
	resource.insert( "jp2", uncompressed_decode< jpeg2000::decode > );
	resource.insert( "png", uncompressed_decode< png::decode > );
	return dynamic::close_type( new driver( core, set, service, resource ), close );
}
