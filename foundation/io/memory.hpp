/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_MEMORY_HPP
#define OOE_FOUNDATION_IO_MEMORY_HPP

#include "foundation/io/descriptor.hpp"
#include "foundation/utility/tuple.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- memory_id ------------------------------------------------------------------------------------
struct memory_id
{
    const up_t size;
    void* const area;

    memory_id( s32, u8, up_t, up_t );
    ~memory_id( void );
};

//--- memory ---------------------------------------------------------------------------------------
class OOE_VISIBLE memory
{
public:
    enum
    {
        none        = 0,
        read        = 1 << 0,
        write       = 1 << 1,
        execute     = 1 << 2,

        read_write  = read | write
    };

    enum advise_type
    {
        normal,
        random,
        sequential,
        will_need,
        dont_need
    };

    typedef tuple< up_t /* offset */, up_t /* size */ > region;

    memory( const descriptor&, u8 = read );
    memory( const descriptor&, u8, region );
    ~memory( void );

    void advise( advise_type ) const;
    void sync( void ) const;

    void protect( u8, region );

    up_t size( void ) const;
    void* get( void ) const;

    template< typename to >
        to* as( void ) const
    {
        return reinterpret_cast< to* >( get() );
    }

private:
    shared_ptr< memory_id > internal;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_IO_MEMORY_HPP
