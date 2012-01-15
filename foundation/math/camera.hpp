/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_MATH_CAMERA_HPP
#define OOE_FOUNDATION_MATH_CAMERA_HPP

#include "foundation/math/geometry.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- camera ---------------------------------------------------------------------------------------
class OOE_VISIBLE camera
{
public:
    camera( radian, f32, f32 = 100, f32 = 1000000 );

    void rotate( const vec3& );
    void translate( const vec3& );

    vec3 position( void ) const;
    mat4 matrix( void ) const;
    ooe::aabb aabb( void ) const;

    bool includes( const vec3& ) const;
    geometry::intersection includes( const sphere& ) const;

private:
    struct limits
    {
        const f32 aspect;
        const f32 near;
        const f32 far;
        const f32 tangent;

        const f32 sphere_x;
        const f32 sphere_y;

        const mat4 projection;

        limits( radian, f32, f32, f32 );
    };

    struct axes
    {
        vec3 x;
        vec3 y;
        vec3 z;

        axes( const vec3&, const vec3&, const vec3& );
        axes twist( void ) const;
    };

    limits limit;
    axes look;
    axes axis;
    vec3 eye;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_MATH_CAMERA_HPP
