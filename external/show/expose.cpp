/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include <vector>

#include "foundation/general/text.hpp"
#include "foundation/lua/table.hpp"
#include "foundation/lua/type.hpp"
#include "foundation/maths/maths.hpp"

#include "external/graph/node.hpp"
#include "external/show/expose.hpp"

namespace
{
	using namespace ooe;

	mat4 matrix_mul( mat4& m, mat4& n )
	{
		return m * n;
	}

	mat4::column& matrix_index( mat4& m, u8 i )
	{
		return m[ static_cast< u8 >( i - 1 ) ];
	}
}

namespace ooe
{
	void expose( lua::stack& stack, cache_DEPRECATED< texture >& cache_texture,
		cache_DEPRECATED< mesh >& cache_mesh )
	{
		lua::type< mat4 >( stack )
			.insert( lua::mul( matrix_mul ) )
			.insert( lua::index( matrix_index ) );

		lua::type< text >( stack )
			.insert( "build", &text::build );

		lua::type< scene::graph >( stack )
			.insert( "insert", &scene::graph::insert );

		lua::type< cache_DEPRECATED< texture > >( stack )
			.insert( lua::call( &cache_DEPRECATED< texture >::operator [] ) );

		lua::type< cache_DEPRECATED< mesh > >( stack )
			.insert( lua::call( &cache_DEPRECATED< mesh >::operator [] ) );

		lua::type< mesh >( stack )
			.insert( "get", &mesh::get );

		lua::type< scene::node >( stack )
			.insert( "blend", &scene::node::blend )
			.insert( "matrix", &scene::node::matrix );

		lua::inherit< scene::sight, scene::node >( stack )
			.insert( "update", &scene::sight::update );

		( lua::type< texture_ptr >( stack ) );
		( lua::type< video_data >( stack ) );
		( lua::inherit< scene::mesh, scene::node >( stack ) );
		( lua::inherit< scene::text, scene::node >( stack ) );
		( lua::type< video >( stack ) );

		lua::table table_scene( stack );
		table_scene[ "rotate" ] = scene::rotate;
		table_scene[ "translate" ] = scene::translate;
		table_scene[ "scale" ] = scene::scale;
		table_scene[ "mesh" ] =
			lua::constructor< scene::mesh, texture&, const video_data& >( stack );
		table_scene[ "text" ] =
			lua::constructor< scene::text, const texture_ptr&, const video_data& >( stack );
		table_scene[ "sight" ] =
			lua::constructor< scene::sight, const video&, const video_data&, u16, u16 >( stack );

		lua::table table_cache( stack );
		table_cache[ "texture" ] = &cache_texture;
		table_cache[ "mesh" ] = &cache_mesh;

		lua::table global( stack, lua::global );
		global[ "scene" ] = table_scene;
		global[ "cache" ] = table_cache;
	}
}
