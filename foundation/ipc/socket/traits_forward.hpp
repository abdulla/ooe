/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_SOCKET_TRAITS_FORWARD_HPP
#define OOE_FOUNDATION_IPC_SOCKET_TRAITS_FORWARD_HPP

namespace ooe
{
	namespace nipc
	{
		template< typename >
			struct is_cstring;

		template< typename >
			struct is_pod;

		template< typename >
			struct is_array;

		template< typename >
			struct is_specialised;

//--- nipc::size ---------------------------------------------------------------
		template< typename, typename = void >
			struct size;

		template< typename type >
			struct size< type, typename enable_if< is_cstring< type > >::type >;

		template< typename type >
			struct size< type, typename enable_if< is_pod< type > >::type >;

		template< typename type >
			struct size< type, typename enable_if< is_array< type > >::type >;

//--- nipc::read ---------------------------------------------------------------
		template< typename, typename = void >
			struct read;

		template< typename type >
			struct read< type, typename enable_if< is_cstring< type > >::type >;

		template< typename type >
			struct read< type, typename enable_if< is_pod< type > >::type >;

		template< typename type >
			struct read< type, typename enable_if< is_array< type > >::type >;

//--- nipc::write --------------------------------------------------------------
		template< typename, typename = void >
			struct write;

		template< typename type >
			struct write< type, typename enable_if< is_cstring< type > >::type >;

		template< typename type >
			struct write< type, typename enable_if< is_pod< type > >::type >;

		template< typename type >
			struct write< type, typename enable_if< is_array< type > >::type >;
	}

//--- nipc::is_cstring ---------------------------------------------------------
	template< typename type >
		struct nipc::is_cstring
	{
		static const bool value = is_like< type, c8* >::value || is_like< type, const c8* >::value;
	};

//--- nipc::is_pod -------------------------------------------------------------
	template< typename type >
		struct nipc::is_pod
	{
		static const bool value = !is_cstring< type >::value && !is_specialised< type >::value &&
			( ooe::is_pod< typename no_ref< type >::type >::value ||
			has_trivial_copy< typename no_ref< type >::type >::value );
			// has_trivial_copy only works with gcc 4.3+ and boost 1.35+
	};

//--- nipc::is_array -----------------------------------------------------------
	template< typename type >
		struct nipc::is_array
	{
		static const bool value = !is_pod< type >::value &&
			ooe::is_array< typename no_ref< type >::type >::value;
	};

//--- nipc::is_specialised -----------------------------------------------------
	template< typename type >
		struct nipc::is_specialised
		: public false_type
	{
	};

//--- nipc::traits: cstring ----------------------------------------------------
	template< typename type >
		struct nipc::size< type, typename enable_if< nipc::is_cstring< type > >::type >
	{
		static u32 call( const c8* value )
		{
			return std::char_traits< c8 >::length( value ) + 1;
		}
	};

	template< typename type >
		struct nipc::read< type, typename enable_if< nipc::is_cstring< type > >::type >
	{
		static void call( const u8* buffer, const c8*& value )
		{
			value = reinterpret_cast< const c8* >( buffer );
		}
	};
	template< typename type >
		struct nipc::write< type, typename enable_if< nipc::is_cstring< type > >::type >
	{
		static void call( socket& socket, const c8* value, u32 length )
		{
			socket.send( value, length );
		}
	};

//--- nipc::traits: pod --------------------------------------------------------
	template< typename type >
		struct nipc::size< type, typename enable_if< nipc::is_pod< type > >::type >
	{
		typedef typename no_ref< type >::type value_type;

		static u32 call( const value_type& )
		{
			return sizeof( value_type );
		}
	};

	template< typename type >
		struct nipc::read< type, typename enable_if< nipc::is_pod< type > >::type >
	{
		typedef typename no_ref< type >::type value_type;

		static void call( const u8* buffer, value_type& value )
		{
			value = *reinterpret_cast< const value_type* >( buffer );
		}
	};

	template< typename type >
		struct nipc::write< type, typename enable_if< nipc::is_pod< type > >::type >
	{
		typedef typename no_ref< type >::type value_type;

		static void call( socket& socket, value_type value, u32 length )
		{
			socket.send( &value, length );
		}
	};

//--- nipc::traits: array ------------------------------------------------------
	template< typename type >
		struct nipc::size< type, typename enable_if< nipc::is_array< type > >::type >
	{
		typedef typename no_ref< type >::type value_type;

		static u32 call( const value_type& )
		{
			return sizeof( value_type );
		}
	};

	template< typename type >
		struct nipc::read< type, typename enable_if< nipc::is_array< type > >::type >
	{
		typedef typename no_ref< type >::type value_type;

		static void call( const u8* buffer, value_type& value )
		{
			value = *reinterpret_cast< value_type* >( buffer );
		}
	};

	template< typename type >
		struct nipc::write< type, typename enable_if< nipc::is_array< type > >::type >
	{
		typedef typename no_ref< type >::type value_type;

		static void call( socket& socket, const value_type& value, u32 length )
		{
			socket.send( &value, length );
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_SOCKET_TRAITS_FORWARD_HPP
