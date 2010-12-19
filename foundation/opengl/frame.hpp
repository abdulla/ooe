/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_FRAME_HPP
#define OOE_FOUNDATION_OPENGL_FRAME_HPP

#include <map>

#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

class default_frame
    : public ooe::frame
{
public:
    default_frame( u32, u32 );
    virtual ~default_frame( void );

    virtual void read( const std::string&, image_format::type, buffer_ptr& );
    virtual void write( const frame_ptr& );
    virtual void clear( void );

    virtual void output( const std::string&, const texture_ptr& );
    virtual void output( const std::string&, const target_ptr& );

private:
    const u32 width;
    const u32 height;
};

struct frame
    : public ooe::frame
{
    typedef std::vector< u32 > colour_vector;
    typedef std::map< std::string, s32 > location_map;

    const u32 id;
    const u32 program;
    const u32 width;
    const u32 height;
    bool do_check;
    colour_vector colours;
    location_map locations;

    frame( u32, u32, u32 );
    virtual ~frame( void );

    virtual void read( const std::string&, image_format::type, buffer_ptr& );
    virtual void write( const frame_ptr& );
    virtual void clear( void );

    virtual void output( const std::string&, const texture_ptr& );
    virtual void output( const std::string&, const target_ptr& );

    void check( void );
};

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif  // OOE_FOUNDATION_OPENGL_FRAME_HPP
