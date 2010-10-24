/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VISUAL_GRAPHICS_HPP
#define OOE_FOUNDATION_VISUAL_GRAPHICS_HPP

#include <vector>

#include "foundation/image/image.hpp"
#include "foundation/utility/string.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

class vfs;
class view_data;
class image_pyramid;

template< u8 >
    class matrix;

typedef matrix< 3 > mat3;
typedef matrix< 4 > mat4;

//--- texture --------------------------------------------------------------------------------------
struct texture
{
    enum type
    {
        nearest,
        linear
    };

    virtual ~texture( void ) {}
    virtual void write( const image&, u32, u32, u8 = 0 ) = 0;
};

typedef shared_ptr< texture > texture_type;

//--- texture_array --------------------------------------------------------------------------------
struct texture_array
{
    virtual ~texture_array( void ) {}
    virtual void write( const image&, u32, u32, u32 ) = 0;
};

typedef shared_ptr< texture_array > texture_array_type;

//--- map ------------------------------------------------------------------------------------------
struct map
{
    void* const data;
    const up_t size;

    map( void*, up_t ) OOE_VISIBLE;
    virtual ~map( void ) {}
};

typedef shared_ptr< map > map_type;

//--- buffer ---------------------------------------------------------------------------------------
struct buffer
{
    enum type
    {
        point,
        index,
        pixel
    };

    enum usage_type
    {
        static_read,
        static_write,
        stream_read,
        stream_write
    };

    enum access_type
    {
        read,
        write,
        read_write
    };

    virtual ~buffer( void ) {}
    virtual map_type map( access_type ) const = 0;
};

typedef shared_ptr< buffer > buffer_type;

//--- target ---------------------------------------------------------------------------------------
struct target
{
    virtual ~target( void ) {}
};

typedef shared_ptr< target > target_type;

//--- shader ---------------------------------------------------------------------------------------
struct shader
{
    enum type
    {
        vertex,
        geometry,
        fragment
    };

    virtual ~shader( void ) {}
};

typedef shared_ptr< shader > shader_type;
typedef std::vector< shader_type > shader_vector;

//--- block ----------------------------------------------------------------------------------------
struct block
{
    virtual ~block( void ) {}
    virtual void input( const std::string&, s32[][ 1 ], u32 ) = 0;
    virtual void input( const std::string&, s32[][ 2 ], u32 ) = 0;
    virtual void input( const std::string&, s32[][ 3 ], u32 ) = 0;
    virtual void input( const std::string&, f32[][ 1 ], u32 ) = 0;
    virtual void input( const std::string&, f32[][ 2 ], u32 ) = 0;
    virtual void input( const std::string&, f32[][ 3 ], u32 ) = 0;
    virtual void input( const std::string&, const mat3*, u32 ) = 0;
    virtual void input( const std::string&, const mat4*, u32 ) = 0;

    virtual void input( const std::string&, const texture_type& ) = 0;
    virtual void input( const std::string&, const texture_array_type& ) = 0;
    virtual void input( const std::string&, u8, const buffer_type&, bool = false ) = 0;

    void input( const std::string& name, s32 x )
    {
        s32 data[] = { x };
        input( name, &data, 1 );
    }

    void input( const std::string& name, s32 x, s32 y )
    {
        s32 data[] = { x, y };
        input( name, &data, 1 );
    }

    void input( const std::string& name, s32 x, s32 y, s32 z )
    {
        s32 data[] = { x, y, z };
        input( name, &data, 1 );
    }

    void input( const std::string& name, f32 x )
    {
        f32 data[] = { x };
        input( name, &data, 1 );
    }

    void input( const std::string& name, f32 x, f32 y )
    {
        f32 data[] = { x, y };
        input( name, &data, 1 );
    }

    void input( const std::string& name, f32 x, f32 y, f32 z )
    {
        f32 data[] = { x, y, z };
        input( name, &data, 1 );
    }

    template< u8 size >
        void input( const std::string& name, const matrix< size >& m )
    {
        input( name, &m, 1 );
    }

    template< u8 size >
        void input( const std::string& name, const std::vector< matrix< size > >& m )
    {
        input( name, &m[ 0 ], m.size() );
    }
};

typedef shared_ptr< block > block_type;
typedef std::vector< mat3 > mat3_vector;
typedef std::vector< mat4 > mat4_vector;

//--- frame ----------------------------------------------------------------------------------------
typedef shared_ptr< struct frame > frame_type;

struct frame
{
    virtual ~frame( void ) {}
    virtual void read( const std::string&, image::type, buffer_type& ) = 0;
    virtual void write( const std::string&, const std::string&, const frame_type& ) = 0;
    virtual void clear( void ) = 0;

    virtual void output( const std::string&, const texture_type& ) = 0;
    virtual void output( const std::string&, const target_type& ) = 0;
};

//--- program --------------------------------------------------------------------------------------
struct program
{
    virtual ~program( void ) {}
    virtual block_type block( const buffer_type& ) const = 0;
    virtual frame_type frame( u32, u32 ) const = 0;
};

typedef shared_ptr< program > program_type;

//--- device ---------------------------------------------------------------------------------------
struct device
{
    enum set_type
    {
        depth_test,
        blend
    };

    enum limit_type
    {
        draw_buffers,
        texture_size,
        texture_units,
        array_size
    };

    virtual ~device( void ) {}
    virtual void draw( const block_type&, const frame_type&, u32 ) = 0;
    virtual void swap( void ) = 0;

    virtual void set( set_type, bool ) = 0;
    virtual u32 limit( limit_type ) const = 0;

    virtual texture_type texture
        ( const image_pyramid&, texture::type = texture::linear, bool = true ) const = 0;
    virtual texture_array_type texture_array( u32, u32, u32, image::type ) const = 0;
    virtual buffer_type buffer
        ( up_t, buffer::type, buffer::usage_type = buffer::static_write ) const = 0;
    virtual target_type target( u32, u32, image::type ) const = 0;
    virtual shader_type shader( const std::string&, shader::type ) const = 0;
    virtual program_type program( const shader_vector&, u32 = 0 ) const = 0;
    virtual frame_type default_frame( u32, u32 ) const = 0;
};

typedef shared_ptr< device > device_type;
typedef device_type ( device_open_type )( const view_data&, bool );

//--- image_pyramid --------------------------------------------------------------------------------
class OOE_VISIBLE image_pyramid
{
public:
    const u32 width;
    const u32 height;
    const ooe::image::type format;

    image_pyramid( const ooe::image& );
    image_pyramid( u32, u32, ooe::image::type );

    void insert( const ooe::image& );
    ooe::image image( u8 ) const;
    u8 size( void ) const;
    ooe::image::data_type operator []( u8 ) const;

private:
    std::vector< ooe::image::data_type > vector;
};

//--- shader_include -------------------------------------------------------------------------------
class OOE_VISIBLE shader_include
{
public:
    shader_include( const device_type&, const ooe::vfs& );
    void insert( const std::string& );
    shader_type compile( const std::string&, shader::type ) const;

private:
    const device_type& device;
    const ooe::vfs& vfs;
    std::string header;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_VISUAL_GRAPHICS_HPP
