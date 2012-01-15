/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#include <algorithm>
#include <cmath>

#include "foundation/math/camera.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- camera::limits -------------------------------------------------------------------------------
camera::limits::limits( radian fov, f32 aspect_, f32 near_, f32 far_ )
    : aspect( aspect_ ), near( near_ ), far( far_ ), tangent( std::tan( fov ) ),
    sphere_x( 1 / std::cos( std::atan( tangent * aspect ) ) ),
    sphere_y( 1 / std::cos( fov ) ), projection( perspective( fov, aspect, near, far ) )
{
}

//--- camera::axes ---------------------------------------------------------------------------------
camera::axes::axes( const vec3& x_, const vec3& y_, const vec3& z_ )
    : x( x_ ), y( y_ ), z( z_ )
{
}

camera::axes camera::axes::twist( void ) const
{
    return axes( -z, y, x );
}

//--- camera ---------------------------------------------------------------------------------------
camera::camera( radian fov, f32 aspect, f32 near, f32 far )
    : limit( fov, aspect, near, far ), look( vec3::unit_x, vec3::unit_y, -vec3::unit_z ),
    axis( look ), eye( vec3::zero )
{
}

void camera::rotate( const vec3& change )
{
    const mat3 rotate_y( axis_angle< mat3 >( axis.y, change.y ) );
    axis.x = dot( rotate_y, axis.x );
    axis.z = dot( rotate_y, axis.z );

    look.x = dot( rotate_y, look.x );
    look.y = dot( rotate_y, look.y );
    look.z = dot( rotate_y, look.z );

    const mat3 rotate_z( axis_angle< mat3 >( axis.z, change.z ) );
    look.x = dot( rotate_z, look.x );
    look.y = dot( rotate_z, look.y );
    look.z = dot( rotate_z, look.z );
}

void camera::translate( const vec3& change )
{
    eye += axis.x * change.x;
    eye += axis.y * change.y;
    eye += axis.z * change.z;
}

vec3 camera::position( void ) const
{
    return -eye;
}

mat4 camera::matrix( void ) const
{
    mat3 rotation(
        look.z.x, look.y.x, look.x.x,
        look.z.y, look.y.y, look.x.y,
        look.z.z, look.y.z, look.x.z );

    return limit.projection * ooe::translate( rotation, eye );
}

aabb camera::aabb( void ) const
{
    axes a = look.twist();

    const vec3 negative_eye = -eye;
    const vec3 near_center = negative_eye - a.z * limit.near;
    const vec3 far_center = negative_eye - a.z * limit.far;

    const f32 near_height = limit.near * limit.tangent;
    const f32 near_width = limit.near * limit.aspect;
    const f32 far_height = limit.far * limit.tangent;
    const f32 far_width = limit.far * limit.aspect;

    vec3 corner[ 8 ];
    corner[ 0 ] = near_center + a.y * near_height - a.x * near_width;   // near_top_left
    corner[ 1 ] = near_center + a.y * near_height + a.x * near_width;   // near_top_right
    corner[ 2 ] = near_center - a.y * near_height - a.x * near_width;   // near_bottom_left
    corner[ 3 ] = near_center - a.y * near_height + a.x * near_width;   // near_bottom_right

    corner[ 4 ] = far_center + a.y * far_height - a.x * far_width;      // far_top_left
    corner[ 5 ] = far_center + a.y * far_height + a.x * far_width;      // far_top_right
    corner[ 6 ] = far_center - a.y * far_height - a.x * far_width;      // far_bottom_left
    corner[ 7 ] = far_center - a.y * far_height + a.x * far_width;      // far_bottom_right

    ooe::aabb box;

    for ( up_t i = 0; i != 8; ++i )
    {
        box.min.x = std::min( box.min.x, corner[ i ].x );
        box.max.x = std::max( box.max.x, corner[ i ].x );

        box.min.y = std::min( box.min.y, corner[ i ].y );
        box.max.y = std::max( box.max.y, corner[ i ].y );

        box.min.z = std::min( box.min.z, corner[ i ].z );
        box.max.z = std::max( box.max.z, corner[ i ].z );
    }

    return box;
}

bool camera::includes( const vec3& point ) const
{
    const axes a = look.twist();
    const vec3 v = -eye - point;
    const f32 z_coord = dot( v, a.z );

    // test the z coordinate
    if ( z_coord > limit.far || z_coord < limit.near )
        return false;

    const f32 y_coord = dot( v, a.y );
    const f32 y_bound = z_coord * limit.tangent;

    // test the y coordinate
    if ( y_coord > y_bound || y_coord < -y_bound )
        return false;

    const f32 x_coord = dot( v, a.x );
    const f32 x_bound = y_bound * limit.aspect;

    // test the x coordinate
    if ( x_coord > x_bound || x_coord < -x_bound )
        return false;

    return true;
}

geometry::intersection camera::includes( const sphere& sphere ) const
{
    geometry::intersection result = geometry::inside;

    const axes a = look.twist();
    const vec3 v = -eye - sphere.point;
    const f32 z_coord = dot( v, a.z );

    // test the z plane
    if ( z_coord > limit.far + sphere.radius || z_coord < limit.near - sphere.radius )
        return geometry::outside;
    else if ( z_coord > limit.far - sphere.radius || z_coord < limit.near + sphere.radius )
        result = geometry::intersect;

    const f32 y_coord = dot( v, a.y );
    const f32 y_bound = z_coord * limit.tangent;
    const f32 y_radius = sphere.radius * limit.sphere_y;

    // test the y plane
    if ( y_coord > y_bound + y_radius || y_coord < -y_bound - y_radius )
        return geometry::outside;
    else if ( y_coord > y_bound - y_radius || y_coord < -y_bound + y_radius )
        result = geometry::intersect;

    const f32 x_coord = dot( v, a.x );
    const f32 x_bound = y_bound * limit.aspect;
    const f32 x_radius = sphere.radius * limit.sphere_x;

    // test the x plane
    if ( x_coord > x_bound + x_radius || x_coord < -x_bound - x_radius )
        return geometry::outside;
    else if ( x_coord > x_bound - x_radius || x_coord < -x_bound + x_radius )
        result = geometry::intersect;

    return result;
}

OOE_NAMESPACE_END( ( ooe ) )
