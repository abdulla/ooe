/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_VISUAL_GRAPHICS_HPP
#define OOE_FOUNDATION_VISUAL_GRAPHICS_HPP

#include <vector>

#include "foundation/image/image.hpp"
#include "foundation/utility/string.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

template< u8 >
    class matrix;

typedef matrix< 3 > mat3;
typedef matrix< 4 > mat4;
class view_data;

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

typedef shared_ptr< texture > texture_ptr;

//--- texture_array --------------------------------------------------------------------------------
struct texture_array
{
    virtual ~texture_array( void ) {}
    virtual void write( const image&, u32, u32, u32 ) = 0;
};

typedef shared_ptr< texture_array > texture_array_ptr;

//--- map ------------------------------------------------------------------------------------------
struct map
{
    void* const data;
    const up_t size;

    map( void*, up_t ) OOE_VISIBLE;
    virtual ~map( void ) {}
};

typedef shared_ptr< map > map_ptr;

//--- checked_map ----------------------------------------------------------------------------------
class OOE_VISIBLE checked_map
{
public:
    checked_map( const map_ptr& );
    void write( const void*, up_t );

private:
    map_ptr map;
    up_t cursor;
};

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
    virtual map_ptr map( access_type ) const = 0;
};

typedef shared_ptr< buffer > buffer_ptr;

//--- target ---------------------------------------------------------------------------------------
struct target
{
    virtual ~target( void ) {}
};

typedef shared_ptr< target > target_ptr;

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

typedef shared_ptr< shader > shader_ptr;
typedef std::vector< shader_ptr > shader_vector;

//--- block ----------------------------------------------------------------------------------------
struct block
{
    enum type
    {
        u8_1,
        u8_2,
        u8_3,
        u8_4,
        f32_1,
        f32_2,
        f32_3,
        f32_4,
        f32_mat3,
        f32_mat4
    };

    virtual ~block( void ) {}
    virtual void input( const std::string&, s32[][ 1 ], u32 ) = 0;
    virtual void input( const std::string&, s32[][ 2 ], u32 ) = 0;
    virtual void input( const std::string&, s32[][ 3 ], u32 ) = 0;
    virtual void input( const std::string&, s32[][ 4 ], u32 ) = 0;
    virtual void input( const std::string&, f32[][ 1 ], u32 ) = 0;
    virtual void input( const std::string&, f32[][ 2 ], u32 ) = 0;
    virtual void input( const std::string&, f32[][ 3 ], u32 ) = 0;
    virtual void input( const std::string&, f32[][ 4 ], u32 ) = 0;
    virtual void input( const std::string&, const mat3*, u32 ) = 0;
    virtual void input( const std::string&, const mat4*, u32 ) = 0;

    virtual void input( const std::string&, const texture_ptr& ) = 0;
    virtual void input( const std::string&, const texture_array_ptr& ) = 0;
    virtual void input( const std::string&, type, bool, const buffer_ptr& ) = 0;

    void input( const std::string&, s32 ) OOE_VISIBLE;
    void input( const std::string&, s32, s32 ) OOE_VISIBLE;
    void input( const std::string&, s32, s32, s32 ) OOE_VISIBLE;
    void input( const std::string&, s32, s32, s32, s32 ) OOE_VISIBLE;
    void input( const std::string&, f32 ) OOE_VISIBLE;
    void input( const std::string&, f32, f32 ) OOE_VISIBLE;
    void input( const std::string&, f32, f32, f32 ) OOE_VISIBLE;
    void input( const std::string&, f32, f32, f32, f32 ) OOE_VISIBLE;

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

typedef shared_ptr< block > block_ptr;
typedef std::vector< mat3 > mat3_vector;
typedef std::vector< mat4 > mat4_vector;

//--- frame ----------------------------------------------------------------------------------------
typedef shared_ptr< struct frame > frame_ptr;

struct frame
{
    virtual ~frame( void ) {}
    virtual void read( const std::string&, image_format::type, buffer_ptr& ) = 0;
    virtual void write( const frame_ptr& ) = 0;
    virtual void clear( void ) = 0;

    virtual void output( const std::string&, const texture_ptr& ) = 0;
    virtual void output( const std::string&, const target_ptr& ) = 0;
    virtual void output( const target_ptr& ) = 0;
};

//--- program --------------------------------------------------------------------------------------
struct program
{
    virtual ~program( void ) {}
    virtual block_ptr block( const buffer_ptr& ) const = 0;
    virtual frame_ptr frame( u32, u32 ) const = 0;
};

typedef shared_ptr< program > program_ptr;

//--- image_pyramid --------------------------------------------------------------------------------
class OOE_VISIBLE image_pyramid
    : public image_metadata
{
public:
    image_pyramid( const ooe::image& );
    image_pyramid( const image_metadata& );

    void insert( const ooe::image& );
    ooe::image image( u8 ) const;
    u8 size( void ) const;
    ooe::image::data_ptr operator []( u8 ) const;

private:
    std::vector< ooe::image::data_ptr > vector;
};

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
        array_size,
        geometry_output
    };

    virtual ~device( void ) {}
    virtual void draw( const block_ptr&, u32, const frame_ptr& ) = 0;
    virtual void swap( void ) = 0;

    virtual void set( set_type, bool ) = 0;
    virtual u32 limit( limit_type ) const = 0;

    virtual texture_ptr texture
        ( const image_pyramid&, texture::type = texture::linear, bool = true ) const = 0;
    virtual texture_array_ptr texture_array( const image_metadata&, u32 ) const = 0;
    virtual buffer_ptr buffer
        ( up_t, buffer::type, buffer::usage_type = buffer::static_write ) const = 0;
    virtual target_ptr target( const image_metadata& ) const = 0;
    virtual shader_ptr shader( const std::string&, shader::type ) const = 0;
    virtual program_ptr program( const shader_vector& ) const = 0;
    virtual frame_ptr default_frame( u32, u32 ) const = 0;
};

typedef shared_ptr< device > device_ptr;
typedef device_ptr ( device_open_type )( const view_data&, bool );

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_VISUAL_GRAPHICS_HPP
