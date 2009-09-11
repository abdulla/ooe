/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_TRAITS_FORWARD_HPP
#define OOE_FOUNDATION_IPC_TRAITS_FORWARD_HPP

#include <boost/static_assert.hpp>

#include "foundation/utility/string.hpp"
#include "foundation/utility/traits.hpp"

namespace ooe
{
	namespace ipc
	{
		template< typename >
			struct is_empty;

		template< typename >
			struct is_pod;

		template< typename >
			struct is_array;

		template< typename >
			struct is_specialised;

//--- ipc::size ----------------------------------------------------------------
		template< typename, typename = void >
			struct size;

		template< typename t >
			struct size< t, typename enable_if< is_empty< t > >::type >;

		template< typename t >
			struct size< t, typename enable_if< is_string< t > >::type >;

		template< typename t >
			struct size< t, typename enable_if< is_pod< t > >::type >;

		template< typename t >
			struct size< t, typename enable_if< is_array< t > >::type >;

//--- ipc::read ----------------------------------------------------------------
		template< typename, typename = void >
			struct read;

		template< typename t >
			struct read< t, typename enable_if< is_empty< t > >::type >;

		template< typename t >
			struct read< t, typename enable_if< is_string< t > >::type >;

		template< typename t >
			struct read< t, typename enable_if< is_pod< t > >::type >;

		template< typename t >
			struct read< t, typename enable_if< is_array< t > >::type >;

//--- ipc::write ---------------------------------------------------------------
		template< typename, typename = void >
			struct write;

		template< typename t >
			struct write< t, typename enable_if< is_empty< t > >::type >;

		template< typename t >
			struct write< t, typename enable_if< is_string< t > >::type >;

		template< typename t >
			struct write< t, typename enable_if< is_pod< t > >::type >;

		template< typename t >
			struct write< t, typename enable_if< is_array< t > >::type >;
	}

//--- ipc::is_empty ------------------------------------------------------------
	template< typename t >
		struct ipc::is_empty
		: public ooe::is_empty< typename no_ref< t >::type >
	{
	};

//--- ipc::is_pod --------------------------------------------------------------
	template< typename t >
		struct ipc::is_pod
	{
		static const bool value = !is_cstring< t >::value &&
			!is_specialised< typename no_ref< t >::type >::value &&
			( ooe::is_pod< typename no_ref< t >::type >::value ||
			has_trivial_copy< typename no_ref< t >::type >::value );
	};

//--- ipc::is_array ------------------------------------------------------------
	template< typename t >
		struct ipc::is_array
	{
		static const bool value = !is_pod< t >::value &&
			ooe::is_array< typename no_ref< t >::type >::value;
	};

//--- ipc::is_specialised ------------------------------------------------------
	template< typename t >
		struct ipc::is_specialised
		: public false_type
	{
	};

//--- ipc::traits: default -----------------------------------------------------
	template< typename NO_SPECIALISATION_DEFINED, typename >
		struct ipc::size
	{
		static up_t call( NO_SPECIALISATION_DEFINED )
		{
			BOOST_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
			return 0;
		}
	};

	template< typename NO_SPECIALISATION_DEFINED, typename >
		struct ipc::read
	{
		static up_t call( const u8*, NO_SPECIALISATION_DEFINED )
		{
			BOOST_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
			return 0;
		}
	};

	template< typename NO_SPECIALISATION_DEFINED, typename >
		struct ipc::write
	{
		static up_t call( u8*, NO_SPECIALISATION_DEFINED )
		{
			BOOST_STATIC_ASSERT( !sizeof( NO_SPECIALISATION_DEFINED ) );
			return 0;
		}
	};

//--- ipc::traits: empty -------------------------------------------------------
	template< typename t >
		struct ipc::size< t, typename enable_if< ipc::is_empty< t > >::type >
	{
		static up_t call( t )
		{
			return 0;
		}
	};

	template< typename t >
		struct ipc::read< t, typename enable_if< ipc::is_empty< t > >::type >
	{
		static up_t call( const u8*, t )
		{
			return 0;
		}
	};

	template< typename t >
		struct ipc::write< t, typename enable_if< ipc::is_empty< t > >::type >
	{
		static up_t call( u8*, t )
		{
			return 0;
		}
	};

//--- ipc::traits: string ------------------------------------------------------
	template< typename t >
		struct ipc::size< t, typename enable_if< is_string< t > >::type >
	{
		static up_t call( typename call_traits< t >::param_type value )
		{
			return string_size< t >::call( value ) + 1;
		}
	};

	template< typename t >
		struct ipc::read< t, typename enable_if< is_string< t > >::type >
	{
		static up_t call( const u8* buffer, typename call_traits< t >::reference value )
		{
			value = reinterpret_cast< const c8* >( buffer );
			return size< t >::call( value );
		}
	};

	template< typename t >
		struct ipc::write< t, typename enable_if< is_string< t > >::type >
	{
		static up_t call( u8* buffer, typename call_traits< t >::param_type value )
		{
			const c8* string = string_data< t >::call( value );
			up_t size = ipc::size< t >::call( value );
			std::memcpy( buffer, string, size );
			return size;
		}
	};

//--- ipc::traits: pod ---------------------------------------------------------
	template< typename t >
		struct ipc::size< t, typename enable_if< ipc::is_pod< t > >::type >
	{
		static up_t call( t )
		{
			return sizeof( t );
		}
	};

	template< typename t >
		struct ipc::read< t, typename enable_if< ipc::is_pod< t > >::type >
	{
		static up_t call( const u8* buffer, typename call_traits< t >::reference value )
		{
			value = *reinterpret_cast< const t* >( buffer );
			return size< t >::call( value );
		}
	};

	template< typename t >
		struct ipc::write< t, typename enable_if< ipc::is_pod< t > >::type >
	{
		static up_t call( u8* buffer, t value )
		{
			*reinterpret_cast< t* >( buffer ) = value;
			return size< t >::call( value );
		}
	};

//--- ipc::traits: array -------------------------------------------------------
	template< typename t >
		struct ipc::size< t, typename enable_if< ipc::is_array< t > >::type >
	{
		static up_t call( typename call_traits< t >::param_type value )
		{
			typedef typename remove_extent< typename no_ref< t >::type >::type type;

			up_t sum = 0;

			for ( up_t i = 0; i != extent< type >::value; ++i )
				sum += size< type >::call( value[ i ] );

			return sum;
		}
	};

	template< typename t >
		struct ipc::read< t, typename enable_if< ipc::is_array< t > >::type >
	{
		static up_t call( const u8* buffer, t value )
		{
			typedef typename remove_extent< typename no_ref< t >::type >::type type;
			const u8* pointer = buffer;

			for ( up_t i = 0; i != extent< type >::value; ++i )
				pointer += read< type >::call( pointer, value[ i ] );

			return pointer - buffer;
		}
	};

	template< typename t >
		struct ipc::write< t, typename enable_if< ipc::is_array< t > >::type >
	{
		static up_t call( u8* buffer, typename call_traits< t >::param_type value, up_t )
		{
			typedef typename remove_extent< typename no_ref< t >::type >::type type;
			u8* pointer = buffer;

			for ( up_t i = 0; i != extent< type >::value; ++i )
				pointer += write< type >::call( pointer, value[ i ] );

			return pointer - buffer;
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_TRAITS_FORWARD_HPP
