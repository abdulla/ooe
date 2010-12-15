/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP
#define OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP

#include <bitset>
#include <list>
#include <map>

#include <tbb/concurrent_queue.h>

#include "foundation/utility/atom.hpp"
#include "foundation/utility/tuple.hpp"
#include "foundation/visual/graphics.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

class thread_pool;
class virtual_texture;

//--- pyramid_index --------------------------------------------------------------------------------
struct pyramid_index
{
    u32 x;
    u32 y;
    u8 level;

    pyramid_index( void );
    pyramid_index( u32, u32, u8 );
};

bool operator <( const pyramid_index&, const pyramid_index& );

//--- physical_source ------------------------------------------------------------------------------
struct physical_source
{
    virtual ~physical_source( void ) {}

    virtual u32 size( void ) const = 0;
    virtual u16 page_size( void ) const = 0;
    virtual image_format::type format( void ) const = 0;

    virtual image read( const pyramid_index& ) = 0;
};

//--- page_cache -----------------------------------------------------------------------------------
class page_cache
    : private noncopyable
{
public:
    typedef tuple< virtual_texture*, pyramid_index > key_type;
    typedef tuple< u16, key_type, bool > cache_type;
    typedef std::list< cache_type > cache_list;
    typedef std::multimap< virtual_texture*, cache_list::iterator > page_map;

    page_cache( const device_ptr&, thread_pool&, u16, image_format::type );

    u16 page_size( void ) const;
    image_format::type format( void ) const;
    up_t pending( void ) const;

    void write( void );

private:
    typedef std::map< key_type, cache_list::iterator > cache_map;
    typedef tuple< key_type, bool, atom_ptr< image > > pending_type;
    typedef tbb::concurrent_queue< pending_type > pending_queue;

    thread_pool& pool;

    const u16 page_size_;
    const image_format::type format_;
    texture_array_ptr array;

    cache_list list;
    cache_map map;
    page_map pages;

    up_t loads;
    pending_queue queue;

    void read( virtual_texture&, pyramid_index, bool );
    void load( virtual_texture&, const pyramid_index&, bool );
    void unlock( virtual_texture&, const pyramid_index& );
    void evict( virtual_texture& );

    friend class virtual_texture;
};

//--- virtual_texture ------------------------------------------------------------------------------
class virtual_texture
    : private noncopyable
{
public:
    virtual_texture( const device_ptr&, page_cache&, physical_source& );
    ~virtual_texture( void );

    void input( block_ptr&, const std::string& ) const;
    void load( u32, u32, u32, u32, u8, bool = false );
    void unlock( u32, u32, u32, u32, u8 );

private:
    typedef std::bitset< sizeof( up_t ) * 8 > table_bitset;

    page_cache& cache;
    physical_source& source;

    image_pyramid pyramid;
    texture_ptr table;
    table_bitset bitset;

    void write( void );

    friend class page_cache;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_VIRTUAL_TEXTURE_HPP
