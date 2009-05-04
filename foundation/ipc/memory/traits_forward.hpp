/* Copyright (C) 2009 Abdulla Kamar. All rights reserved. */

#ifndef OOE_FOUNDATION_IPC_MEMORY_TRAITS_FORWARD_HPP
#define OOE_FOUNDATION_IPC_MEMORY_TRAITS_FORWARD_HPP

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

		template< typename type >
			struct size< type, typename enable_if< is_empty< type > >::type >;

		template< typename type >
			struct size< type, typename enable_if< is_string< type > >::type >;

		template< typename type >
			struct size< type, typename enable_if< is_pod< type > >::type >;

		template< typename type >
			struct size< type, typename enable_if< is_array< type > >::type >;

//--- ipc::read ----------------------------------------------------------------
		template< typename, typename = void >
			struct read;

		template< typename type >
			struct read< type, typename enable_if< is_empty< type > >::type >;

		template< typename type >
			struct read< type, typename enable_if< is_string< type > >::type >;

		template< typename type >
			struct read< type, typename enable_if< is_pod< type > >::type >;

		template< typename type >
			struct read< type, typename enable_if< is_array< type > >::type >;

//--- ipc::write ---------------------------------------------------------------
		template< typename, typename = void >
			struct write;

		template< typename type >
			struct write< type, typename enable_if< is_empty< type > >::type >;

		template< typename type >
			struct write< type, typename enable_if< is_string< type > >::type >;

		template< typename type >
			struct write< type, typename enable_if< is_pod< type > >::type >;

		template< typename type >
			struct write< type, typename enable_if< is_array< type > >::type >;
	}

//--- ipc::is_empty ------------------------------------------------------------
	template< typename type >
		struct ipc::is_empty
		: public ooe::is_empty< typename no_ref< type >::type >
	{
	};

//--- ipc::is_pod --------------------------------------------------------------
	template< typename type >
		struct ipc::is_pod
	{
		static const bool value = !is_cstring< type >::value &&
			!is_specialised< typename no_ref< type >::type >::value &&
			( ooe::is_pod< typename no_ref< type >::type >::value ||
			has_trivial_copy< typename no_ref< type >::type >::value );
	};

//--- ipc::is_array ------------------------------------------------------------
	template< typename type >
		struct ipc::is_array
	{
		static const bool value = !is_pod< type >::value &&
			ooe::is_array< typename no_ref< type >::type >::value;
	};

//--- ipc::is_specialised ------------------------------------------------------
	template< typename type >
		struct ipc::is_specialised
		: public false_type
	{
	};

//--- ipc::traits: empty -------------------------------------------------------
	template< typename t >
		struct ipc::size< t, typename enable_if< ipc::is_empty< t > >::type >
	{
		static u32 call( t )
		{
			return 0;
		}
	};

	template< typename t >
		struct ipc::read< t, typename enable_if< ipc::is_empty< t > >::type >
	{
		static void call( t, const u8* )
		{
		}
	};

	template< typename t >
		struct ipc::write< t, typename enable_if< ipc::is_empty< t > >::type >
	{
		static void call( t, u32, u8* )
		{
		}
	};

//--- ipc::traits: string ------------------------------------------------------
	template< typename t >
		struct ipc::size< t, typename enable_if< is_string< t > >::type >
	{
		static u32 call( typename call_traits< t >::param_type value )
		{
			return string_size< t >::call( value ) + 1;
		}
	};

	template< typename t >
		struct ipc::read< t, typename enable_if< is_string< t > >::type >
	{
		static void call( t& value, const u8* buffer )
		{
			value = buffer;
		}
	};

	template< typename t >
		struct ipc::write< t, typename enable_if< is_string< t > >::type >
	{
		static void call( typename call_traits< t >::param_type value, u32 size, u8* buffer )
		{
			const c8* string = string_data< t >::call( in );
			std::copy( string, string + size, buffer );
		}
	};

//--- ipc::traits: pod ---------------------------------------------------------
	template< typename t >
		struct ipc::replace< t, typename enable_if< ipc::is_pod< t > >::type >
	{
		static u32 call( type )
	};

	template< typename type >
		struct ipc::pack< type, typename enable_if< ipc::is_pod< type > >::type >
	{
		typedef typename replace< type >::type value_type;

		static void call( value_type in, value_type& out, buffer_pack& )
		{
			out = in;
		}
	};

	template< typename type >
		struct ipc::unpack< type, typename enable_if< ipc::is_pod< type > >::type >
	{
		typedef typename replace< type >::type value_type;

		static void call( value_type in, value_type& out, const buffer_unpack& )
		{
			out = in;
		}
	};

//--- ipc::traits: array -------------------------------------------------------
	template< typename t >
		struct ipc::replace< t, typename enable_if< ipc::is_array< t > >::type >
	{
		typedef typename no_ref< t >::type value_type;
		static const up_t size = extent< value_type >::value;
		typedef typename replace< typename remove_extent< value_type >::type >::type type[ size ];
	};

	template< typename t >
		struct ipc::pack< t, typename enable_if< ipc::is_array< t > >::type >
	{
		typedef typename no_ref< t >::type type;
		typedef typename replace< t >::type value_type;

		static void call( const type in, value_type out, buffer_pack& buffer_pack )
		{
			typedef typename remove_extent< value_type >::type element_type;

			for ( up_t i = 0; i != extent< type >::value; ++i )
				pack< element_type >::call( in[ i ], out[ i ], buffer_pack );
		}
	};

	template< typename t >
		struct ipc::unpack< t, typename enable_if< ipc::is_array< t > >::type >
	{
		typedef typename no_ref< t >::type type;
		typedef typename replace< t >::type value_type;

		static void call( const value_type in, type out, const buffer_unpack& buffer_unpack )
		{
			typedef typename remove_extent< value_type >::type element_type;

			for ( up_t i = 0; i != extent< type >::value; ++i )
				unpack< element_type >::call( in[ i ], out[ i ], buffer_unpack );
		}
	};
}

#endif	// OOE_FOUNDATION_IPC_MEMORY_TRAITS_FORWARD_HPP
