/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_BUFFER_HPP
#define OOE_FOUNDATION_OPENGL_BUFFER_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

class map
    : public ooe::map
{
public:
    map( void*, up_t, u32 );
    virtual ~map( void );

private:
    const u32 target;
};

struct buffer
    : public ooe::buffer
{
    const u32 id;
    const u32 target;
    const up_t size;

    buffer( up_t, type, usage_type );
    virtual ~buffer( void );

    virtual map_ptr map( access_type ) const;
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif  // OOE_FOUNDATION_OPENGL_BUFFER_HPP
