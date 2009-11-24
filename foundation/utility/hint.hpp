/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_UTILITY_HINT_HPP
#define OOE_FOUNDATION_UTILITY_HINT_HPP

#include "foundation/utility/traits.hpp"

namespace ooe
{
//--- proxy_ptr ----------------------------------------------------------------
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

//--- construct_ptr ------------------------------------------------------------
	template< typename t >
		struct construct_ptr
		: public proxy_ptr< t >
	{
		construct_ptr( t* value_ = 0 )
			: proxy_ptr< t >( value_ )
		{
		}
	};

//--- destruct_ptr -------------------------------------------------------------
	template< typename t >
		struct destruct_ptr
		: public proxy_ptr< t >
	{
		destruct_ptr( t* value_ = 0 )
			: proxy_ptr< t >( value_ )
		{
		}
	};

//--- is_construct -------------------------------------------------------------
	template< typename t >
		struct is_construct
#if __GNUC__ >=4 && __GNUC_MINOR__ >= 2 && __GNUC_PATCHLEVEL__ >= 4
		: public is_template1< t, construct_ptr >
	{
	};
#else
	{
		template< typename >
			struct apply
			: public false_type
		{
		};

		template< typename t0 >
			struct apply< construct_ptr< t0 > >
			: public true_type
		{
		};

		static const bool value = apply< typename no_ref< t >::type >::value;
	};
#endif

//--- is_destruct --------------------------------------------------------------
	template< typename t >
		struct is_destruct
#if __GNUC__ >=4 && __GNUC_MINOR__ >= 2 && __GNUC_PATCHLEVEL__ >= 4
		: public is_template1< t, destruct_ptr >
	{
	};
#else
	{
		template< typename >
			struct apply
			: public false_type
		{
		};

		template< typename t0 >
			struct apply< destruct_ptr< t0 > >
			: public true_type
		{
		};

		static const bool value = apply< typename no_ref< t >::type >::value;
	};
#endif
}

#endif	// OOE_FOUNDATION_UTILITY_HINT_HPP
