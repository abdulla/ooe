/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#include "foundation/math/math.hpp"
#include "test/unit/check.hpp"
#include "test/unit/group.hpp"

OOE_ANONYMOUS_BEGIN( ( ooe ) )

typedef unit::group< anonymous_t, anonymous_t, 7 > group_type;
typedef group_type::fixture_type fixture_type;
group_type group( "math" );

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe )( unit ) )

template<>
template<>
    void fixture_type::test< 0 >( anonymous_t& )
{
    std::cerr << "is_equal\n";

    OOE_CHECK( "1 == 1", is_equal( 1, 1 ) );
    OOE_CHECK( "1 != 2", !is_equal( 1, 2 ) );
}

template<>
template<>
    void fixture_type::test< 1 >( anonymous_t& )
{
    std::cerr << "radian\n";

    OOE_CHECK( "1 rad == pi", is_equal( radian( pi ), pi ) );
    OOE_CHECK( "180 deg == pi", is_equal( radian( degree( 180 ) ), pi ) );
}

template<>
template<>
    void fixture_type::test< 2 >( anonymous_t& )
{
    std::cerr << "degree\n";

    OOE_CHECK( "180 deg == 180", is_equal( degree( 180 ), 180 ) );
    OOE_CHECK( "180 deg == pi", is_equal( degree( radian( pi ) ), 180 ) );
}

template<>
template<>
    void fixture_type::test< 3 >( anonymous_t& )
{
    std::cerr << "vector< 3 >\n";

    vec3 v( 0, 1, 2 );
    OOE_CHECK( "v.x == 0", is_equal( v.x, 0 ) );
    OOE_CHECK( "v.y == 1", is_equal( v.y, 1 ) );
    OOE_CHECK( "v.z == 2", is_equal( v.z, 2 ) );
    OOE_CHECK( "v == v", v == v );

    vec3 u( 2, 1, 0 );
    OOE_CHECK( "v != u", v != u );

    vec3 t = -v;
    OOE_CHECK( "-v = { 0 -1 -2 }", t == vec3( 0, -1, -2 ) );

    t = v - u;
    OOE_CHECK( "v - u == { -2 0 2 }", t == vec3( -2, 0, 2 ) );

    t = v + u;
    OOE_CHECK( "v + u == { 2 2 2 }", t == vec3( 2, 2, 2 ) );

    t = v / 2;
    OOE_CHECK( "v / 2 == { 0 .5 1 }", t == vec3( 0, .5, 1 ) );

    t = v * 2;
    OOE_CHECK( "v * 2 == { 0 2 4 }", t == vec3( 0, 2, 4 ) );

    f32 f = magnitude_squared( v );
    OOE_CHECK( "| v |\u00b2 == 5", is_equal( f, 5 ) );

    f = magnitude( v );
    OOE_CHECK( "| v | == 2.23606797", is_equal( f, 2.23606797 ) );

    f = sum( v );
    OOE_CHECK( "\u03a3 v == 3", is_equal( f, 3 ) );

    f = dot( v, u );
    OOE_CHECK( "v . u == 1", is_equal( f, 1 ) );

    t = cross( v, u );
    OOE_CHECK( "v x u == { -2 4 -2 }", t == vec3( -2, 4, -2 ) );

    t = normalise( v );
    OOE_CHECK( "|| v || == { 0 0.44721359 0.89442719 }", t == vec3( 0, 0.44721359, 0.89442719 ) );

    f32 c[ 3 ];
    column( c, v );
    OOE_CHECK( "c[ 0 ] == 0", is_equal( c[ 0 ], 0 ) );
    OOE_CHECK( "c[ 1 ] == 1", is_equal( c[ 1 ], 1 ) );
    OOE_CHECK( "c[ 2 ] == 2", is_equal( c[ 2 ], 2 ) );

    f32 d[] = { 2, 1, 0 };
    OOE_CHECK( "u == { 2 1 0 }", u == column( d ) );
}

template<>
template<>
    void fixture_type::test< 4 >( anonymous_t& )
{
    std::cerr << "matrix< 3 >\n";

    mat4 n( 0, 1, 2, 9,
            3, 4, 5, 9,
            6, 7, 8, 9,
            9, 9, 9, 9 );

    mat3 m( n );
    OOE_CHECK( "m == { 0 1 2 3 4 5 6 7 8 }",
        m == mat3(
        0, 1, 2,
        3, 4, 5,
        6, 7, 8 ) );

    m = -m;
    OOE_CHECK( "-m == { 0 -1 -2 -3 -4 -5 -6 -7 -8 }",
        m == mat3(
        0, -1, -2,
        -3, -4, -5,
        -6, -7, -8 ) );

    m = m + mat3( n );
    OOE_CHECK( "m - m == mat3::zero", m == mat3::zero );

    m = mat3( n ) / 2;
    OOE_CHECK( "m / 2 == { 0 .5 1 1.5 2 2.5 3 3.5 4 }",
        m == mat3(
        0, .5, 1,
        1.5, 2, 2.5,
        3, 3.5, 4 ) );

    m = mat3( n ) * 2;
    OOE_CHECK( "m * 2 == { 0 2 4 6 8 10 12 14 16 }",
        m == mat3(
        0, 2, 4,
        6, 8, 10,
        12, 14, 16 ) );

    m = mat3( n ) * vec3( 0, 1, 2 );
    OOE_CHECK( "m * v == { 0 0 0 3 4 5 12 14 16 }",
        m == mat3(
        0, 0, 0,
        3, 4, 5,
        12, 14, 16 ) );

    m = mat3( n ) * mat3( n );
    OOE_CHECK( "m * m == { 15 18 21 42 54 66 69 90,111 }",
        m == mat3(
        15, 18, 21,
        42, 54, 66,
        69, 90, 111 ) );

    vec3 v = dot( mat3( n ), vec3( 0, 1, 2 ) );
    OOE_CHECK( "m . v == { 5 14 23 }", v == vec3( 5, 14, 23 ) );

    v = dot( vec3( 0, 1, 2 ), mat3( n ) );
    OOE_CHECK( "v . m == { 15 18 21 }", v == vec3( 15, 18, 21 ) );

    m = transpose( mat3( n ) );
    OOE_CHECK( "m T == { 0 3 6 1 4 7 2 5 8 }",
        m == mat3(
        0, 3, 6,
        1, 4, 7,
        2, 5, 8 ) );

    mat4 o = translate( mat3( n ), vec3( 0, 1, 2 ) );
    OOE_CHECK( "translate m v == { 0 1 2 0 3 4 5 0 6 7 8 0 5 18 21 1 }",
        o == mat4(
        0, 1, 2, 0,
        3, 4, 5, 0,
        6, 7, 8, 0,
        15, 18, 21, 1 ) );

    m = axis_angle< mat3 >( vec3( .5, 1, 0 ), degree( 90 ) );
    OOE_CHECK( "axis-angle v 90\u00b0 == { .25 .5 -1 .5 1 .5 1 -.5 0 }",
        m == mat3(
        .25, .5, -1,
        .5, 1, .5,
        1, -.5, 0 ) );

    m = euler_angle< mat3 >( degree( 90 ), degree( 90 ), degree( 90 ) );
    OOE_CHECK( "euler-angle 90\u00b0 90\u00b0 90\u00b0 == { 0 1 0 1 0 0 0 0 -1 }",
        m == mat3(
        0, 1, 0,
        1, 0, 0,
        0, 0, -1 ) );
}

template<>
template<>
    void fixture_type::test< 5 >( anonymous_t& )
{
    std::cerr << "matrix< 4 >\n";

    mat3 n( 0, 1, 2,
            3, 4, 5,
            6, 7, 8 );

    mat4 m( n );
    OOE_CHECK( "m == { 0 1 2 0 3 4 5 0 6 7 8 0 0 0 0 1 }",
        m == mat4(
        0, 1, 2, 0,
        3, 4, 5, 0,
        6, 7, 8, 0,
        0, 0, 0, 1 ) );

    m = -m;
    OOE_CHECK( "-m = { 0 -1 -2 0 -3 -4 -5 0 -6 -7 -8 0 0 0 0 -1 }",
        m == mat4(
        0, -1, -2, 0,
        -3, -4, -5, 0,
        -6, -7, -8, 0,
        0, 0, 0, -1 ) );

    m = m + mat4( n );
    OOE_CHECK( "m - m == mat4::zero", m == mat4::zero );

    m = mat4( n ) / 2;
    OOE_CHECK( "m / 2 == { 0 .5 1 0 1.5 2 2.5 0 3 3.5 4 0 0 0 0 .5 }",
        m == mat4(
        0, .5, 1, 0,
        1.5, 2, 2.5, 0,
        3, 3.5, 4, 0,
        0, 0, 0, .5 ) );

    m = mat4( n ) * 2;
    OOE_CHECK( "m * 2 == { 0 2 4 0 6 8 10 0 12 14 16 0 0 0 0 2 }",
        m == mat4(
        0, 2, 4, 0,
        6, 8, 10, 0,
        12, 14, 16, 0,
        0, 0, 0, 2 ) );

    m = mat4( n ) * mat4( n );
    OOE_CHECK( "m * m == { 15 18 21 0 42 54 66 0 69 90 111 0 0 0 0 1 }",
        m == mat4(
        15, 18, 21, 0,
        42, 54, 66, 0,
        69, 90, 111, 0,
        0, 0, 0, 1 ) );

    vec3 v = dot( mat4( n ), vec3( 0, 1, 2 ) );
    OOE_CHECK( "m . v == { 5 14 23 }", v == vec3( 5, 14, 23 ) );

    v = dot( vec3( 0, 1, 2 ), mat4( n ) );
    OOE_CHECK( "v . m == { 15 18 21 }", v == vec3( 15, 18, 21 ) );

    m = transpose( mat4( n ) );
    OOE_CHECK( "m T == { 0 3 6 0 1 4 7 0 2 5 8 0 0 0 0 1 }",
        m == mat4(
        0, 3, 6, 0,
        1, 4, 7, 0,
        2, 5, 8, 0,
        0, 0, 0, 1 ) );

    m = translate( mat4( n ), vec3( 0, 1, 2 ) );
    OOE_CHECK( "translate m v == { 0 1 2 0 3 4 5 0 6 7 8 0 15 18 21 1 }",
        m == mat4(
        0, 1, 2, 0,
        3, 4, 5, 0,
        6, 7, 8, 0,
        15, 18, 21, 1 ) );

    m = scale( mat4( n ), vec3( 0, 1, 2 ) );
    OOE_CHECK( "scale m v == { 0 1 4 0 0 4 10 0 0 7 16 0 0 0 0 1 }",
        m == mat4(
        0, 1, 4, 0,
        0, 4, 10, 0,
        0, 7, 16, 0,
        0, 0, 0, 1 ) );

    m = perspective( degree( 45 ), 4. / 3., 1, 100 );
    OOE_CHECK( "perspective 45\u00b0 4/3 1 100 == "
        "{ 1.810660 0 0 0 0 2.414213 0 0 0 0 -1.020202 -1 0 0 -2.020202 0 }",
        m == mat4(
        1.810660, 0, 0, 0,
        0, 2.414213, 0, 0,
        0, 0, -1.020202, -1,
        0, 0, -2.020202, 0 ) );

    m = orthographic( -1, 1, -1, 1 );
    OOE_CHECK( "orthographic -1 1 -1 1 == { 1 0 0 0 0 1 0 0 0 0 -1 0 0 0 0 1 }",
        m == mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, -1, 0,
        0, 0, 0, 1 ) );

    m = look_at( vec3::zero, vec3::unit_x, vec3::unit_y );
    OOE_CHECK( "look-at v0 vx vy == { 0 0 -1 0 0 1 0 0 1 0 0 0 0 0 0 1 }",
        m == mat4(
        0, 0, -1, 0,
        0, 1, 0, 0,
        1, 0, 0, 0,
        0, 0, 0, 1 ) );
}

template<>
template<>
    void fixture_type::test< 6 >( anonymous_t& )
{
    std::cerr << "quaternion\n";

    quat q( 0, 1, 2, 3 );
    OOE_CHECK( "q.x == 0", is_equal( q.x, 0 ) );
    OOE_CHECK( "q.y == 1", is_equal( q.y, 1 ) );
    OOE_CHECK( "q.z == 2", is_equal( q.z, 2 ) );
    OOE_CHECK( "q.w == 3", is_equal( q.w, 3 ) );

    quat r( 3, 2, 1, 0 );
    OOE_CHECK( "q != r", q != r );

    quat s = -q;
    OOE_CHECK( "-q == { 0 -1 -2 -3 }", s == quat( 0, -1, -2, -3 ) );

    s = q - r;
    OOE_CHECK( "q - r == { -3 -1 1 3 }", s == quat( -3, -1, 1, 3 ) );

    s = q + r;
    OOE_CHECK( "q + r == { 3 3 3 3 }", s == quat( 3, 3, 3, 3 ) );

    s = q / 2;
    OOE_CHECK( "q / 2 == { 0 .5 1 1.5 }", s == quat( 0, .5, 1, 1.5 ) );

    s = q * 2;
    OOE_CHECK( "q * 2 == { 0 2 4 6 }", s == quat( 0, 2, 4, 6 ) );

    vec3 v = quat::identity * vec3( 0, 1, 2 );
    OOE_CHECK( "quat::identity * v == { 0 1 2 }", v == vec3( 0, 1, 2 ) );

    s = q * quat::identity;
    OOE_CHECK( "q * quat::identity == q", s == q );

    f32 f = dot( q, r );
    OOE_CHECK( "q . r == 4", is_equal( f, 4 ) );

    f = magnitude_squared( q );
    OOE_CHECK( "| q |\u00b22 == 14", is_equal( f, 14 ) );

    f = magnitude( q );
    OOE_CHECK( "| q | == 3.74165738", is_equal( f, 3.74165738 ) );

    f = sum( q );
    OOE_CHECK( "\u03a3 q == 6", is_equal( f, 6 ) );

    s = normalise( q );
    OOE_CHECK( "|| q || == { 0, 0.26726124, 0.53452247, 0.80178368 }",
        s == quat( 0, 0.26726124, 0.53452247, 0.80178368 ) );

    s = conjugate( q );
    OOE_CHECK( "q* == { 0 -1 -2 3 }", s == quat( 0, -1, -2, 3 ) );

    s = inverse( q );
    OOE_CHECK( "q\u207b\u00b9 == { 0, -0.07142858, -0.14285715, 0.21428572 }",
        s == quat( 0, -0.07142858, -0.14285715, 0.21428572 ) );

    mat3 m = to_matrix( quat::z_180 );
    OOE_CHECK( "to-matrix q == m",
        m == mat3(
        -1, 0, 0,
        0, -1, 0,
        0, 0, 1 ) );

    s = rotate( vec3::unit_x, vec3::unit_y );
    OOE_CHECK( "rotate vec3::unit_x vec3::unit_y == { 0, 0, 0.70710677, 0.70710677 }",
        s == quat( 0, 0, 0.70710677, 0.70710677 ) );

    s = slerp( .5, quat::x_180, quat::y_180 );
    OOE_CHECK( "slerp .5 quat::x_180 quat::y_180 == { 0.70710677, 0.70710677, 0, 0 }",
        s == quat( 0.70710677, 0.70710677, 0, 0 ) );

    s = squad( .5, quat::x_180, quat::y_180, quat::y_180, quat::z_180 );
    OOE_CHECK( "squad .5 quat::x_180 quat::y_180 quat::x_180, quat::z_180 == "
        "{ 0.49999997, 0.70710677, 0.49999997, 0 }",
        s == quat( 0.49999997, 0.70710677, 0.49999997, 0 ) );

    s = axis_angle< quat >( vec3::unit_x, degree( 180 ) );
    OOE_CHECK( "axis-angle vec3::unit_x 180\u00b0 == quat::x_180", s == quat::x_180 );

    s = euler_angle< quat >( degree( 180 ), 0, 0 );
    OOE_CHECK( "euler-angle 180\u00b0 0 0 == quat::x_180", s == quat::x_180 );
}

OOE_NAMESPACE_END( ( ooe )( unit ) )
