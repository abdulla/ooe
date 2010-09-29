/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_COMPONENT_PYTHON_VM_HPP
#define OOE_COMPONENT_PYTHON_VM_HPP

#include "component/python/header.hpp"
#include "foundation/io/descriptor.hpp"

OOE_NAMESPACE_BEGIN( ( ooe )( python ) )

//--- vm -------------------------------------------------------------------------------------------
class OOE_VISIBLE vm
{
public:
    typedef void ( * function_type )( PyObject* );

    vm( void );
    ~vm( void );

    void load( const std::string&, const descriptor& );
    void collect( void );

    std::string version( void ) const;

    void setup( function_type );

private:
    PyObject* globals;
};

OOE_NAMESPACE_END( ( ooe )( python ) )

#endif  // OOE_COMPONENT_PYTHON_VM_HPP
