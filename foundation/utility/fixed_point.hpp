/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_FIXED_POINT_HPP
#define OOE_FOUNDATION_UTILITY_FIXED_POINT_HPP

#include <cmath>

#include "foundation/utility/binary.hpp"
#include "foundation/utility/error.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- fx1664 ---------------------------------------------------------------------------------------
class fx1664
{
public:
    fx1664( void )
        : value( 0 )
    {
    }

    fx1664( f64 fp )
        : value( pack_float( fp ) )
    {
    }

    fx1664( u16 m, u64 f )
        : value( pack( m, f ) )
    {
    }

    fx1664& operator +=( f64 fp )
    {
        value += pack_float( fp );
        return *this;
    }

    fx1664& operator -=( f64 fp )
    {
        value -= pack_float( fp );
        return *this;
    }

    fx1664& operator +=( u16 i )
    {
        value += pack( i, 0 );
        return *this;
    }

    fx1664& operator -=( u16 i )
    {
        value -= pack( i, 0 );
        return *this;
    }

    fx1664& operator <<=( s8 s )
    {
        value <<= s;
        return *this;
    }

    fx1664& operator >>=( s8 s )
    {
        value >>= s;
        return *this;
    }

    void magnitude( u16 i )
    {
        value = pack( i, fraction() );
    }

    void fraction( u64 f )
    {
        value = pack( magnitude(), f );
    }

    void set( u64 v )
    {
        value = v;
    }

    u64 get( void ) const
    {
        return value;
    }

    u16 magnitude( void ) const
    {
        return value >> 48;
    }

    u64 fraction( void ) const
    {
        return value & 0xffffffffffff;
    }

    f64 floating_point( void ) const
    {
        return magnitude() + fraction() / f64( 0xffffffffffff );
    }

private:
    u64 value;

    static u64 pack( u16 m, u64 f )
    {
        if ( f < ( u64( 1 ) << 48 ) )
            return ( u64( m ) << 48 ) | ( f & 0xffffffffffff );

        throw error::runtime( "fx1664: " ) << "Fraction " << f << " >= 1 << 48";
    }

    static u64 pack_float( f64 fp )
    {
        if ( fp < ( 1 << 16 ) )
            return pack( fp, std::fmod( fp, 1 ) * 0xffffffffffff );

        throw error::runtime( "fx1664: " ) << "Value " << fp << " >= 1 << 16";
    }
};

inline fx1664 operator <<( fx1664 fx, s8 s )
{
    return fx <<= s;
}

inline fx1664 operator >>( fx1664 fx, s8 s )
{
    return fx >>= s;
}

inline fx1664 saturated_shift( fx1664 fx, s8 s )
{
    fx.set( saturated_shift( fx.get(), s ) );
    return fx;
}

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_UTILITY_FIXED_POINT_HPP
