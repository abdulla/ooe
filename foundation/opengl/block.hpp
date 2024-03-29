/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_BLOCK_HPP
#define OOE_FOUNDATION_OPENGL_BLOCK_HPP

#include <map>

#include "foundation/math/math.hpp"
#include "foundation/utility/tuple.hpp"
#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

class block
    : public ooe::block
{
public:
    typedef void ( * uniform_function )( s32, u32, const void* );
    typedef shared_array< u8 > uniform_array;
    typedef tuple< uniform_function, u32, uniform_array > uniform_tuple;
    typedef std::map< s32, uniform_tuple > uniform_map;
    typedef tuple< texture_ptr, texture_array_ptr > texture_tuple;
    typedef std::map< s32, texture_tuple > texture_map;
    typedef tuple< s32, type, bool /* divisor */, bool /* pointer */ > buffer_tuple;
    typedef std::multimap< buffer_ptr, buffer_tuple > buffer_map;
    typedef std::map< std::string, s32 > location_map;

    u32 id;
    const u32 program;
    const buffer_ptr index;
    bool rebuild;

    uniform_map uniforms;
    texture_map textures;
    buffer_map buffers;

    block( u32, const buffer_ptr& );
    virtual ~block( void );

    virtual void input( const std::string&, s32[][ 1 ], u32 );
    virtual void input( const std::string&, s32[][ 2 ], u32 );
    virtual void input( const std::string&, s32[][ 3 ], u32 );
    virtual void input( const std::string&, s32[][ 4 ], u32 );
    virtual void input( const std::string&, f32[][ 1 ], u32 );
    virtual void input( const std::string&, f32[][ 2 ], u32 );
    virtual void input( const std::string&, f32[][ 3 ], u32 );
    virtual void input( const std::string&, f32[][ 4 ], u32 );
    virtual void input( const std::string&, const mat3*, u32 );
    virtual void input( const std::string&, const mat4*, u32 );

    virtual void input( const std::string&, const texture_ptr& );
    virtual void input( const std::string&, const texture_array_ptr& );
    virtual void input( const std::string&, type, bool, const buffer_ptr& );

private:
    typedef std::map< s32, buffer_map::iterator > iterator_map;

    iterator_map iterators;
    location_map locations;
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif  // OOE_FOUNDATION_OPENGL_BLOCK_HPP
