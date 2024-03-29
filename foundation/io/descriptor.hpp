/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IO_DESCRIPTOR_HPP
#define OOE_FOUNDATION_IO_DESCRIPTOR_HPP

#include "foundation/utility/macro.hpp"
#include "foundation/utility/pointer.hpp"
#include "foundation/utility/string.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

struct descriptor_id
{
    const s32 fd;

    descriptor_id( s32 );
    descriptor_id( const std::string&, s32 );
    ~descriptor_id( void );
};

class OOE_VISIBLE descriptor
{
public:
    enum
    {
        read        = 1 << 0,
        write       = 1 << 1,
        append      = 1 << 2,
        truncate    = 1 << 3,
        create      = 1 << 4,

        write_new   = write | truncate | create,
        read_write  = read | write | create
    };

    enum node_type
    {
        unknown,

        directory,
        character,
        block,
        file,
        pipe,
        link,
        socket
    };

    explicit descriptor( s32 );
    descriptor( const std::string&, u8 = read );
    ~descriptor( void );

    s32 get( void ) const;
    node_type type( void ) const;
    up_t size( void ) const;
    void resize( up_t );

private:
    shared_ptr< const descriptor_id > id;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_IO_DESCRIPTOR_HPP
