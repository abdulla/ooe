/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_SHADER_HPP
#define OOE_FOUNDATION_OPENGL_SHADER_HPP

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

struct shader
    : public ooe::shader
{
    const u32 id;

    shader( const std::string&, type );
    virtual ~shader( void );
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif  // OOE_FOUNDATION_OPENGL_SHADER_HPP
