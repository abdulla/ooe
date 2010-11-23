/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_UI_CONVERT_SOURCE_HPP
#define OOE_COMPONENT_UI_CONVERT_SOURCE_HPP

#include "component/ui/virtual_texture.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- convert_source -------------------------------------------------------------------------------
class convert_source
    : public physical_source
{
public:
    typedef image ( * function_type )( const image& );

    convert_source( physical_source&, image_format::type );
    virtual ~convert_source( void );

    virtual u32 size( void ) const;
    virtual u16 page_size( void ) const;
    virtual image_format::type format( void ) const;

private:
    physical_source& source;
    image_format::type format_;
    function_type function;

    virtual image read( const pyramid_index& );
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_COMPONENT_UI_CONVERT_SOURCE_HPP
