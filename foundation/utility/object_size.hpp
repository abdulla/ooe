/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_OBJECT_SIZE_HPP
#define OOE_FOUNDATION_UTILITY_OBJECT_SIZE_HPP

#include "foundation/utility/pointer.hpp"
#include "foundation/utility/string.hpp"
#include "foundation/utility/traits.hpp"

namespace ooe
{
//--- is_scopedptr -------------------------------------------------------------
	template< typename t >
		struct is_scopedptr
		: public is_template1< t, scoped_ptr >
	{
	};

//--- is_sharedptr -------------------------------------------------------------
	template< typename t >
		struct is_sharedptr
		: public is_template1< t, shared_ptr >
	{
	};

//--- object_size --------------------------------------------------------------
	template< typename t, typename = void >
		struct object_size
	{
		static up_t call( typename call_traits< t >::param_type )
		{
			return sizeof( typename no_ref< t >::type );
		}
	};

	template< typename t >
		struct object_size< t, typename enable_if< is_string< t > >::type >
	{
		static up_t call( typename call_traits< t >::param_type value )
		{
			return string_size( value ) + sizeof( typename no_ref< t >::type );
		}
	};

	template< typename t >
		struct object_size< t, typename enable_if< is_stdcontainer< t > >::type >
	{
		static up_t call( typename call_traits< t >::param_type value )
		{
			typedef typename no_ref< t >::type type;
			typedef typename type::value_type value_type;

			return container_size< type, value_type >::
				call( value, object_size< value_type >::call );
		}
	};

	template< typename t >
		struct object_size< t, typename enable_if< is_scopedptr< t > >::type >
	{
		static up_t call( typename call_traits< t >::param_type value )
		{
			return object_size< t >::call( *value ) + sizeof( t );
		}
	};

	template< typename t >
		struct object_size< t, typename enable_if< is_sharedptr< t > >::type >
	{
		static up_t call( typename call_traits< t >::param_type value )
		{
			return object_size< typename t::value_type >::call( *value ) +
				sizeof( typename t::ref_type ) + sizeof( t );
		}
	};
}

#endif	// OOE_FOUNDATION_UTILITY_OBJECT_SIZE_HPP
