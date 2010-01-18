/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_EXECUTABLE_CHECKPOINT_HPP
#define OOE_FOUNDATION_EXECUTABLE_CHECKPOINT_HPP

#include <vector>

#include "foundation/io/descriptor.hpp"
#include "foundation/utility/function.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

class OOE_VISIBLE checkpoint
{
public:
    typedef function< void ( void ) > function_type;
	typedef scoped_ptr< descriptor > desc_ptr;

    checkpoint( void );

    void insert( const function_type& );
    void update( void );

private:
    typedef std::vector< function_type > vector_type;

	desc_ptr desc;
    vector_type vector;
};

OOE_NAMESPACE_END( ( ooe ) )

#endif  // OOE_FOUNDATION_EXECUTABLE_CHECKPOINT_HPP
