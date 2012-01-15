/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_OPENGL_FRAME_HPP
#define OOE_FOUNDATION_OPENGL_FRAME_HPP

#include <map>

#include "foundation/utility/tuple.hpp"
#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( opengl ) )

//--- default_frame --------------------------------------------------------------------------------
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
    virtual void output( const target_ptr& );

private:
    const u32 width;
    const u32 height;
};

//--- frame ----------------------------------------------------------------------------------------
class frame
    : public ooe::frame
{
public:
    enum state_type
    {
        none,
        checked,
        built
    };

    typedef tuple< texture_ptr, target_ptr > attachment_tuple;
    typedef std::map< s32, attachment_tuple > attachment_map;
    typedef std::map< std::string, s32 > location_map;

    u32 id;
    const u32 program;
    const u32 width;
    const u32 height;
    state_type state;

    target_ptr depth;
    attachment_map attachments;

    frame( u32, u32, u32 );
    virtual ~frame( void );

    virtual void read( const std::string&, image_format::type, buffer_ptr& );
    virtual void write( const frame_ptr& );
    virtual void clear( void );

    virtual void output( const std::string&, const texture_ptr& );
    virtual void output( const std::string&, const target_ptr& );
    virtual void output( const target_ptr& );

private:
    location_map locations;
};

void frame_check( u32, frame::state_type& );

OOE_NAMESPACE_END( ( ooe )( opengl ) )

#endif  // OOE_FOUNDATION_OPENGL_FRAME_HPP
