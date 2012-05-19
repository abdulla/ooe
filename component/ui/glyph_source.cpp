/* Copyright (C) 2012 Abdulla Kamar. All rights reserved. */

OOE_ANONYMOUS_BEGIN( ( ooe ) )

const u32 page_size_ = 256;
const image_format::type format_ = image_format::rgba_u8;

OOE_ANONYMOUS_END( ( ooe ) )

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- glyph_source ---------------------------------------------------------------------------------
glyph_source::glyph_source( const device_ptr& device, const font::face& face_ )
    : mutex(), face( face_ ), index_rw(), index_cache(), kerning_rw(), kerning_cache(),
    size_( device->limit( device::texture_size ) * page_size_ )
{
}

glyph_source::~glyph_source( void )
{
}

u32 glyph_source::size( void ) const
{
    return size_;
}

u16 glyph_source::page_size( void ) const
{
    return page_size_;
}

image_format::type glyph_source::format( void ) const
{
    return format_;
}

u32 glyph_source::glyph_index( u32 code_point ) const
{
    {
        read_lock read_lock( index_rw );
        index_type::const_iterator i = index_cache.find( code_point );

        if ( i != index_cache.end() )
            return i->second;
    }

    u32 index;
    {
        lock lock( mutex );
        index = face.glyph_index( code_point );
    }
    {
        write_lock lock( index_rw );
        index_cache.insert( std::make_pair( code_point, index ) );
    }
    return index;
}

u32 glyph_source::kerning( u32 left, u32 right ) const
{
    kerning_type::key_type key( left, right );
    {
        read_lock read_lock( kerning_rw );
        kerning_type::const_iterator i = kerning_cache.find( key );

        if ( i != kerning_cache.end() )
            return i->second;
    }

    u32 delta;
    {
        lock lock( mutex );
        delta = face.kerning( left, right );
    }
    {
        write_lock lock( kerning_rw );
        kerning_cache.insert( std::make_pair( key, delta ) );
    }
    return delta;
}

glyph_coord glyph_source::glyph_coord( u32 glyph_index, u16 size ) const
{
    lock lock( mutex );
    font::bitmap bitmap = face.bitmap( glyph_index, size );
    return glyph_source( bitmap.metric );
}

OOE_NAMESPACE_END( ( ooe ) )
