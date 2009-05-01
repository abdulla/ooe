/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#include "external/graph/scene.hpp"

namespace ooe
{
//--- scene --------------------------------------------------------------------
	mat4 scene::rotate( f32 x, f32 y, f32 z )
	{
		return mat4(
			axis_angle< mat3 >( vec3::unit_x, x ) *
			axis_angle< mat3 >( vec3::unit_y, y ) *
			axis_angle< mat3 >( vec3::unit_z, z ) );
	}

	mat4 scene::translate( f32 x, f32 y, f32 z )
	{
		return mat4(
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			x, y, z, 1 );
	}

	mat4 scene::scale( f32 x, f32 y, f32 z )
	{
		return mat4(
			x, 0, 0, 0,
			0, y, 0, 0,
			0, 0, z, 0,
			0, 0, 0, 1 );
	}
}
