/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_MATHS_GEOMETRY_HPP
#define OOE_FOUNDATION_MATHS_GEOMETRY_HPP

#include "foundation/maths/maths.hpp"

namespace ooe
{
	namespace geometry
	{
		enum intersection
		{
			outside,
			intersect,
			inside
		};
	}

//--- sphere -------------------------------------------------------------------
	struct OOE_VISIBLE sphere
	{
		vec3 point;
		f32 radius;

		sphere( void );
		sphere( const vec3&, f32 );
	};

//--- aabb ---------------------------------------------------------------------
	struct OOE_VISIBLE aabb
	{
		vec3 min;
		vec3 max;

		aabb( void );
		aabb( const vec3&, const vec3& );
	};

	// aabb center_extent( const vec3&, const vec3& ) OOE_VISIBLE;
	aabb bounds( const f32*, up_t, up_t ) OOE_VISIBLE;
	aabb merge( const aabb&, const aabb& ) OOE_VISIBLE;
	geometry::intersection includes( const aabb&, const aabb& ) OOE_VISIBLE;
	vec3 aabb_center( const aabb& ) OOE_VISIBLE;
	vec3 aabb_extent( const aabb& ) OOE_VISIBLE;

//--- plane --------------------------------------------------------------------
	struct OOE_VISIBLE plane
	{
		vec3 n;
		f32 d;

		plane( void );
		plane( const vec3&, f32 );
	};

	f32 operator -( const plane&, const vec3& ) OOE_VISIBLE;

	plane point_normal( const vec3&, const vec3& ) OOE_VISIBLE;
	plane three_point( const vec3&, const vec3&, const vec3& ) OOE_VISIBLE;

//--- frustum ------------------------------------------------------------------
	class OOE_VISIBLE frustum
	{
	public:
		enum
		{
			left,
			right,

			bottom,
			top,

			near,
			far
		};

		frustum( const mat4& );

		const plane& operator []( u8 ) const;
		plane& operator []( u8 );

	private:
		// stored as [ left, right, bottom, top, near, far ]
		plane f[ 6 ];
	};

	geometry::intersection includes( const frustum&, const ooe::aabb& ) OOE_VISIBLE;
}

#endif	// OOE_FOUNDATION_MATHS_GEOMETRY_HPP
