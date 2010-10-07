/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include <algorithm>

#include "foundation/math/geometry.hpp"

OOE_ANONYMOUS_NAMESPACE_BEGIN( ( ooe ) )

vec3 planar_max( const aabb& aabb, const vec3& normal )
{
    return vec3(
        normal.x > 0 ? aabb.max.x : aabb.min.x,
        normal.y > 0 ? aabb.max.y : aabb.min.y,
        normal.z > 0 ? aabb.max.z : aabb.min.z );
}

vec3 planar_min( const aabb& aabb, const vec3& normal )
{
    return vec3(
        normal.x < 0 ? aabb.max.x : aabb.min.x,
        normal.y < 0 ? aabb.max.y : aabb.min.y,
        normal.z < 0 ? aabb.max.z : aabb.min.z );
}

OOE_ANONYMOUS_NAMESPACE_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- sphere ---------------------------------------------------------------------------------------
sphere::sphere( void )
    : point(), radius()
{
}

sphere::sphere( const vec3& point_, f32 radius_ )
    : point( point_ ), radius( radius_ )
{
}

//--- aabb -----------------------------------------------------------------------------------------
aabb::aabb( void )
    : min(), max()
{
}

aabb::aabb( const vec3& min_, const vec3& max_ )
    : min( min_ ), max( max_ )
{
}

/*aabb center_extent( const vec3& center, const vec3& extent )
{
    return aabb( center - extent, center + extent );
}*/

aabb bounds( const f32* array, up_t length, up_t size )
{
    typedef const f32* f32_ptr;
    aabb aabb( column( array ), column( array ) );

    for ( f32_ptr i = array, end = array + length * size; i != end; i += size )
    {
        aabb.min.x = std::min( aabb.min.x, i[ 0 ] );
        aabb.max.x = std::max( aabb.max.x, i[ 0 ] );

        aabb.min.y = std::min( aabb.min.y, i[ 1 ] );
        aabb.max.y = std::max( aabb.max.y, i[ 1 ] );

        aabb.min.z = std::min( aabb.min.z, i[ 2 ] );
        aabb.max.z = std::max( aabb.max.z, i[ 2 ] );
    }

    return aabb;
}

aabb merge( const aabb& a, const aabb& b )
{
    const vec3 min(
        std::min( a.min.x, b.min.x ),
        std::min( a.min.y, b.min.y ),
        std::min( a.min.z, b.min.z ) );

    const vec3 max(
        std::max( a.max.x, b.max.x ),
        std::max( a.max.y, b.max.y ),
        std::max( a.max.z, b.max.z ) );

    return aabb( min, max );
}

geometry::intersection includes( const aabb& a, const aabb& b )
{
    if ( a.min.x > b.max.x || a.max.x < b.min.x ||
        a.min.y > b.max.y || a.max.y < b.min.y ||
        a.min.z > b.max.z || a.max.z < b.min.z )
        return geometry::outside;
    else if ( a.min.x < b.min.x && a.max.x > b.max.x &&
        a.min.y < b.min.y && a.max.y > b.max.y &&
        a.min.z < b.min.z && a.max.z > b.max.z )
        return geometry::inside;
    else
        return geometry::intersect;
}

vec3 aabb_center( const aabb& box )
{
    return ( box.max + box.min ) * .5;
}

vec3 aabb_extent( const aabb& box )
{
    return ( box.max - box.min ) * .5;
}

//--- plane ----------------------------------------------------------------------------------------
plane::plane( void )
    : n(), d()
{
}

plane::plane( const vec3& n_, f32 d_ )
    : n( n_ ), d( d_ )
{
    const f32 m = magnitude( n );
    n /= m;
    d /= m;
}

f32 operator -( const plane& plane, const vec3& vector )
{
    return plane.d + dot( plane.n, vector );
}

plane point_normal( const vec3& point, const vec3& normal )
{
    const vec3 n = normalise( normal );
    return plane( n, -dot( n, point ) );
}

plane three_point( const vec3& p0, const vec3& p1, const vec3& p2 )
{
    const vec3 v = p1 - p0;
    const vec3 u = p2 - p0;

    return point_normal( p0, cross( v, u ) );
}

//--- frustum --------------------------------------------------------------------------------------
frustum::frustum( const mat4& m )
    : f()
{
    f[ left ] = plane( vec3(
        m[ 0 ][ 3 ] + m[ 0 ][ 0 ],
        m[ 1 ][ 3 ] + m[ 1 ][ 0 ],
        m[ 2 ][ 3 ] + m[ 2 ][ 0 ] ),
        m[ 3 ][ 3 ] + m[ 3 ][ 0 ] );

    f[ right ] = plane( vec3(
        m[ 0 ][ 3 ] - m[ 0 ][ 0 ],
        m[ 1 ][ 3 ] - m[ 1 ][ 0 ],
        m[ 2 ][ 3 ] - m[ 2 ][ 0 ] ),
        m[ 3 ][ 3 ] - m[ 3 ][ 0 ] );

    f[ bottom ] = plane( vec3(
        m[ 0 ][ 3 ] + m[ 0 ][ 1 ],
        m[ 1 ][ 3 ] + m[ 1 ][ 1 ],
        m[ 2 ][ 3 ] + m[ 2 ][ 1 ] ),
        m[ 3 ][ 3 ] + m[ 3 ][ 1 ] );

    f[ top ] = plane( vec3(
        m[ 0 ][ 3 ] - m[ 0 ][ 1 ],
        m[ 1 ][ 3 ] - m[ 1 ][ 1 ],
        m[ 2 ][ 3 ] - m[ 2 ][ 1 ] ),
        m[ 3 ][ 3 ] - m[ 3 ][ 1 ] );

    f[ near ] = plane( vec3(
        m[ 0 ][ 3 ] + m[ 0 ][ 2 ],
        m[ 1 ][ 3 ] + m[ 1 ][ 2 ],
        m[ 2 ][ 3 ] + m[ 2 ][ 2 ] ),
        m[ 3 ][ 3 ] + m[ 3 ][ 2 ] );

    f[ far ] = plane( vec3(
        m[ 0 ][ 3 ] - m[ 0 ][ 2 ],
        m[ 1 ][ 3 ] - m[ 1 ][ 2 ],
        m[ 2 ][ 3 ] - m[ 2 ][ 2 ] ),
        m[ 3 ][ 3 ] - m[ 3 ][ 2 ] );
}

const plane& frustum::operator []( u8 i ) const
{
    return f[ i ];
}

plane& frustum::operator []( u8 i )
{
    return f[ i ];
}

geometry::intersection includes( const frustum& frustum, const ooe::aabb& aabb )
{
    geometry::intersection result = geometry::inside;

    for ( u8 i = 0; i < 6; i++ )
    {
        const plane& plane = frustum[ i ];

        if ( plane - planar_max( aabb, plane.n ) < 0 )
            return geometry::outside;
        else if ( plane - planar_min( aabb, plane.n ) < 0 )
            result = geometry::intersect;
    }

    return result;
}

OOE_NAMESPACE_END( ( ooe ) )
