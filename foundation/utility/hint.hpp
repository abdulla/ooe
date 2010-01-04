/* Copyright (C) 2010 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_HINT_HPP
#define OOE_FOUNDATION_UTILITY_HINT_HPP

#include "foundation/utility/traits.hpp"

OOE_NAMESPACE_BEGIN( ( ooe ) )

//--- proxy_ptr ------------------------------------------------------------------------------------
template< typename t >
	class proxy_ptr
{
public:
	typedef t value_type;
	typedef t* pointer;
	typedef t& reference;

	operator pointer( void ) const
	{
		return value;
	}

	pointer operator ->( void ) const
	{
		return value;
	}

	reference operator *( void ) const
	{
		return *value;
	}

protected:
	proxy_ptr( pointer value_ )
		: value( value_ )
	{
	}

private:
	pointer value;
};

//--- construct_ptr --------------------------------------------------------------------------------
template< typename t >
	struct construct_ptr
	: public proxy_ptr< t >
{
	construct_ptr( t* value_ = 0 )
		: proxy_ptr< t >( value_ )
	{
	}
};

//--- destruct_ptr ---------------------------------------------------------------------------------
template< typename t >
	struct destruct_ptr
	: public proxy_ptr< t >
{
	destruct_ptr( t* value_ = 0 )
		: proxy_ptr< t >( value_ )
	{
	}
};

//--- is_construct ---------------------------------------------------------------------------------
template< typename t >
	struct is_construct
	: public is_template1< t, construct_ptr >
{
};

//--- is_destruct ----------------------------------------------------------------------------------
template< typename t >
	struct is_destruct
	: public is_template1< t, destruct_ptr >
{
};

OOE_NAMESPACE_END( ( ooe ) )

#endif	// OOE_FOUNDATION_UTILITY_HINT_HPP
